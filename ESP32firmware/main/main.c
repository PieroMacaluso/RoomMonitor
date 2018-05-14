#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>
#include <math.h>
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "lwip/sockets.h"
#include "driver/gpio.h"
#include "packet.h"

/**
 * TYPE [0 MASK 0x0C]
 */
#define TYPE_MANAGEMENT       0x00
#define TYPE_CONTROL          0x01
#define TYPE_DATA             0x02

/**
 * SUBTYPE [0 MASK 0xF0]
 */
#define SUBTYPE_PROBE_REQUEST 	0b0100
#define SUBTYPE_PROBE_RESPONSE	0b0101
#define SUBTYPE_BEACON 			0b1000

#define	WIFI_CHANNEL_MAX		(13)
#define	WIFI_CHANNEL_SWITCH_INTERVAL	(500)

/**
 * DIMENSIONI BUFFER
 */
#define SIZEBUF 			120
#define SECOND_SCAN_MODE	20

/**
 *  TIMER
 */
#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
#define TIMER_DIVIDER   80               /*!< Hardware timer clock divider, 80 to get 1MHz clock to timer */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_FINE_ADJ   (0*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
#define TIMER_INTERVAL0_SEC   (SECOND_SCAN_MODE)   /*!< test interval for timer 0 */

/*
 * Parametri connessione server da configurare tramite make menuconfig impostati nel file kconfig.projbuild
 */
#define SSID CONFIG_WIFI_SSID
#define PASSPHARSE CONFIG_WIFI_PASSWORD
#define TCPServerIP CONFIG_IP_SERVER
#define TCPServerPORT CONFIG_PORT_SERVER
#define SCANChannel CONFIG_SCAN_CHANNEL
#define MESSAGE "HelloTCPServer\0"

#define BLINK_GPIO 2				//pin led

static wifi_country_t wifi_country = { .cc = "CN", .schan = 1, .nchan = 13,
		.policy = WIFI_COUNTRY_POLICY_AUTO };

/* Prototipi Funzioni */
/*static esp_err_t event_handler1(void *ctx, system_event_t *event);*/
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
/*static const char * type2str(wifi_promiscuous_pkt_type_t type);*/
const char * subtype2str(uint8_t type);
static void wifi_sniffer_packet_handler(void *buff,
		wifi_promiscuous_pkt_type_t type);
static void getMAC(char *addr, const uint8_t* data, uint16_t offset);
static void printDataSpan(uint16_t start, uint16_t size, const uint8_t* data);
float calculateDistance(signed rssi);
/*int list_packet_init(int size);
 int list_packet_update(int size);
 void list_packet_free();*/
static void timer0_init();
void wifi_connect();
static esp_err_t event_handler(void *ctx, system_event_t *event);
int tcpClient();

/* Variabili */
node_t head;
int mod = 0;							//0=> scan 1=>send server

/*Variabili per comunicazione verso server*/
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static const char *TAG = "tcp_client";

/**
 * @brief      Funzione Main che contiene la chiamata alle configurazioni iniziali e il loop principale
 */
void app_main(void) {

	/*uint8_t level = 0;*/

	/*if (list_packet_init(size_list_packet) == -1) {
	 printf("Error list_packet_init()\n");
	 return;
	 }*/
	if ((head = init_packet_list()) == NULL) {
		printf("Error init_packet_list()\n");
		return;
	}
	wifi_event_group = xEventGroupCreate();
	wifi_sniffer_init();									//init modalità scan
	vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
	wifi_sniffer_set_channel(atoi(SCANChannel));

	/* loop */
	while (true) {

		esp_wifi_set_promiscuous(true);
		mod = 0;

		printf("Inizio raccolta dati...\n");
		timer0_init();											//start timer

		while (mod == 0) {
			vTaskDelay(20 / portTICK_PERIOD_MS);				//attesa alarm
		}

		esp_wifi_set_promiscuous(false);
		printf("Fine periodo cattura.\n");
		timer_pause(TIMER_GROUP_0, 0);
		vTaskDelay(200 / portTICK_PERIOD_MS);//sostituire con l'invio del buffer

		tcpClient();	//todo sostituire MESSAGE con buffer pacchetti

	}
	free_packet_list(head);
	//todo list_packet_free();
}

/**
 * @brief      Un semplice event_handler che ritorna semore ESP_OK. Non molto utile.
 *
 * @param      ctx    The context
 * @param      event  The event
 *
 * @return     ESP_OK
 */
/*
 esp_err_t event_handler1(void *ctx, system_event_t *event) {
 return ESP_OK;
 }
 */

/**
 * @brief      Wifi sniffer init. Contiene tutte le procedure per configurare correttamente il dispositivo per lo sniffing
 */
void wifi_sniffer_init(void) {
	esp_log_level_set("wifi", ESP_LOG_NONE); 	// disable wifi driver logging
	nvs_flash_init();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	;
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
	ESP_ERROR_CHECK(esp_wifi_start());
	esp_wifi_set_promiscuous(true);
	esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

/**
 * @brief      Restituisce il MAC address.
 *
 * @param      addr    Puntatore all'array in cui sarà scritto il MAC in formato leggibile
 * @param[in]  data    Puntatore alla porzione payload del pacchetto
 * @param[in]  offset  Punto del payload in cui si trova l'indirizzo MAC richiesto
 */

static void getMAC(char *addr, const uint8_t* data, uint16_t offset) {
	sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset + 0],
			data[offset + 1], data[offset + 2], data[offset + 3],
			data[offset + 4], data[offset + 5]);
}

void setMAC(unsigned *addr, const uint8_t* data, uint16_t offset) {
	int i;
	for (i = 0; i < 6; i++) {
		addr[i] = data[offset + i];
	}
}

/**
 * @brief      Stampa i dati contenuti nel pacchetto partendo da start, fino a start+size
 *
 * @param[in]  start  The start
 * @param[in]  size   The size
 * @param[in]  data   Puntatore alla porzione payload del pacchetto
 */
static void printDataSpan(uint16_t start, uint16_t size, const uint8_t* data) {
	for (uint16_t i = start; i < start + size; i++) {
		printf("%c", data[i]);
	}
}
/**
 * @brief      Funzione Handler invocata quando viene ricevuto un pacchetto.
 *
 * @param      buff  Il buffer, o meglio il pacchetto
 * @param[in]  type  Il tipo di pacchetto
 */
void wifi_sniffer_packet_handler(void* buff, wifi_promiscuous_pkt_type_t type) {
	/*char tmp[4], tmp1[7];*/
	// Conversione del buffer in pacchetto e estrazione di tipo e sottotipo
	const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *) buff;
	uint8_t frameControl = (unsigned int) ppkt->payload[0];
	/*uint8_t version = (frameControl & 0x03) >> 0;*/
	uint8_t frameType = (frameControl & 0x0C) >> 2;
	uint8_t frameSubType = (frameControl & 0xF0) >> 4;

	// Filtraggio dei pacchetti non desiderati
	if (frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST) {
		return;
	}

	// Set Packet
	addto_packet_list(ppkt, head);

	// Stampa dei dati a video
	printf("TYPE= %s", subtype2str(frameSubType));
	printf(" RSSI: %02d", ppkt->rx_ctrl.rssi);
	printf(" Distance: %3.2fm\t", calculateDistance(ppkt->rx_ctrl.rssi));
	printf(" T: %d", ppkt->rx_ctrl.timestamp);

	char addr[] = "00:00:00:00:00:00";
	getMAC(addr, ppkt->payload, 10);
	printf(" Peer MAC: %s", addr);

	uint8_t SSID_length = ppkt->payload[25];
	if (SSID_length > 0) {
		printf(" SSID: ");
		printDataSpan(26, SSID_length, ppkt->payload);
	}
	printf("\n");
}

/**
 * @brief      Imposta il canale WiFi desiderato nel dispositivo
 *
 * @param[in]  channel  The channel
 */
void wifi_sniffer_set_channel(uint8_t channel) {
	esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

/**
 * @brief      Permette di convertire il numero del tipo in stringa personalizzabile
 *
 * @param[in]  type  The type
 *
 * @return     Stringa del tipo richiesto
 */
/*
 const char *type2str(wifi_promiscuous_pkt_type_t type) {
 switch (type) {
 case WIFI_PKT_MGMT:
 return "MGMT";
 case WIFI_PKT_DATA:
 return "DATA";
 default:
 case WIFI_PKT_MISC:
 return "MISC";
 }
 }
 */

/**
 * @brief      Permette di convertire il numero del sottotipo in stringa personalizzabile
 *
 * @param[in]  type  Il sottotipo
 *
 * @return     Stringa del sottotipo richiesto
 */
const char *subtype2str(uint8_t type) {
	switch (type) {
	case SUBTYPE_PROBE_REQUEST:
		return "PREQ";
	case SUBTYPE_PROBE_RESPONSE:
		return "PRES";
	case SUBTYPE_BEACON:
		return "BEAC";
	default:
		return "OTHE";
	}
}

/**
 * @brief      Conversione molto grossolana da RSSI a Metri
 *
 * @param[in]  rssi  RSSI
 *
 * @return     float in metri
 */
float calculateDistance(signed rssi) {

	signed txPower = -59; //hard coded power value. Usually ranges between -59 to -65

	if (rssi == 0) {
		return -1.0;
	}

	float ratio = (float) rssi * 1.0 / txPower;
	if (ratio < 1.0) {
		return pow(ratio, 10);
	} else {
		float distance = (0.89976) * pow(ratio, 7.7095) + 0.111;
		return distance;
	}
}

/*
 *@brief  Gestisce l'interrupt generato allo scadere del timer
 *
 *@param para=0 indice timer0
 */
void IRAM_ATTR timer_group0_isr(void *para) { // timer group 0, ISR
	int timer_idx = (int) para;
	uint32_t intr_status = TIMERG0.int_st_timers.val;
	if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
		TIMERG0.hw_timer[timer_idx].update = 1;
		TIMERG0.int_clr_timers.t0 = 1;				//clear the interupt called
		TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;

		//gestione allarme
		mod = 1;
	}
}
/*
 * @brief Init timer usato per periodo "scan paccheti"
 */
static void timer0_init() {
	int timer_group = TIMER_GROUP_0;
	int timer_idx = TIMER_0;
	timer_config_t config;
	config.alarm_en = 1;	//per abilitare l'allarme ogni TIMER_INTERVAL0_SEC
	config.auto_reload = 1;
	config.counter_dir = TIMER_COUNT_UP;	//abilita conteggio da 0 in avanti
	config.divider = TIMER_DIVIDER;
	config.intr_type = TIMER_INTR_SEL;
	config.counter_en = TIMER_PAUSE;
	/*Configure timer*/
	timer_init(timer_group, timer_idx, &config);
	/*Stop timer counter*/
	timer_pause(timer_group, timer_idx);
	/*Load counter value */
	timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
	/*Set alarm value*/
	timer_set_alarm_value(timer_group, timer_idx,
			(TIMER_INTERVAL0_SEC * TIMER_SCALE) - TIMER_FINE_ADJ);
	/*Enable timer interrupt*/
	timer_enable_intr(timer_group, timer_idx);
	/*Set ISR handler*/
	timer_isr_register(timer_group, timer_idx, timer_group0_isr,
			(void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
	/*Start timer counter*/
	timer_start(timer_group, timer_idx);
}

/*
 * @brief Funzione per effettuare la connessione con la rete per raggiungere il server
 */

void wifi_connect() {
	wifi_config_t cfg = { .sta = { .ssid = SSID, .password = PASSPHARSE, }, };
	ESP_ERROR_CHECK(esp_wifi_disconnect());
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg));
	ESP_ERROR_CHECK(esp_wifi_connect());
}

/**
 * @brief      event_handler per init connessione wifi che ritorna semore ESP_OK.
 *
 * @param      ctx    The context
 * @param      event  The event
 *
 * @return     ESP_OK
 */
static esp_err_t event_handler(void *ctx, system_event_t *event) {
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		esp_wifi_connect();
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		break;
	default:
		break;
	}
	return ESP_OK;
}

/**
 * @brief      funzione per connessione tcp con server per scambio pacchetti acquisiti
 *
 * @param      *sbuf cosa inviare al server

 *
 * @return    0=ok, -2=error
 */
int tcpClient() {
	int s;
	int result;

	struct in_addr addr;
	struct sockaddr_in saddr;
	gpio_set_level(BLINK_GPIO, 1);
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true,
	portMAX_DELAY);		//attende la configurazione dal dhcp

	ESP_LOGI(TAG, "tcp_client task started \n");
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == -1) {
		ESP_LOGE(TAG, "Error socket. \n");
		return -2;
	}

	result = inet_aton(TCPServerIP, &addr);
	if (!result) {
		ESP_LOGE(TAG, "Error ip addres.\n");
		return -2;
	}
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(TCPServerPORT));
	saddr.sin_addr = addr;

	ESP_LOGI(TAG, "Connecting with %s:%s ...\n", TCPServerIP, TCPServerPORT);
	result = connect(s, (struct sockaddr*) &saddr, sizeof(saddr));
	if (result == -1) {
		ESP_LOGE(TAG, "Error connect errno=%d \n", errno);
		return -2;
	}
	printf("Connect done.\n");
	result = send_packets(s, head);

	if (result <= 0) {
		ESP_LOGI(TAG, "Error send message\n");
		return -2;
	}

	printf("Message sent.\n");
	reset_packet_list(head);

	close(s);
	gpio_set_level(BLINK_GPIO, 0);
	return 0;
}


#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>

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

static wifi_country_t wifi_country = { .cc = "CN", .schan = 1, .nchan = 13,
		.policy = WIFI_COUNTRY_POLICY_AUTO };

/* Prototipi Funzioni */
static esp_err_t event_handler(void *ctx, system_event_t *event);
static void wifi_sniffer_init(void);
static void wifi_sniffer_set_channel(uint8_t channel);
static const char * type2str(wifi_promiscuous_pkt_type_t type);
const char * subtype2str(uint8_t type);
static void wifi_sniffer_packet_handler(void *buff,
		wifi_promiscuous_pkt_type_t type);
static void getMAC(char *addr, const uint8_t* data, uint16_t offset);
static void printDataSpan(uint16_t start, uint16_t size, const uint8_t* data);

/**
 * @brief      Funzione Main che contiene la chiamata alle configurazioni iniziali e il loop principale
 */
void app_main(void) {

	uint8_t level = 0, channel = 1;

	wifi_sniffer_init();

	gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
	/* loop */
	while (true) {
		gpio_set_level(GPIO_NUM_4, level ^= 1);
		vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
		wifi_sniffer_set_channel(channel);
		channel = (channel % WIFI_CHANNEL_MAX) + 1;
	}
}

/**
 * @brief      Un semplice event_handler che ritorna semore ESP_OK. Non molto utile.
 *
 * @param      ctx    The context
 * @param      event  The event
 *
 * @return     ESP_OK
 */
esp_err_t event_handler(void *ctx, system_event_t *event) {
	return ESP_OK;
}

/**
 * @brief      Wifi sniffer init. Contiene tutte le procedure per configurare correttamente il dispositivo per lo sniffing
 */
void wifi_sniffer_init(void) {
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

	// Conversione del buffer in pacchetto e estrazione di tipo e sottotipo
	const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *) buff;
	uint8_t frameControl = (unsigned int) ppkt->payload[0];
	uint8_t version = (frameControl & 0x03) >> 0;
	uint8_t frameType = (frameControl & 0x0C) >> 2;
	uint8_t frameSubType = (frameControl & 0xF0) >> 4;

	// Filtraggio dei pacchetti non desiderati
	if (frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST)
		return;

	// Stampa dei dati a video
	printf("TYPE= %s", subtype2str(frameSubType));
	printf(" RSSI: %02d", ppkt->rx_ctrl.rssi);
	printf(" Ch: %02d", ppkt->rx_ctrl.channel);

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


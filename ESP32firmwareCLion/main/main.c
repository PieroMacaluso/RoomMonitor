#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "esp32/rom/crc.h"
#include <string.h>
#include <math.h>
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "mdns.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/api.h"
#include "lwip/netdb.h"
#include "driver/gpio.h"
#include "packet.h"
#include "time.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "query_resolver.h"

// set AP CONFIG values
#ifdef CONFIG_AP_HIDE_SSID
#define CONFIG_AP_SSID_HIDDEN 1
#else
#define CONFIG_AP_SSID_HIDDEN 0
#endif
#ifdef CONFIG_WIFI_AUTH_OPEN
#define CONFIG_AP_AUTHMODE WIFI_AUTH_OPEN
#endif
#ifdef CONFIG_WIFI_AUTH_WEP
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WEP
#endif
#ifdef CONFIG_WIFI_AUTH_WPA_PSK
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA_PSK
#endif
#ifdef CONFIG_WIFI_AUTH_WPA2_PSK
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA2_PSK
#endif
#ifdef CONFIG_WIFI_AUTH_WPA_WPA2_PSK
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA_WPA2_PSK
#endif
#ifdef CONFIG_WIFI_AUTH_WPA2_ENTERPRISE
#define CONFIG_AP_AUTHMODE WIFI_AUTH_WPA2_ENTERPRISE
#endif

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
//#define SECOND_SCAN_MODE	20

/**
 *  TIMER
 */

//#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
//#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
//#define TIMER_DIVIDER   80               /*!< Hardware timer clock divider, 80 to get 1MHz clock to timer */
//#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
//#define TIMER_FINE_ADJ   (0*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
//#define TIMER_INTERVAL0_SEC   (SECOND_SCAN_MODE)   /*!< test interval for timer 0 */

/*
 * Parametri connessione server da configurare tramite make menuconfig impostati nel file kconfig.projbuild
 */
#define SSID CONFIG_WIFI_SSID
#define PASSPHARSE CONFIG_WIFI_PASSWORD
#define TCPServerIP CONFIG_IP_SERVER
#define TCPServerPORT CONFIG_PORT_SERVER
#define SCANChannel CONFIG_SCAN_CHANNEL
#define MESSAGE "HelloTCPServer\0"

const int STA_CONNECTED_BIT = BIT0;
#define BLINK_GPIO 2				//pin led

static wifi_country_t wifi_country = { .cc = "CN", .schan = 1, .nchan = 13,
        .policy = WIFI_COUNTRY_POLICY_AUTO };

/* Prototipi Funzioni */
/*static esp_err_t event_handler1(void *ctx, system_event_t *event);*/
void initialize_spiffs(void);
static void wifi_sniffer_init(void);
void wifi_sniffer_update(void);
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
//static void timer0_init();
void wifi_connect();
static esp_err_t event_handler(void *ctx, system_event_t *event);
int tcpClient();
void getMacAddress(char* baseMacChr);
static void http_server(void *pvParameters);
static void http_server_netconn_serve(struct netconn *conn);
void spiffs_serve(char *resource, struct netconn *conn);
int spiffs_save(char *resource, struct netconn *conn);
char *my_nvs_get_str(char * key);

// Static Headers for HTTP response
const static char http_html_hdr[] = "HTTP/1.1 200 OK\n\n";
const static char http_404_hdr[] = "HTTP/1.1 404 NOT FOUND\n\n";
const static char id_str[] = "id=([^&#]+)";
const static char ip_str[] = "ipaddress=([^&#]+)";
const static char posx_str[] = "posx=([^&#]+)";
const static char posy_str[] = "posy=([^&#]+)";

/* Variabili */
node_t head;
static int mod = 0;							//0=> scan 1=>send server
char baseMacChr[18] = {0};

/*Variabili per comunicazione verso server*/
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static const char *TAG = "tcp_client";



/**
 * @brief      Funzione Main che contiene la chiamata alle configurazioni iniziali e il loop principale
 */
void app_main(void) {

    int nallarm=-1;				//nallarm usato per contare quante volte scade il timer da 1 min, ogni 5 allarm bisogna settare l'orario
    /*uint8_t level = 0;*/

    /*if (list_packet_init(size_list_packet) == -1) {
     printf("Error list_packet_init()\n");
     return;
     }*/

    //GetMac_ESP32
    //char baseMacChr[18] = {0};
    getMacAddress(baseMacChr);
    esp_err_t err = nvs_flash_init();
    // TODO: Check if is useful
    if (nvs_flash_init() != ESP_OK){
        printf("Initialization of NVS went wrong. Let's format!\n");
        const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        if(!nvs_partition) printf("FATAL ERROR: No NVS partition found\n");
        err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
        if(err != ESP_OK) printf("FATAL ERROR: Unable to erase the partition\n");
    }
    /*for(i=0;i<18;i++){
            printf("%c",baseMacChr[i]);
        }
        printf("\n");*/
    if ((head = init_packet_list(baseMacChr)) == NULL) {
        printf("Error init_packet_list()\n");
        return;
    }
    wifi_event_group = xEventGroupCreate();
    initialize_spiffs();
    wifi_sniffer_init();									//init modalit� scan
    vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
    wifi_sniffer_set_channel(atoi(SCANChannel));

    initialize_sntp();
    checkTime(&nallarm);
    //vTaskDelay(20 / portTICK_PERIOD_MS);

    timer0_init();											//init timer gestione modalità scan e comunicazione server

    gpio_set_direction(BLINK_GPIO,GPIO_MODE_OUTPUT);

    /* loop */
    while (true) {

        mod = 0;

        printf("Inizio raccolta dati...\n");
        timer_start(TIMER_GROUP_0, TIMER_0);
        while (mod == 0) {
            vTaskDelay(20 / portTICK_PERIOD_MS);				//attesa alarm
        }

        esp_wifi_set_promiscuous(false);
        printf("Fine periodo cattura.\n");
        timer_pause(TIMER_GROUP_0, TIMER_0);

        vTaskDelay(200 / portTICK_PERIOD_MS);//sostituire con l'invio del buffer

        tcpClient();

        checkTime(&nallarm);

        esp_wifi_set_promiscuous(true);
    }
    free_packet_list(head);
}


void getMacAddress(char *baseMacChr) {
    uint8_t baseMac[6];
    // Get MAC address for WiFi station
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
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
    tcpip_adapter_init();
    printf("- TCP adapter initialized\n");

    // stop DHCP server
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
    printf("- DHCP server stopped\n");

    // assign a static IP to the network interface
    tcpip_adapter_ip_info_t info;
    memset(&info, 0, sizeof(info));
    IP4_ADDR(&info.ip, 192, 168, 1, 1);
    IP4_ADDR(&info.gw, 192, 168, 1, 1);
    IP4_ADDR(&info.netmask, 255, 255, 255, 0);
    ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &info));
    printf("- TCP adapter configured with IP 192.168.1.1/24\n");

    // start the DHCP server
    ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));
    printf("- DHCP server started\n");

    // initialize the wifi event handler
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    printf("- Event loop initialized\n");

    //ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    ;
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    wifi_config_t ap_config = {
            .ap = {
                    .ssid = CONFIG_AP_SSID,
                    .password = CONFIG_AP_PASSWORD,
                    .ssid_len = 0,
                    .channel = CONFIG_AP_CHANNEL,
                    .authmode = CONFIG_AP_AUTHMODE,
                    .ssid_hidden = CONFIG_AP_SSID_HIDDEN,
                    .max_connection = CONFIG_AP_MAX_CONNECTIONS,
            },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
}

char *my_nvs_get_str(char * key) {
    esp_err_t err;
    char* value = NULL;
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    size_t required_size;
    err = nvs_get_str(my_handle, key, NULL, &required_size);
    if (err == ESP_OK) {
        value = malloc(required_size);
        nvs_get_str(my_handle, key, value, &required_size);
        printf("- NVS: %s, %s\n", key, value);
    }
    nvs_close(my_handle);
    return value;
}


void initialize_spiffs(void) {
    // initialize SPIFFS
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 10,
            .format_if_mount_failed = false};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    printf("- SPIFFS VFS module registered\n");
}

/**
 * @brief      Wifi sniffer update. Contiene tutte le procedure per ripristinare correttamente il dispositivo per lo sniffing
 *
 * EDIT non necessaria, basta risettare true la promiscuous. Fatto nel ciclo while del main
 */
/*void wifi_sniffer_update(void){
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);
    vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);
    wifi_sniffer_set_channel(atoi(SCANChannel));
}*/

/**
 * @brief      Restituisce il MAC address.
 *
 * @param      addr    Puntatore all'array in cui sar� scritto il MAC in formato leggibile
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
    time_t now;
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
    uint32_t plen = 0;
    plen = crc32_le(0, ppkt->payload, ppkt->rx_ctrl.sig_len);
    printf("%2d %2d %2d ", get_id(), get_posx(), get_posy());
    printf("%u\t", ppkt->rx_ctrl.sig_len);
//	for(i=0; i<4; i++) {
    printf("%08x\t", plen);
//	}
    printf(" TYPE= %s", subtype2str(frameSubType));
    printf(" RSSI: %02d", ppkt->rx_ctrl.rssi);
    printf(" Distance: %3.2fm\t", calculateDistance(ppkt->rx_ctrl.rssi));
    printf(" T: %ld", (long) time(&now));

    char addr[] = "00:00:00:00:00:00";
    getMAC(addr, ppkt->payload, 10);
    printf(" Peer MAC: %s", addr);
    printf(" Board Mac: %s", baseMacChr);
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
/*static void timer0_init() {
	int timer_group = TIMER_GROUP_0;
	int timer_idx = TIMER_0;
	timer_config_t config;
	config.alarm_en = 1;	//per abilitare l'allarme ogni TIMER_INTERVAL0_SEC
	config.auto_reload = 1;
	config.counter_dir = TIMER_COUNT_UP;	//abilita conteggio da 0 in avanti
	config.divider = TIMER_DIVIDER;
	config.intr_type = TIMER_INTR_SEL;
	config.counter_en = TIMER_PAUSE;
	//Configure timer
	timer_init(timer_group, timer_idx, &config);
	//Stop timer counter
	timer_pause(timer_group, timer_idx);
	//Load counter value
	timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
	//Set alarm value
	timer_set_alarm_value(timer_group, timer_idx,
			(TIMER_INTERVAL0_SEC * TIMER_SCALE) - TIMER_FINE_ADJ);
	//Enable timer interrupt
	timer_enable_intr(timer_group, timer_idx);
	//Set ISR handler
	timer_isr_register(timer_group, timer_idx, timer_group0_isr,
			(void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
	//Start timer counter
	timer_start(timer_group, timer_idx);
}*/

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
        case SYSTEM_EVENT_AP_START:

            printf("- Wifi adapter started\n\n");

            // create and configure the mDNS service
            ESP_ERROR_CHECK(mdns_init());
            ESP_ERROR_CHECK(mdns_hostname_set("esp32web"));
            ESP_ERROR_CHECK(mdns_instance_name_set("ESP32 webserver"));
            printf("- mDNS service started\n");

            // start the HTTP server task
            xTaskCreate(&http_server, "http_server", 20000, NULL, 5, NULL);
            printf("- HTTP server started\n");

            break;

        case SYSTEM_EVENT_AP_STACONNECTED:

            xEventGroupSetBits(wifi_event_group, STA_CONNECTED_BIT);
            break;
        default:
            break;
    }
    return ESP_OK;
}

// HTTP server task
static void http_server(void *pvParameters)
{

    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);
    printf("* HTTP Server listening\n");
    do
    {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK)
        {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
        vTaskDelay(10);
    } while (err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
}

static void http_server_netconn_serve(struct netconn *conn)
{

    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK)
    {

        // get the request and terminate the string
        netbuf_data(inbuf, (void **)&buf, &buflen);
        buf[buflen] = '\0';

        // get the request body and the first line
        char *body = strstr(buf, "\r\n\r\n");
        char *request_line = strtok(buf, "\n");

        if (request_line)
        {

            // static content, get it from SPIFFS
            if (strstr(request_line, "GET /save.html"))
            {
                char *method = strtok(request_line, " ");
                char *resource = strtok(NULL, " ");
                //printf("SAVE START: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                spiffs_save(resource, conn);
                //printf("SAVE END: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                spiffs_serve("/index.html", conn);
            }
            // default page -> redirect to index.html
            if (strstr(request_line, "GET / "))
            {
                //printf("SERVE START: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                spiffs_serve("/index.html", conn);
                //printf("SERVE END: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
            }
            else
            {
                char *method = strtok(request_line, " ");
                char *resource = strtok(NULL, " ");
                //printf("SERVE START: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                spiffs_serve(resource, conn);
                //printf("SERVE END: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
            }
        }
    }

    netbuf_free(inbuf);
}

// serve static content from SPIFFS
void spiffs_serve(char *resource, struct netconn *conn)
{
    if (resource == NULL)
    {
        return;
    }
    // check if it exists on SPIFFS
    char full_path[100];
    int rv;
    char buff[100];
    int len = 100;

    sprintf(full_path, "/spiffs%s", resource);
    printf("+ Serving static resource: %s\n", full_path);
    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_COPY);

        // int filedesc = open(full_path, O_RDWR);
        // if (filedesc == -1)
        // {
        // 	// print which type of error have in a code
        // 	printf("Error Number % d\n", errno);

        // 	// print program detail "Success or failure"
        // 	perror("Program");
        // 	return;
        // }

        // open the file for reading
        FILE *f = fopen(full_path, "r");
        //FILE *f = fdopen(filedesc, "r");

        if (f == NULL)
        {
            printf("Unable to open the file %s\n", full_path);
            return;
        }

        // send the file content to the client
        char buffer[500];
        len = 500;
        int size = 0;
        size_t char_read = 0;
        ; /* there was data to read */
        // char_read = read(filedesc, buffer, len); /* there was data to read */
        while ((char_read = fread(buffer, sizeof(char), len, f)) != 0)
        {
            size += char_read / sizeof(char);
            netconn_write(conn, buffer, char_read, NETCONN_COPY);
        }
        fclose(f);
        // close(filedesc);
        fflush(stdout);
        printf("+ served %d bytes\n", size);
    }
    else
    {
        printf("Error 404\n");
        netconn_write(conn, http_404_hdr, sizeof(http_404_hdr) - 1, NETCONN_COPY);
    }
}

// serve static content from SPIFFS
int spiffs_save(char *resource, struct netconn *conn)
{

    int N_PAR = 4;
    printf("%s\n", resource);
    int i, err;
    struct query_t *url;

    if ((url = query_init(N_PAR)) == NULL)
    {
        //TODO: Change debug print
        fprintf(stderr, "Could not parse url!\n");
        return 1;
    }
    if (-1 == query_parse(url, resource))
    {
        fprintf(stderr, "Could not parse url!\n");
        return 1;
    }
    printf("Query string parameters:\n");
    for (i = 0; i < N_PAR; i++)
    {
        printf("\t%s: %s\n", url->params[i].key, url->params[i].val);
    }
    FILE *f = fopen("/spiffs/data.json", "w");
    fprintf(f, "{ \"id\": \"%s\", \"ipaddr\": \"%s\", \"posx\": \"%s\", \"posy\":\"%s\"}", url->params[0].val, url->params[1].val, url->params[2].val, url->params[3].val);
    fclose(f);
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        for (i = 0; i < N_PAR; i++)
        {
            err = nvs_set_str(my_handle, url->params[i].key, url->params[i].val);
            switch (err)
            {
                case ESP_OK:
                    printf("Saved\n");
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    printf("The value is not initialized yet!\n");
                    break;
                default:
                    printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
        }
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    }
    query_free(url);
    printf("ESP32 Restarting...\n");
    esp_restart();
    return EXIT_SUCCESS;
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


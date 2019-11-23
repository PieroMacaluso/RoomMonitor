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
#include <cJSON.h>
#include "soc/timer_group_struct.h"
#include "driver/timer.h"
#include "mdns.h"
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/api.h"
#include "packet.h"
#include "time.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "validation.h"

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
#define SUBTYPE_PROBE_REQUEST     0b0100
#define SUBTYPE_PROBE_RESPONSE    0b0101
#define SUBTYPE_BEACON            0b1000

#define    WIFI_CHANNEL_MAX        (13)
#define    WIFI_CHANNEL_SWITCH_INTERVAL    (500)


/**
 * Parametri connessione server da configurare tramite make menuconfig impostati nel file kconfig.projbuild
 */
#define SSID CONFIG_WIFI_SSID
#define PASSPHARSE CONFIG_WIFI_PASSWORD
#define TCPServerIP CONFIG_IP_SERVER
#define TCPServerPORT CONFIG_PORT_SERVER
#define SCANChannel CONFIG_SCAN_CHANNEL
const int STA_CONNECTED_BIT = BIT0;
// Pin LED
#define BLINK_GPIO 2

static wifi_country_t wifi_country = {.cc = "CN", .schan = 1, .nchan = 13,
        .policy = WIFI_COUNTRY_POLICY_AUTO};

/** Prototipi Funzioni */
void initialize_spiffs(void);

void wifi_init(void);

static void wifi_sniffer_init(void);

static void wifi_sniffer_set_channel(uint8_t channel);

const char *subtype2str(uint8_t type);

static void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type);

static void getMAC(char *addr, const uint8_t *data, uint16_t offset);

static void printDataSpan(uint16_t start, uint16_t size, const uint8_t *data);

float calculateDistance(signed rssi);

void wifi_connect();

static esp_err_t event_handler(void *ctx, system_event_t *event);

int tcpClient();

void getMacAddress(char *macChr);

static void http_server(void *pvParameters);

static void http_server_netconn_serve(struct netconn *conn);

void spiffs_serve(char *resource, struct netconn *conn);

int spiffs_save(char *resource, struct netconn *conn);

char *my_nvs_get_str(char *key);

/** Static Headers for HTTP response */
const static char http_html_hdr[] = "HTTP/1.1 200 OK\n\n";
const static char http_404_hdr[] = "HTTP/1.1 404 NOT FOUND\n\n";

/** Variabili */
node_t head;
static int mod = 0;                            //0=> scan 1=>send server
char baseMacChr[18] = {0};

/** Variabili per comunicazione verso server */
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;
static const char *TAG = "tcp_client";

/** STRUCT FOR CRC **/
typedef struct {
    unsigned frame_ctrl : 16;
    unsigned duration_id : 16;
    uint8_t addr1[6]; /* receiver address */
    uint8_t addr2[6]; /* sender address */
    uint8_t addr3[6]; /* filtering address */
    unsigned sequence_ctrl : 16;
    uint8_t addr4[6]; /* optional */
} wifi_ieee80211_mac_hdr_t;

typedef struct {
    uint8_t point[0];
    uint32_t crc;
} crc_t;

typedef struct {
    wifi_ieee80211_mac_hdr_t hdr;
    crc_t payload; /* network data ended with 4 bytes csum (CRC32) */
} wifi_ieee80211_packet_t;

/**
 * Funzione Main che contiene la chiamata alle configurazioni iniziali e il loop principale
 */
void app_main(void) {

    // Conteggio quante volte scade il timer da 1 min, ogni 5 bisogna settare l'orario
    int nallarm = 0;
    //bool orario = false;

    //GetMac_ESP32
    getMacAddress(baseMacChr);
    esp_err_t err = nvs_flash_init();
    // TODO: Check if is useful
    if (err != ESP_OK) {
        printf("Initialization of NVS went wrong. Let's format!\n");
        const esp_partition_t *nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA,
                                                                        ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
        if (!nvs_partition) printf("FATAL ERROR: No NVS partition found\n");
        err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
        if (err != ESP_OK) printf("FATAL ERROR: Unable to erase the partition\n");
    }

    if ((head = init_packet_list(baseMacChr)) == NULL) {
        printf("Error init_packet_list()\n");
        return;
    }

    wifi_event_group = xEventGroupCreate();

    initialize_spiffs();
    wifi_init();

    //Evita che la schedina tenti di partire fin se prima non viene inizializzata tramite captive portal
    while (my_nvs_get_str("saved") == NULL) {
        sleep(30);
    }

    if (!initialize_sntp()) {
        printf("ESP32 Restarting...\n");
        esp_restart();
    }

    wifi_sniffer_init();

    vTaskDelay(WIFI_CHANNEL_SWITCH_INTERVAL / portTICK_PERIOD_MS);

    // INIT timer gestione modalità scan e comunicazione server
    timer0_init();

    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    /** Main Loop */
    while (true) {

        mod = 0;
        printf("Inizio raccolta dati...\n");
        ESP_ERROR_CHECK(timer_start(TIMER_GROUP_0, TIMER_0));
        while (mod == 0) {
            // Attesa alarm
            vTaskDelay(60 / portTICK_PERIOD_MS);
        }

        esp_wifi_set_promiscuous(false);
        printf("Fine periodo cattura.\n");
        ESP_ERROR_CHECK(timer_start(TIMER_GROUP_0, TIMER_0));

        // Sostituire con l'invio del buffer
        vTaskDelay(200 / portTICK_PERIOD_MS);

        if (tcpClient() != 0) {
            printf("TCP Client Error\n");
            break;
        }

        checkTime(&nallarm);

        ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
    }
    free_packet_list(head);
    printf("ESP32 Restarting...\n");
    esp_restart();
}

/**
 * Ottiene il MAC della board e lo restituisce tramite il parametro passato per riferimento
 * @param macChr puntatore alla memoria allocata che conterà il MAC della schedina
 */
void getMacAddress(char *macChr) {
    uint8_t baseMac[6];
    esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
    sprintf(macChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4],
            baseMac[5]);
}


/**
 * Impostazione dei parametri di rete sia per il captive portal sia per la comunicazione in rete
 */
void wifi_init() {
    esp_log_level_set("wifi", ESP_LOG_NONE);    // disable wifi driver logging
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
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
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
    // Settaggio dati da NVS se presenti
    char *ssid = my_nvs_get_str("ssid_ap");
    char *pass = my_nvs_get_str("password_ap");
    if (ssid != NULL) {
        memcpy(ap_config.ap.ssid, ssid, strlen(ssid));
        strcpy((char *) ap_config.ap.ssid, ssid);
        free(ssid);
    }
    if (pass != NULL) {
        strcpy((char *) ap_config.ap.password, pass);
        free(pass);
    }
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

 /**
  * Wifi sniffer init. Contiene tutte le procedure per configurare correttamente il dispositivo per lo sniffing
  */
void wifi_sniffer_init(void) {
    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&wifi_sniffer_packet_handler);

    char *channel = my_nvs_get_str("channel");
    if (channel != NULL) {
        wifi_sniffer_set_channel(atoi(channel));
        free(channel);
    } else {
        wifi_sniffer_set_channel(atoi(SCANChannel));
    }
}

 /**
  * Funzione che permette di ottenere il valore salvato nella memoria NVS come vettore di char
  * @param key: chiave del valore salvato nella memoria NVS
  * @return valore contenuto nella memoria NVS se questo è presente, altrimenti viene ritornato NULL
  */
char *my_nvs_get_str(char *key) {
    // TODO: doc
    esp_err_t err;
    char *value = NULL;
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

/**
 * Inizializzazione partizione SPIFFS che viene utilizzata per il CAPTIVE PORTAL
 */
void initialize_spiffs(void) {
    // initialize SPIFFS
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 100,
            .format_if_mount_failed = false};

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    printf("- SPIFFS VFS module registered\n");
}

/**
 * Restituisce il MAC address.
 *
 * @param      addr    Puntatore all'array in cui sarà scritto il MAC in formato leggibile
 * @param  data    Puntatore alla porzione payload del pacchetto
 * @param  offset  Punto del payload in cui si trova l'indirizzo MAC richiesto
 */

static void getMAC(char *addr, const uint8_t *data, uint16_t offset) {
    sprintf(addr, "%02x:%02x:%02x:%02x:%02x:%02x", data[offset + 0],
            data[offset + 1], data[offset + 2], data[offset + 3],
            data[offset + 4], data[offset + 5]);
}

 /**
  * Stampa i dati contenuti nel pacchetto partendo da start, fino a start+size
  *
  * @param start    posizione iniziale
  * @param size     la dimensione
  * @param data     puntatore alla porzione payload del pacchetto
  */
static void printDataSpan(uint16_t start, uint16_t size, const uint8_t *data) {
    for (uint16_t i = start; i < start + size; i++) {
        printf("%c", data[i]);
    }
}

/**
 * Funzione Handler invocata quando viene ricevuto un pacchetto.
 *
 * @param buff  Il buffer, o meglio il pacchetto
 * @param type  Il tipo di pacchetto
 */
void wifi_sniffer_packet_handler(void *buff, wifi_promiscuous_pkt_type_t type) {
    // Conversione del buffer in pacchetto e estrazione di tipo e sottotipo
    const wifi_promiscuous_pkt_t *ppkt = (wifi_promiscuous_pkt_t *) buff;
    time_t now;
    uint8_t frameControl = (unsigned int) ppkt->payload[0];
    // uint8_t version = (frameControl & 0x03) >> 0;
    uint8_t frameType = (frameControl & 0x0C) >> 2;
    uint8_t frameSubType = (frameControl & 0xF0) >> 4;

    // Filtraggio dei pacchetti non desiderati
    if (frameType != TYPE_MANAGEMENT || frameSubType != SUBTYPE_PROBE_REQUEST) {
        return;
    }

    // Set Packet

    // Test aumento pacchetti
    //for(int i=0;i<100;i++)
    addto_packet_list(ppkt, head);

    // Stampa dei dati a video
    uint32_t plen = 0;
    uint32_t size = ppkt->rx_ctrl.sig_len;
    plen = crc32_le(0, ppkt->payload, size);
    //printf("%2d %2d %2d ", get_id(), get_posx(), get_posy());
    printf("%2d ", get_id());
    printf("%u\t", ppkt->rx_ctrl.sig_len);
    printf("%08x\t", plen);
    printf(" TYPE= %s", subtype2str(frameSubType));
    printf(" RSSI: %02d", ppkt->rx_ctrl.rssi);
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
 * Imposta il canale WiFi desiderato nel dispositivo
 *
 * @param  channel  The channel
 */
void wifi_sniffer_set_channel(uint8_t channel) {
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

/**
 * Permette di convertire il numero del sottotipo in stringa personalizzabile
 *
 * @param type  Il sottotipo
 *
 * @return      Stringa del sottotipo richiesto
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
 * Gestisce l'interrupt generato allo scadere del timer
 * @param para = 0 indice timer0
 */
void IRAM_ATTR timer_group0_isr(void *para) {
    // Timer group 0, ISR
    int timer_idx = (int) para;
    uint32_t intr_status = TIMERG0.int_st_timers.val;
    if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
        TIMERG0.hw_timer[timer_idx].update = 1;
        // Clear the interrupt called
        TIMERG0.int_clr_timers.t0 = 1;
        TIMERG0.hw_timer[timer_idx].config.alarm_en = 1;
        // Gestione allarme
        mod = 1;
    }
}

/**
 * Funzione per effettuare la connessione con la rete per raggiungere il server
 */
void wifi_connect() {
    wifi_config_t cfg = {.sta = {.ssid = SSID, .password = PASSPHARSE,},};
    char *ssid = my_nvs_get_str("ssid_server");
    char *pass = my_nvs_get_str("password_server");
    if (ssid != NULL) {
        memcpy(cfg.sta.ssid, ssid, strlen(ssid));
        cfg.sta.ssid[strlen(ssid)] = '\0';
        free(ssid);
    }
    if (pass != NULL) {
        memcpy(cfg.sta.password, pass, strlen(pass));
        cfg.sta.password[strlen(pass)] = '\0';
        free(pass);
    }
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
            xTaskCreate(&http_server, "http_server", 20480, NULL, 5, NULL);
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

/**
 * HTTP server task
 * @param pvParameters  Parametri
 */
static void http_server(void *pvParameters) {

    struct netconn *conn, *newconn;
    err_t err;
    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);
    printf("* HTTP Server listening\n");
    do {
        err = netconn_accept(conn, &newconn);
        printf("New client connected\n");
        if (err == ERR_OK) {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
        vTaskDelay(10);
    } while (err == ERR_OK);
    netconn_close(conn);
    netconn_delete(conn);
    printf("\n");
}

/**
 * Funzione che permette di gestire le richieste fatte all'HTTP Server
 * @param conn  Connessione
 */
static void http_server_netconn_serve(struct netconn *conn) {

    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK) {

        // get the request and terminate the string
        netbuf_data(inbuf, (void **) &buf, &buflen);
        buf[buflen] = '\0';

        // get the request body and the first line
        char *body = strstr(buf, "\r\n\r\n");
        char *first_line = strtok(buf, "\n");


        if (first_line) {
            // static content, get it from SPIFFS
            if (strstr(first_line, "GET / ")) {
                printf("B\n");
                //printf("SERVE START: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                spiffs_serve("/index.html", conn);
                //printf("SERVE END: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
            } else if (strstr(first_line, "POST /save.html")) {
                char *data = strstr(body, "{");
                printf("A\n");
                //printf("SAVE START: %d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
                err = spiffs_save(data, conn);
                if (err != 0) {
                    printf("Errore salvataggio %d\n", err);
                    spiffs_serve("/error.html", conn);
                } else {
                    spiffs_serve("/success.html", conn);
                    netbuf_delete(inbuf);
                    netconn_close(conn);
                    printf("ESP32 Restarting...\n");
                    esp_restart();
                }
            } else {
                char *method = strtok(first_line, " ");
                char *resource = strtok(NULL, " ");
                printf("%s\n", resource);
                spiffs_serve(resource, conn);
            }
        } else printf("Unknown request\n");
    }
    netbuf_delete(inbuf);
    netconn_close(conn);
    printf("CA\n");
}

/**
 * Serve contenuto statico da partizione SPIFFS
 * @param resource  Path della risorsa richiesta
 * @param conn      Struttura connessione
 */
void spiffs_serve(char *resource, struct netconn *conn) {
    if (resource == NULL) {
        return;
    }
    // check if it exists on SPIFFS
    char full_path[100];
    int len = 100;

    sprintf(full_path, "/spiffs%s", resource);
    printf("+ Serving static resource: %s\n", full_path);
    struct stat st;
    if (stat(full_path, &st) == 0) {
        netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_COPY);

        // open the file for reading
        FILE *f = fopen(full_path, "r");
        if (f == NULL) {
            printf("Unable to open the file %s\n", full_path);
            return;
        }
        // send the file content to the client
        char buffer[500];
        len = 500;
        int size = 0;
        size_t char_read = 0;; /* there was data to read */
        while ((char_read = fread(buffer, sizeof(char), len, f)) != 0) {
            size += char_read / sizeof(char);
            netconn_write(conn, buffer, char_read, NETCONN_COPY);
        }
        fclose(f);
        fflush(stdout);
        printf("+ served %d bytes\n", size);
    } else {
        printf("Error 404\n");
        netconn_write(conn, http_404_hdr, sizeof(http_404_hdr) - 1, NETCONN_COPY);
    }
}

/**
 * Richiesta per salvataggio dei dati all'interno della schedina tramite richiesta HTTP
 * @param resource      Risorsa Da Salvare
 * @param conn          Connessione
 * @return              Esito positivo ritorno 0, esito negativo diverso da 0
 */
int spiffs_save(char *resource, struct netconn *conn) {
    cJSON *json = cJSON_Parse(resource);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return 1;
    }

    // Validazione input
    int res;

    char *id = cJSON_GetObjectItem(json, "id")->valuestring;
    res = idValidation(id);
    if (res != 0)
        return res;

    char *ssid_ap = cJSON_GetObjectItem(json, "ssid_ap")->valuestring;
    res = ssidApValidation(ssid_ap);
    if (res != 0)
        return res;

    char *password_ap = cJSON_GetObjectItem(json, "password_ap")->valuestring;
    res = passwordApValidation(password_ap);
    if (res != 0)
        return res;

    char *channel = cJSON_GetObjectItem(json, "channel")->valuestring;
    res = channelValidation(channel);
    if (res != 0)
        return res;

    char *ssid_server = cJSON_GetObjectItem(json, "ssid_server")->valuestring;
    res = ssidServerValidation(ssid_server);
    if (res != 0)
        return res;

    char *password_server = cJSON_GetObjectItem(json, "password_server")->valuestring;
    res = passServerValidation(password_server);
    if (res != 0)
        return res;
    char *ip_server = cJSON_GetObjectItem(json, "ip_server")->valuestring;
    res = ipValidation(ip_server);
    if (res != 0)
        return res;

    printf("Validazione input eseguita.");
    int err;

    FILE *f = fopen("/spiffs/data.json", "w");
    fprintf(f, "%s", cJSON_Print(json));
    fclose(f);
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        cJSON *current_element = NULL;
        char *current_key = NULL;
        cJSON_ArrayForEach(current_element, json) {
            current_key = current_element->string;
            if (current_key != NULL) {
                printf("%s %s\n", current_key, current_element->valuestring);
                err = nvs_set_str(my_handle, current_key, current_element->valuestring);
            }
        }
        printf("Committing updates in NVS ... ");
        switch (err) {
            case ESP_OK:
                printf("Saved\n");
                // indica che i dati ricevuti sono stati salvati, solo in tale situazione il funzionamento della schedina viene sbloccato
                err = nvs_set_str(my_handle, "saved", "saved");
                switch (err) {
                    case ESP_OK:
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default:
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default:
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    }
    return 0;
}

 /**
  * Funzione per connessione tcp con server per scambio pacchetti acquisiti
  * @return  0=ok, -2=error
  */
int tcpClient() {
    int s;
    int result;

    struct in_addr addr;
    struct sockaddr_in saddr;
    gpio_set_level(BLINK_GPIO, 1);
    // Attende la configurazione dal dhcp
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);

    ESP_LOGI(TAG, "tcp_client task started \n");
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1) {
        ESP_LOGE(TAG, "Error socket. \n");
        return -2;
    }
    // TODO: Porta da NVS
    char *server = my_nvs_get_str("ip_server");
    if (server != NULL) {
        result = inet_aton(server, &addr);
        if (!result) {
            ESP_LOGE(TAG, "Error ip address.\n");
            close(s);
            free(server);
            return -2;
        }
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(atoi(TCPServerPORT));
        saddr.sin_addr = addr;

        ESP_LOGI(TAG, "Connecting with %s:%s ...\n", server, TCPServerPORT);
        free(server);
    } else {
        result = inet_aton(TCPServerIP, &addr);
        if (!result) {
            ESP_LOGE(TAG, "Error ip addres.\n");
            close(s);
            return -2;
        }
        saddr.sin_family = AF_INET;
        saddr.sin_port = htons(atoi(TCPServerPORT));
        saddr.sin_addr = addr;

        ESP_LOGI(TAG, "Connecting with %s:%s ...\n", TCPServerIP, TCPServerPORT);
    }
    result = connect(s, (struct sockaddr *) &saddr, sizeof(saddr));
    if (result == -1) {
        ESP_LOGE(TAG, "Error connect errno=%d \n", errno);
        close(s);
        return -2;
    }
    printf("Connect done.\n");
    // Invio dei pacchetti
    result = send_packets(s, head);

    int retValue = 0;
    if (result <= 0) {
        // In caso di errore svuoto la lista, i pacchetti vengono persi
        ESP_LOGI(TAG, "Error send message\n");
        retValue = -2;
    } else {
        ESP_LOGI(TAG, "Message sent.\n");
    }
    // Messaggi inviati, svuoto comunque la lista
    reset_packet_list(head);
    close(s);
    gpio_set_level(BLINK_GPIO, 0);
    return retValue;
}


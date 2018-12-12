#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mdns.h"
#include "lwip/err.h"
#include "lwip/api.h"
#include "lwip/netdb.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include <sys/unistd.h>
#include "query_resolver.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "json/frozen.h"

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

// static headers for HTTP responses
const static char http_html_hdr[] = "HTTP/1.1 200 OK\n\n";
const static char http_404_hdr[] = "HTTP/1.1 404 NOT FOUND\n\n";
const static char id_str[] = "id=([^&#]+)";
const static char ip_str[] = "ipaddress=([^&#]+)";
const static char posx_str[] = "posx=([^&#]+)";
const static char posy_str[] = "posy=([^&#]+)";

// Event group
static EventGroupHandle_t event_group;
const int STA_CONNECTED_BIT = BIT0;

// prototypes
static esp_err_t event_handler(void *ctx, system_event_t *event);
void ap_monitor_task(void *pvParameter);
static void http_server(void *pvParameters);
static void http_server_netconn_serve(struct netconn *conn);
void spiffs_serve(char *resource, struct netconn *conn);
int spiffs_save(char *resource, struct netconn *conn);

static const char *TAG = "example";

// AP event handler
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	switch (event->event_id)
	{

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

		xEventGroupSetBits(event_group, STA_CONNECTED_BIT);
		break;

	default:
		break;
	}

	return ESP_OK;
}

// AP monitor task, receive Wifi AP events
void ap_monitor_task(void *pvParameter)
{

	while (1)
	{

		xEventGroupWaitBits(event_group, STA_CONNECTED_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
		printf("New station connected\n");
	}
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

	return EXIT_SUCCESS;
}

// Main application
void app_main()
{
	// disable the default wifi logging
	esp_log_level_set("wifi", ESP_LOG_NONE);

	printf("ESP32 SoftAP HTTP Server\n\n");

	// create the event group to handle wifi events
	event_group = xEventGroupCreate();

	// initialize NVS
	ESP_ERROR_CHECK(nvs_flash_init());
	printf("- NVS initialized\n");

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

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_LOGE(TAG, "Erasing NVS (%s)", esp_err_to_name(err));
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	ESP_ERROR_CHECK(err);
	printf("- NVS module registered\n");
	nvs_handle my_handle;
	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	size_t required_size;
	err = nvs_get_str(my_handle, "id", NULL, &required_size);
	if (err == ESP_OK) {
		char* id = malloc(required_size);
		nvs_get_str(my_handle, "id", id, &required_size);
		printf("- NVS: %s, %s\n", "id", id);
	}
	err = nvs_get_str(my_handle, "ipaddress", NULL, &required_size);
	if (err == ESP_OK) {
		char* ipaddress = malloc(required_size);
		nvs_get_str(my_handle, "ipaddress", ipaddress, &required_size);
		printf("- NVS: %s, %s\n", "ipaddress", ipaddress);
	}
	err = nvs_get_str(my_handle, "posx", NULL, &required_size);
	if (err == ESP_OK) {
		char* posx = malloc(required_size);
		nvs_get_str(my_handle, "posx", posx, &required_size);
		printf("- NVS: %s, %s\n", "posx", posx);

	}
	err = nvs_get_str(my_handle, "posy", NULL, &required_size);
	if (err == ESP_OK) {
		char* posy = malloc(required_size);
		nvs_get_str(my_handle, "posy", posy, &required_size);
		printf("- NVS: %s, %s\n", "posy", posy);

	}
	nvs_close(my_handle);

	
	

	// initialize the tcp stack
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

	// initialize the wifi stack in AccessPoint mode with config in RAM
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	printf("- Wifi adapter configured in SoftAP mode\n");

	// configure the wifi connection and start the interface
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
	printf("- Wifi network settings applied\n");

	// start the wifi interface
	ESP_ERROR_CHECK(esp_wifi_start());
	printf("- Wifi adapter starting...\n");

	// start the tasks
	xTaskCreate(&ap_monitor_task, "ap_monitor_task", 2048, NULL, 5, NULL);
}

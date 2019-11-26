#ifndef APP_TEMPLATE_PACKET_H
#define APP_TEMPLATE_PACKET_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp32/rom/crc.h"
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
#include "esp_log.h"
#include "nvs_getter.h"

#define STARTSIZE 	20
#define LENPACKET 	11
#define OFFMAC 		10

typedef struct node *node_t;

int get_id();
node_t init_packet_list(char baseMacChr[18]);
node_t addto_packet_list(const wifi_promiscuous_pkt_t *ppkt, node_t h);
int send_packets(int s, node_t h);
void reset_packet_list(node_t h);
void print_all_list(node_t h);


#endif //APP_TEMPLATE_PACKET_H

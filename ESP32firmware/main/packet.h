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
#include "esp_log.h"

static const char *TAGP = "tcp_client";


#define STARTSIZE 	20
#define LENPACKET 	11
#define OFFMAC 		10


typedef struct packetlist *PacketList;

typedef struct packet *Packet;

PacketList init_packet_list();
Packet init_packet();
int setPacket(Packet p, const wifi_promiscuous_pkt_t *ppkt);
PacketList addto_packet_list(Packet p, PacketList list);
int getn_packet_list(PacketList list);
int send_packets(int s, PacketList list);
/*unsigned char * string_packet(PacketList list, int n);*/
PacketList reset_packet_list(PacketList list);
void free_packet_list(PacketList list);



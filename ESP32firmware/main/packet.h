#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#define STARTSIZE 	20
#define LENPACKET 	11
#define OFFMAC 		10


typedef struct packetlist *PacketList;

typedef struct packet *Packet;

PacketList init_packet_list();
Packet init_packet();
void setPacket(Packet p, signed rssi, uint32_t time, const uint8_t *data);
PacketList addto_packet_list(Packet p, PacketList list);
int getn_packet_list(PacketList list);
unsigned char * string_packet(PacketList list, int n);
PacketList reset_packet_list(PacketList list);
void free_packet_list(PacketList list);



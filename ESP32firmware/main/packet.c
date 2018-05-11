#include "packet.h"

struct packet {
	signed rssi;
	unsigned mac[6];
	unsigned timestamp[4];
};

struct packetlist {
	Packet * list;
	int max;
	int n;
};

PacketList init_packet_list() {
	PacketList list;
	list = malloc(sizeof(PacketList));
	list->list = malloc(STARTSIZE * sizeof(Packet));
	if (list->list == NULL)
		return NULL;
	list->max = STARTSIZE;
	list->n = 0;
	return list;
}

Packet init_packet() {
	Packet p = malloc(sizeof(Packet));
	return p;
}

void setPacket(Packet p, signed rssi, uint32_t time, const uint8_t *data) {
	int i;
	p->rssi = rssi;
	for (i = 0; i < 6; i++) {
		p->mac[i] = data[OFFMAC + i];
	}

	p->timestamp[0] = time;
}

PacketList realloc_packet_list(PacketList list, int dim) {
	list->list = realloc(list->list, dim * sizeof(Packet));
	if (list->list == NULL)
		return NULL;
	list->max = dim;
	return list;
}

PacketList addto_packet_list(Packet p, PacketList list) {
	if (list->n > list->max) {
		list = realloc(list, list->max * 2);
	}
	list->list[(list->n)++] = p;
	return list;
}

int getn_packet_list(PacketList list) {
	return list->n;
}

unsigned char * string_packet(PacketList list, int n) {
	int i;
	Packet p = list->list[n];
	unsigned char * retBuf;
	retBuf = malloc(LENPACKET * sizeof(char));
	retBuf[0] = p->rssi;
	for (i = 0; i < 6; i++) {
		retBuf[1 + i] = p->mac[i];
	}
	for (i = 0; i < 4; i++) {
		retBuf[7 + i] = p->timestamp[i];
	}
	return retBuf;
}

PacketList reset_packet_list(PacketList list) {
	free_packet_list(list);
	return init_packet_list();
}

void free_packet_list(PacketList list) {
	int i;
	for (i = 0; i < list->n; i++) {
		free(list->list[i]);
	}
	free(list->list);
	free(list);
}


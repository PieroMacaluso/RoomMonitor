#ifndef PACKET_H
#define PACKET_H
#include "packet.h"

struct packet {
	uint8_t rssi;
	uint8_t mac[7];
	uint32_t timestamp;
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

int setPacket(Packet p, const wifi_promiscuous_pkt_t *ppkt) {
	int i;
	p->rssi = ppkt->rx_ctrl.rssi;
	printf("RSSI = %d\n", (signed int)p->rssi);
	for (i = 0; i < 6; i++) {
		printf("%d\n", i);
		p->mac[i] = ppkt->payload[OFFMAC + i];
	}
	p->mac[7]='\0';
	p->timestamp = ppkt->rx_ctrl.timestamp;
	printf("time\n");
	return 0;
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
		list = realloc_packet_list(list, list->max * 2);
	}

	list->list[(list->n)++] = p;
	printf("Inserito correttamente");
	return list;
}

int getn_packet_list(PacketList list) {
	return list->n;
}

int send_packets(int s, PacketList list) {
	int i, j, result;
	for (i = 0; i < list->n; i++) {
		result = send(s, &(list->list[i]->rssi), 1, 0);
		if (result <= 0) {
			ESP_LOGI(TAGP, "Error send RSSI\n");
			return -2;
		}
		for (j = 0; j < 6; j++) {
			result = send(s, &(list->list[i]->mac[j]), 1, 0);
			if (result <= 0) {
				ESP_LOGI(TAGP, "Error send RSSI\n");
				return -2;
			}
		}
		result = send(s, &(list->list[i]->timestamp), 4, 0);
		if (result <= 0) {
			ESP_LOGI(TAGP, "Error send RSSI\n");
			return -2;
		}
		printf("Pack %c sent.\n", i);
	}
	return LENPACKET;
}
/*
unsigned char * string_packet(PacketList list, int n) {
	int i;
	Packet p = list->list[n];
	unsigned char * retBuf;
	retBuf = malloc(LENPACKET * sizeof(char));
	sprintf(retBuf, "%u%02x%02x%02x%02x%02x%02x%u", p->rssi, p->mac[0], p->mac[1], p->mac[2], p->mac[3], p->mac[4], p->mac[5], p->timestamp);
	//retBuf[7 + i] = p->timestamp[i];
	return retBuf;
}
*/
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

#endif


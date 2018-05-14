#ifndef PACKET_H
#define PACKET_H
#include "packet.h"

static const char *TAGP = "tcp_client";

typedef struct packet {
	signed rssi;
	uint8_t mac[7];
	uint32_t timestamp;
} Packet;

struct node {
	Packet * packet;
	struct node * next;
};

node_t init_packet_list() {
	node_t head;
	head = NULL;
	head = malloc(sizeof(struct node));
	head->packet = NULL;
	head->next = NULL;
	return head;
}

Packet* setPacket(const wifi_promiscuous_pkt_t *ppkt) {
	Packet *p;
	p = malloc(sizeof(Packet));
	int i;
	p->rssi = ppkt->rx_ctrl.rssi;
//	printf("RSSI = %d\n", (signed int) p->rssi);
	for (i = 0; i < 6; i++) {
//		printf("%d\n", i);
		p->mac[i] = ppkt->payload[OFFMAC + i];
	}
	p->mac[7] = '\0';
	p->timestamp = ppkt->rx_ctrl.timestamp;
//	printf("time\n");
	return p;
}

node_t addto_packet_list(const wifi_promiscuous_pkt_t *ppkt, node_t h) {
	node_t current = h;

	if (current->packet == NULL) {
		h->packet = setPacket(ppkt);
		h->next = NULL;
		return h;
	}

	while (current->next != NULL)
		current = current->next;
	current->next = malloc(sizeof(struct node));
	current->next->packet = setPacket(ppkt);
	current->next->next = NULL;
//	printf("Inserito correttamente");
	return h;
}

int send_packets(int s, node_t h) {
	char buf[128];
	int j, result;
	node_t i;
	for (i = h; i != NULL; i = i->next) {
		sprintf(buf, "%d %02x:%02x:%02x:%02x:%02x:%02x %u", i->packet->rssi,
				i->packet->mac[0], i->packet->mac[1], i->packet->mac[2],
				i->packet->mac[3], i->packet->mac[4], i->packet->mac[5],
				i->packet->timestamp);
		result = send(s, buf, 127, 0);
				if (result <= 0) {
					ESP_LOGI(TAGP, "Error send RSSI\n");
					return -2;
				}
/*
		result = send(s, &(i->packet->rssi), 1, 0);
		if (result <= 0) {
			ESP_LOGI(TAGP, "Error send RSSI\n");
			return -2;
		}
		for (j = 0; j < 6; j++) {
			result = send(s, &(i->packet->mac[j]), 1, 0);
			if (result <= 0) {
				ESP_LOGI(TAGP, "Error send RSSI\n");
				return -2;
			}
		}
		result = send(s, &(i->packet->timestamp), 4, 0);
		if (result <= 0) {
			ESP_LOGI(TAGP, "Error send RSSI\n");
			return -2;
		}*/
	}
	return LENPACKET;
}

void free_node(node_t n) {
	if (n->next != NULL)
		free_node(n->next);
	free(n);
	n = NULL;
}

void free_packet_list(node_t h) {
	printf("FREE\n");
	free_node(h);

}
void reset_packet_list(node_t h) {
	printf("RESET\n");
	if (h->next != NULL)
		free_node(h->next);
	h->packet = NULL;
	h->next = NULL;
}

#endif


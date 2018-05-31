#ifndef PACKET_H
#define PACKET_H
#include "packet.h"

static const char *TAGP = "tcp_client";
char macESP[18];					//dopo l'init conterrà il mac della esp

typedef struct packet {
	uint32_t fcs;
	signed rssi;
	uint8_t mac[7];
	uint32_t timestamp;
	char * SSID;
	char board[18];
} Packet;

struct node {
	Packet * packet;
	struct node * next;
};

node_t init_packet_list(char baseMacChr[18]) {
	node_t head;
	head = NULL;
	head = malloc(sizeof(struct node));
	head->packet = NULL;
	head->next = NULL;

	strcpy(macESP,baseMacChr);

	return head;
}

char * getSSID(uint16_t start, uint16_t size, const uint8_t* data) {
	char * string;
	if (size== 0) {
		string = malloc(2*sizeof(char));
		strcpy(string, "~");
		return string;

	}
	string = malloc(size+1*sizeof(char));
	for (uint16_t i = start; i < start + size; i++) {
		sprintf(string+(i-start), "%c", data[i]);
	}
	string[size] = '\0';
	return string;
}

Packet* setPacket(const wifi_promiscuous_pkt_t *ppkt) {
	time_t now;
	time(&now);
	Packet *p;
	p = malloc(sizeof(Packet));
	int i;

	p->fcs = crc32_le(0, ppkt->payload, ppkt->rx_ctrl.sig_len);
	p->rssi = ppkt->rx_ctrl.rssi;
	for (i = 0; i < 6; i++) {
		p->mac[i] = ppkt->payload[OFFMAC + i];
	}
	p->mac[7] = '\0';
	p->timestamp = now;
	uint8_t SSID_length = ppkt->payload[25];
	p->SSID = getSSID(26, SSID_length, ppkt->payload);
	strcpy(p->board,macESP);

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
	int result;
	node_t i;
	for (i = h; i != NULL; i = i->next) {
		sprintf(buf, "%08x %d %02x:%02x:%02x:%02x:%02x:%02x %u %s %s", i->packet->fcs, i->packet->rssi,
				i->packet->mac[0], i->packet->mac[1], i->packet->mac[2],
				i->packet->mac[3], i->packet->mac[4], i->packet->mac[5],
				i->packet->timestamp, i->packet->SSID,i->packet->board);
		result = send(s, buf, 127, 0);
				if (result <= 0) {
					ESP_LOGI(TAGP, "Error send RSSI\n");
					return -2;
				}
	}
	return LENPACKET;
}

void free_node(node_t n) {
	if (n->next != NULL)
		free_node(n->next);
	free(n->packet->SSID);
	free(n);
	n = NULL;
}

void free_packet_list(node_t h) {
	free_node(h);

}
void reset_packet_list(node_t h) {
	if (h->next != NULL)
		free_node(h->next);
	free(h->packet->SSID);
	free(h->packet);
	h->packet = NULL;
	h->next = NULL;
}

#endif


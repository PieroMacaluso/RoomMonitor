#ifndef PACKET_H
#define PACKET_H

#include "packet.h"

static const char *TAGP = "tcp_client";
char macESP[18];                    //dopo l'init conterr� il mac della esp
uint8_t id;
uint8_t posx;
uint8_t posy;

int get_id() {
    return id;
}

int get_posx() {
    return posx;
}

int get_posy() {
    return posy;
}

typedef struct packet {
    uint32_t fcs;
    signed rssi;
    uint8_t mac[7];
    uint32_t timestamp;
    char *SSID;
    char board[18];
} Packet;

struct node {
    Packet *packet;
    struct node *next;
};

/**
 * Preleva dalla memoria chiave-valore un intero data la chiave stringa.
 * In caso non fosse presente ritorna 0 come valore di default.
 **/
int my_nvs_get_str_to_int(char *key) {
    esp_err_t err;
    char *value = NULL;
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    size_t required_size;
    err = nvs_get_str(my_handle, key, NULL, &required_size);
    int ret = 0;
    if (err == ESP_OK) {
        value = malloc(required_size);
        nvs_get_str(my_handle, key, value, &required_size);
        printf("- NVS: %s, %s\n", key, value);
        ret = atoi(value);
        free(value);
    }
    nvs_close(my_handle);
    return ret;
}

node_t init_packet_list(char baseMacChr[18]) {
    id = my_nvs_get_str_to_int("id");
    posx = my_nvs_get_str_to_int("posx");
    posy = my_nvs_get_str_to_int("posy");

    node_t head;
    head = NULL;
    head = malloc(sizeof(struct node));
    head->packet = NULL;
    head->next = NULL;

    strcpy(macESP, baseMacChr);

    return head;
}

char *getSSID(uint16_t start, uint16_t size, const uint8_t *data) {
    char *string;
    if (size == 0) {
        string = malloc(2 * sizeof(char));
        strcpy(string, "~");
        return string;

    }
    string = malloc(size + 1 * sizeof(char));
    for (uint16_t i = start; i < start + size; i++) {
        sprintf(string + (i - start), "%c", data[i]);
    }
    string[size] = '\0';
    return string;
}

Packet *setPacket(const wifi_promiscuous_pkt_t *ppkt) {
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
    p->mac[6] = '\0';
    p->timestamp = now;
    uint8_t SSID_length = ppkt->payload[25];
    p->SSID = getSSID(26, SSID_length, ppkt->payload);
    strcpy(p->board, macESP);

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

    if (h->next == NULL)
        return LENPACKET;

    for (i = h; i != NULL; i = i->next) {
        sprintf(buf, "%d %d %d %08x %d %02x:%02x:%02x:%02x:%02x:%02x %u %s %s", id, posx, posy, i->packet->fcs,
                i->packet->rssi,
                i->packet->mac[0], i->packet->mac[1], i->packet->mac[2],
                i->packet->mac[3], i->packet->mac[4], i->packet->mac[5],
                i->packet->timestamp, i->packet->SSID, i->packet->board);
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
/**
 * Recursive function to free the list of packets.
 * @param h Head of the packet linked list
 */
void reset_packet_list(node_t h) {
    if (h->next != NULL){
        free_node(h->next);
        free(h->packet->SSID);
        free(h->packet);
        h->packet = NULL;
        h->next = NULL;
    }
}

#endif

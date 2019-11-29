#ifndef PACKET_H
#define PACKET_H

#include <utils/common.h>
#include <crypto/sha256.h>
#include <mbedtls/md.h>
#include "packet.h"

static const char *TAGP = "tcp_client";
// Dopo l'init conterrà il mac della esp
char macESP[18];
uint8_t id;

int get_id() {
    return id;
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
 * Inizializzazione della lista dei pacchetti
 * @param baseMacChr    MAC della schedina
 * @return              Puntatore alla lista linkata
 */
node_t init_packet_list(char baseMacChr[18]) {
    id = my_nvs_get_str_to_int("id");

    node_t head;
    head = NULL;
    head = malloc(sizeof(struct node));
    head->packet = NULL;
    head->next = NULL;
    strcpy(macESP, baseMacChr);
    return head;
}

/**
 * Ottiene SSID Da punto di inizio e dimensione, partendo da data
 * @param start     Intero di inizio
 * @param size      Dimensione SSID
 * @param data      Payload
 * @return          Vettore di caratteri contenente l'SSID o, se non presente, una tilde (~)
 */
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

/**
 * Partendo dalla struttura dati wifi_promiscuous_pkt_t va a riempire i dati del pacchetto
 * @param ppkt      wifi_promiscuous_pkt_t
 * @return          Pacchetto compilato
 */
Packet *setPacket(const wifi_promiscuous_pkt_t *ppkt) {
    time_t now;
    time(&now);
    Packet *p;
    p = malloc(sizeof(Packet));
    int i;
    uint32_t size = ppkt->rx_ctrl.sig_len;
    p->fcs = crc32_le(0, ppkt->payload, size);
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

/**
 * Aggiunta pacchetto in struttura wifi_promiscuous_pkt_t alla head indicata
 * @param ppkt      wifi_promiscuous_pkt_t pacchetto
 * @param h         Testa lista linkata
 * @return          Ritorno testa lista linkata
 */
node_t addto_packet_list(const wifi_promiscuous_pkt_t *ppkt, node_t h) {
    node_t current = h;

    if (h->packet == NULL) {
//        printf("head");
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

/**
 * Procedura di invio pacchetti
 * @param s         TCP Socket
 * @param h         Testa lista linkata pacchetti
 * @return          LENPACKET se successo, valori negativi altrimenti
 */
int send_packets(int s, node_t h) {
    char buf[1024];
    int result;
    node_t i;

    int j;
    /**
     * Costruisco il contesto per calcolo HMAC con SHA256
     */
    char * key = my_nvs_get_str("secret");
    if (key == NULL) {
        key = "root";
    }
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;
    const size_t keyLength = strlen(key);
    size_t payloadLength = 0;
    // Risultato HMAC
    unsigned char hmacResult[32];

    if (h->next == NULL)
        return LENPACKET;
    for (i = h; i != NULL; i = i->next) {
        sprintf(buf, "%d,%08x,%d,%02x:%02x:%02x:%02x:%02x:%02x,%u,%s,%s|", id, i->packet->fcs,
                i->packet->rssi,
                i->packet->mac[0], i->packet->mac[1], i->packet->mac[2],
                i->packet->mac[3], i->packet->mac[4], i->packet->mac[5],
                i->packet->timestamp, i->packet->SSID, i->packet->board);
        payloadLength = strlen(buf);
        // Calcolo HMAC
        mbedtls_md_init(&ctx);
        mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
        mbedtls_md_hmac_starts(&ctx, (const unsigned char *) key, keyLength);
        mbedtls_md_hmac_update(&ctx, (const unsigned char *) buf, payloadLength);
        mbedtls_md_hmac_finish(&ctx, (unsigned char *) hmacResult);
        mbedtls_md_free(&ctx);

        // Concateniamo HMAC ottenuto
        for (j = 0; j < 32; j++) {
            sprintf(buf + strlen(buf), "%02x", hmacResult[j]);
        }
        // Terminatore di stringa.
        sprintf(buf + strlen(buf), ";");
//        fprintf(stdout, "%s\n", buf);
        result = send(s, buf, strlen(buf), 0);
        if (result <= 0) {
            ESP_LOGI(TAGP, "Error send RSSI\n");
            return -2;
        }
    }
    free(key);
    return LENPACKET;
}

/**
 * Liberazione della struttura dati di un nodo
 * @param n     struttura nodo
 */
void freeNode(node_t curr) {

    if (curr->packet->SSID != NULL) {
        free(curr->packet->SSID);
        curr->packet->SSID = NULL;
    }
    // No free su next perchè è già null
    free(curr->packet);
    curr->packet = NULL;
    curr->next = NULL;
}

/**
 * Funzione di liberazione di tutta la lista
 * @param h     testa lista linkata
 */
void reset_packet_list(node_t h) {

    node_t curr;
    node_t iterator = h;
    iterator = iterator->next;

    // Set curr to head, stop if list empty.
    while ((curr = iterator) != NULL) {
        // advance head to next element.
        iterator = iterator->next;
//        printf("free %08x", curr->packet->fcs);
        // delete saved pointer
        freeNode(curr);
        free(curr);
        curr = NULL;
    }
//    printf("free head %08x", h->packet->fcs);
    free(h->packet->SSID);
    h->packet->SSID = NULL;
    free(h->packet);
    h->packet = NULL;
    h->next = NULL;
    //return h;

}

/**
 * Stampa di tutta la lista, usata per debug
 * @param n testa della lista
 */
void print_all_list(node_t h) {

    if (h->packet == NULL) return;

    node_t cur;
    printf("STAMPO LISTA\n");
    for (cur = h; cur != NULL; cur = cur->next) {
        printf("%d,%08x,%d,%02x:%02x:%02x:%02x:%02x:%02x,%u,%s,%s\n", id, cur->packet->fcs,
               cur->packet->rssi,
               cur->packet->mac[0], cur->packet->mac[1], cur->packet->mac[2],
               cur->packet->mac[3], cur->packet->mac[4], cur->packet->mac[5],
               cur->packet->timestamp, cur->packet->SSID, cur->packet->board);
    }

}

#endif

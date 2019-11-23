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
    struct node *prev;
};

/**
 * Preleva dalla memoria chiave-valore un intero data la chiave stringa.
 * In caso non fosse presente ritorna 0 come valore di default.
 *
 * @param key chiave del valore da ricercare nella memoria
 * @return Intero presente nella chiave valore, altrimenti 0 come default
 */
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
    head->prev = NULL;
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

    if (current->packet == NULL) {
        h->packet = setPacket(ppkt);
        h->next = NULL;
        h->prev = NULL;
        return h;
    }

    while (current->next != NULL)
        current = current->next;
    current->next = malloc(sizeof(struct node));
    current->next->packet = setPacket(ppkt);
    current->next->next = NULL;
    current->next->prev = current;
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
    char buf[256];
    int result;
    node_t i;

    int j;
    /**
     * Costruisco il contesto per calcolo HMAC con SHA256
     */
    // TODO: controllare bene se questa sia una chiave da 256-bit
    char *key = "eThVmYq3t6w9z$C&F)J@NcRfUjXn2r4u";
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
        fprintf(stdout, "%s\n", buf);
        result = send(s, buf, strlen(buf), 0);
        if (result <= 0) {
            ESP_LOGI(TAGP, "Error send RSSI\n");
            return -2;
        }
    }
    return LENPACKET;
}

/**
 * Liberazione della struttura dati di un nodo
 * @param n     struttura nodo
 */
void freeNode(node_t n) {

    node_t current = n;
    node_t prev;

    // Arrivo alla fine della lista
    while (current->next != NULL)
        current = current->next;

    // While che elimina l'ultimo pacchetto e risale di uno tornando verso la testa (Testa = pacchetto con prev==NULL)
    while (current->prev != NULL) {
        if (current->packet->SSID != NULL)
            free(current->packet->SSID);
        // No free su next perchè è già null
        free(current->packet);
        prev = current->prev;
        free(current);
        current = prev;
    }
}

// TODO: Controllare queste due funzione ed eliminare l'intrusa rifattorizzando
/**
 * Funzione di liberazione di tutta la lista
 * @param h     testa lista linkata
 */
void free_packet_list(node_t h) {
    freeNode(h);
}

/**
 * Recursive function to free the list of packets.
 * @param h Head of the packet linked list
 */
void reset_packet_list(node_t h) {
    // If the HEAD is NULL or the head is not null but there aren't packets , simply returns
    if (h == NULL) return;
    if (h->packet == NULL) return;

    if (h->next != NULL) {
        // Next is not NULL, so Recursive Free on nodes!
        freeNode(h->next);
    }

    /*if(h->packet->SSID!=NULL){          //todo controllare perchè in free_node chiama già free di SSID e altro
        free(h->packet->SSID);
        free(h->packet);
    }*/

    h->packet = NULL;
    h->next = NULL;
    h->prev = NULL;
}

#endif

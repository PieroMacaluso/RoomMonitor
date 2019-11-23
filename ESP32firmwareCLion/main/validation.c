//
// Created by root on 9/27/19.
//

/**
 * File creato per contenere le varie funzioni di validazioni degli input.
 * Valore di ritorno: 1 in caso di errore, 0 in caso di successo
 * */

#include <stdlib.h>
#include <stdio.h>
#include "validation.h"

//id solo numero
//ssid no simboli, solo alfanumerico min 1
// ps 1 min 1 Maiusc e 1 num min  max
//ip



int idValidation(char *id) {
    unsigned i;
    if (strlen(id) > 5 || strlen(id) == 0) {
        return 1;
    }
    for (i = 0; i < strlen(id); i++) {
        if (!(id[i] >= '0' && id[i] <= '9')) {
            return 1;
        }
    }
    return 0;
}

int ssidApValidation(char *ssid_ap) {
    unsigned i;

    if (strlen(ssid_ap) == 0 || strlen(ssid_ap) > 50)
        return 2;

    for (i = 0; i < strlen(ssid_ap); i++) {
        // Non possibili caratteri speciali e < >
        if (!isalnum((unsigned char) ssid_ap[i])) {
            return 2;
        }
    }
    return 0;
}

int passwordApValidation(char *pass) {
    int i;
    int min = 0, Maius = 0, num = 0;

    if (strlen(pass) < 3 || strlen(pass) > 30)
        return 3;

    for (i = 0; i < strlen(pass); i++) {
        if (isdigit((unsigned char) pass[i]))
            num = 1;
        if (islower((unsigned char) pass[i]))
            min = 1;
        if (isupper((unsigned char) pass[i]))
            Maius = 1;
    }

    if (num == 1 && min == 1 && Maius == 1)
        return 0;
    else
        return 3;
}

int channelValidation(char *channel) {
    int ch;

    ch = atoi(channel);
    if (ch == 0)
        return 4;

    if (ch > 0 && ch <= 13)      //solo canali 2.4GHz
        return 0;
    else
        return 4;
}

int ssidServerValidation(char *ssid_server) {
    if (strlen(ssid_server) == 0 || strlen(ssid_server) > 50)
        return 5;
    else
        return 0;
}

int passServerValidation(char *password_server) {

    if (strlen(password_server) < 3 || strlen(password_server) > 50)
        return 6;
    else
        return 0;
}

int ipValidation(char *ip_server) {
    int i;
    int numDot = 0;
    int num;
    char *numC;
    char *tempIp = calloc(strlen(ip_server) + 1, sizeof(char));
    strcpy(tempIp, ip_server);

    //0.0.0.0 -> 255.255.255.255

    if (strlen(tempIp) < 7 || strlen(tempIp) > 15)
        return 7;

    for (i = 0; i < strlen(tempIp); i++) {

        if (!((tempIp[i] >= '0' && tempIp[i] <= '9') || tempIp[i] == '.'))           //isdigit non piace
            return 8;

        //Conteggio numero punti
        if (tempIp[i] == '.') {
            numDot++;
            if (numDot > 3)
                return 9;
        }

    }

    //Verifica 4 numeri tra 0 e 255
    for (i = 0; i < 4; i++) {
        numC = strtok(tempIp, ".");
        num = atoi(numC);
        if (num < 0 || num > 255)
            return 10;
    }
    free(tempIp);

    return 0;
}



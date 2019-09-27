//
// Created by root on 9/27/19.
//

/**
 * File creato per contenere le varie funzioni di validazioni degli input.
 * Valore di ritorno: 1 in caso di errore, 0 in caso di successo
 * */

#include <stdlib.h>
#include "validation.h"

//id solo numero
//ssid no simboli, solo alfanumerico min 1
// ps 1 min 1 Maiusc e 1 num min  max
//ip



int idValidation(char* id){
    int i;

    if(strlen(id)>5 || strlen(id)==0){
        return 1;
    }
    for(i=0;i<strlen(id);i++){
        if(!(id[i]>='0' && id[i]<='9')){
            return 1;
        }
    }
    return 0;
}

int ssidApValidation(char* ssid_ap){
    int i;

    if(strlen(ssid_ap)==0 || strlen(ssid_ap)>50 )
        return 1;

    for(i=0;i<strlen(ssid_ap);i++){

        if(!isalnum((unsigned char)ssid_ap[i])){          //non possibili caratteri speciali e < >
            return 1;
        }
    }
    return 0;
}

int passwordApValidation(char* pass){
    int i;
    int min=0,Maius=0,num=0;

    if(strlen(pass)<3 || strlen(pass)>30 )
        return 1;

    for(i=0;i<strlen(pass);i++){
        if(isdigit((unsigned char)pass[i]))
            num=1;
        if(islower((unsigned char)pass[i]))
            min=1;
        if(isupper((unsigned char)pass[i]))
            Maius=1;
    }

    if(num==1 && min==1 && Maius==1)
        return 0;
    else
        return 1;
}

int channelValidation(char* channel){
    int ch;

    ch=atoi(channel);
    if(ch==0)
        return 1;

    if(ch>0 && ch<=13)      //solo canali 2.4GHz
        return 0;
    else
        return 1;
}

int ssidServerValidation(char* ssid_server){
    if(strlen(ssid_server)==0 || strlen(ssid_server)>50 )
        return 1;
    else
        return 0;
}

int passServerValidation(char* password_server){

    if(strlen(password_server)<3 || strlen(password_server)>50 )
        return 1;
    else
        return 0;
}

int ipValidation(char* ip_server){
    int i;
    //0.0.0.0 -> 255.255.255.255
    if(strlen(ip_server)<7 || strlen(ip_server)>15)
        return 1;

    for(i=0;i<strlen(ip_server);i++){
        if(!(isdigit((unsigned char)ip_server) || ip_server[i]=='.'))
            return 1;
        else
            return 0;
    }
}



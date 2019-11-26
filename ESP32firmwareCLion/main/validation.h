//
// Created by root on 9/27/19.
//

#ifndef APP_TEMPLATE_VALIDATION_H
#define APP_TEMPLATE_VALIDATION_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>



int idValidation(char* id);
int ssidApValidation(char* ssid);
int passwordApValidation(char* pass);
int channelValidation(char* channel);
int portValidation(char *port);
int ssidServerValidation(char* ssid_server);
int passServerValidation(char* password_server);
int ipValidation(char* ip_server);
#endif //APP_TEMPLATE_VALIDATION_H

#ifndef ROOMMONITOR_FIRMWARE_VALIDATION_H
#define ROOMMONITOR_FIRMWARE_VALIDATION_H

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

int idValidation(char *id);

int ssidApValidation(char *ssid);

int passwordApValidation(char *pass);

int channelValidation(char *channel);

int portValidation(char *port);

int ssidServerValidation(char *ssid_server);

int passServerValidation(char *password_server);

int ipValidation(char *ip_server);

#endif //ROOMMONITOR_FIRMWARE_VALIDATION_H

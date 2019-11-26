//
// Created by pieromack on 26/11/19.
//

#ifndef ROOMMONITOR_FIRMWARE_HTTPS_SERVER_ESP_H
#define ROOMMONITOR_FIRMWARE_HTTPS_SERVER_ESP_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp32/rom/crc.h>
#include <string.h>
#include <math.h>
#include <cJSON.h>
#include <soc/timer_group_struct.h>
#include <driver/timer.h>
#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/api.h>
#include "packet.h"
#include "time.h"
#include <esp_spiffs.h>
#include <esp_err.h>
#include "validation.h"
#include <esp_http_server.h>
#include <esp_https_server.h>
#include <sys/param.h>
#include <esp_vfs.h>

esp_err_t start_webserver(void);
#endif //ROOMMONITOR_FIRMWARE_HTTPS_SERVER_ESP_H

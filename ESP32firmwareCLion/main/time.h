//
// Created by pieromack on 22/05/19.
//

#ifndef APP_TEMPLATE_TIME_H
#define APP_TEMPLATE_TIME_H


#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string.h>
#include <math.h>
#include "soc/timer_group_struct.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "lwip/sockets.h"
#include "driver/gpio.h"
#include "esp_sntp.h"
// #include "apps/sntp/sntp.h"

#define TIMER_INTR_SEL TIMER_INTR_LEVEL  /*!< Timer level interrupt */
#define TIMER_GROUP    TIMER_GROUP_0     /*!< Test on timer group 0 */
#define TIMER_DIVIDER   80               /*!< Hardware timer clock divider, 80 to get 1MHz clock to timer */
#define TIMER_SCALE    (TIMER_BASE_CLK / TIMER_DIVIDER)  /*!< used to calculate counter value */
#define TIMER_FINE_ADJ   (0*(TIMER_BASE_CLK / TIMER_DIVIDER)/1000000) /*!< used to compensate alarm value */
#define SECOND_SCAN_MODE	60
#define SECOND_CHECK_TIME   300			//ogni 300 sec=5 min
#define TIMER_INTERVAL0_SEC   (SECOND_SCAN_MODE)   /*!< test interval for timer 0 */


//static int mod; 	//0=> scan 1=>send server

void timer0_init();
void timer0_start();
void IRAM_ATTR timer_group0_isr(void *para);
bool initialize_sntp(void);
bool obtain_time(void);
void checkTime(int* n);

#endif //APP_TEMPLATE_TIME_H


#ifndef TIME_H
#define TIME_H
#include "time.h"


/*
 * @brief Init timer usato per periodo "scan paccheti"
 */
void timer0_init() {
    int timer_group = TIMER_GROUP_0;
    int timer_idx = TIMER_0;
    timer_config_t config;
    config.alarm_en = 1;	//per abilitare l'allarme ogni TIMER_INTERVAL0_SEC
    config.auto_reload = 1;
    config.counter_dir = TIMER_COUNT_UP;	//abilita conteggio da 0 in avanti
    config.divider = TIMER_DIVIDER;
    config.intr_type = TIMER_INTR_SEL;
    config.counter_en = TIMER_PAUSE;
    /*Configure timer*/
    timer_init(timer_group, timer_idx, &config);
    /*Stop timer counter*/
    timer_pause(timer_group, timer_idx);
    /*Load counter value */
    timer_set_counter_value(timer_group, timer_idx, 0x00000000ULL);
    /*Set alarm value*/
    timer_set_alarm_value(timer_group, timer_idx,
                          (TIMER_INTERVAL0_SEC * TIMER_SCALE) - TIMER_FINE_ADJ);
    /*Enable timer interrupt*/
    timer_enable_intr(timer_group, timer_idx);
    /*Set ISR handler*/
    timer_isr_register(timer_group, timer_idx, timer_group0_isr,				//timer_group0_isr handler in main.c
                       (void*) timer_idx, ESP_INTR_FLAG_IRAM, NULL);
    /*Start timer counter*/
    timer_start(timer_group, timer_idx);
}

/* protocollo sntp*/
void initialize_sntp(void)
{
    printf("Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}

void obtain_time(void)
{

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        printf( "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    ESP_ERROR_CHECK( esp_wifi_stop() );
}

void checkTime(int* n){

    (*n)++;
    if((*n)==/*SECOND_CHECK_TIME/SECOND_SCAN_MODE*/2 || (*n)==-1 ){		//todo		// SECOND_CHECK_TIME/SECOND_SCAN_MODE =numero di volte che il timer deve scadere per rappresentare il periodo di settaggio orario
        time_t now;
        struct tm timeinfo;
        time(&now);
        localtime_r(&now, &timeinfo);
        // Is time set? If not, tm_year will be (1970 - 1900).
        ESP_LOGI("sntp client", "Time resetting.");
        obtain_time();
        // update 'now' variable with current time
        time(&now);

        char strftime_buf[64];

        // Set timezone to Eastern Standard Time and print local time
        setenv("TZ", "CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", 1);
        tzset();
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        printf( "The current date/time in Italy is: %s\n", strftime_buf);
        (*n)=0;
    }




}





#endif

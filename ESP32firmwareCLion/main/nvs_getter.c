#include "nvs_getter.h"

/**
 * Funzione che permette di ottenere il valore salvato nella memoria NVS come vettore di char
 * @param key: chiave del valore salvato nella memoria NVS
 * @return valore contenuto nella memoria NVS se questo è presente, altrimenti viene ritornato NULL
 */
char *my_nvs_get_str(char *key) {
    esp_err_t err;
    char *value = NULL;
    nvs_handle my_handle;
    ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
    size_t required_size;
    err = nvs_get_str(my_handle, key, NULL, &required_size);
    if (err == ESP_OK) {
        value = malloc(required_size);
        nvs_get_str(my_handle, key, value, &required_size);
        printf("- NVS: %s, %s\n", key, value);
    }
    nvs_close(my_handle);
    return value;
}


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
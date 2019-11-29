//
// Created by pieromack on 26/11/19.
//

#include "https_server_esp.h"

static const char *TAG = "room_monitor";

/** SERVER VAR **/
#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)
#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)
#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);

static esp_err_t rest_common_get_handler(httpd_req_t *req);

static esp_err_t echo_post_handler(httpd_req_t *req);

int spiffs_save(char *resource);

/** SERVER */
/* Set HTTP response content type according to file extension */

/**
 * Scegli il content/type della risposta HTTP a seconda dell'estensione del file
 * @param req       richiesta HTTP
 * @param filepath  file richiesto
 * @return          esp_err_t
 */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath) {
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/**
 * Send HTTP response with the contents of the requested file
 * @param req Richiesta
 * @return esp_err_t
 */
static esp_err_t rest_common_get_handler(httpd_req_t *req) {
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *) req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req) {
    char result[2048];
    result[0] = '\0';
    char buf[1024];
    buf[0] = '\0';
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        // 200
        if (remaining == req->content_len) {
            strncpy(result, buf, strlen(buf));
        } else {
            strcat(result, buf);
        }
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }
    result[req->content_len] = '\0';
    if ((ret = spiffs_save(result)) != 0) {
        printf("%d\n", ret);
        printf("Salvataggio dati non andato a buon fine\n");
        httpd_resp_send_500(req);
    } else {
        printf("Salvataggio dati andato a buon fine\n");
        httpd_resp_send(req, NULL, 0);
        printf("ESP32 Restarting...\n");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t start_webserver(void) {
    char *base_path = "/spiffs";
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.httpd.uri_match_fn = &httpd_uri_match_wildcard;

    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_cacert_pem_end");
    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    REST_CHECK(httpd_ssl_start(&server, &conf) == ESP_OK, "Start server failed", err_start);

    ESP_LOGI(TAG, "Starting HTTP Server");

    httpd_uri_t data = {
            .uri       = "/data",
            .method    = HTTP_POST,
            .handler   = echo_post_handler,
            .user_ctx  = rest_context
    };

    httpd_uri_t common_get_uri = {
            .uri = "/*",
            .method = HTTP_GET,
            .handler = rest_common_get_handler,
            .user_ctx = rest_context
    };
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &data);
    httpd_register_uri_handler(server, &common_get_uri);

    return ESP_OK;
err_start:
    ESP_LOGI(TAG, "Error starting server!");
    free(rest_context);
    err:
    return ESP_FAIL;
}


/**
 * Richiesta per salvataggio dei dati all'interno della schedina tramite richiesta HTTP
 * @param resource      Risorsa Da Salvare
 * @param conn          Connessione
 * @return              Esito positivo ritorno 0, esito negativo diverso da 0
 */
int spiffs_save(char *resource) {
    cJSON *json = cJSON_Parse(resource);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return 1;
    }
    // Validazione input
    int res;

    char *id = cJSON_GetObjectItem(json, "id")->valuestring;
    res = idValidation(id);
    if (res != 0)
        return res;

    char *ssid_ap = cJSON_GetObjectItem(json, "ssid_ap")->valuestring;
    res = ssidApValidation(ssid_ap);
    if (res != 0)
        return res;

    char *password_ap = cJSON_GetObjectItem(json, "password_ap")->valuestring;
    res = passwordApValidation(password_ap);
    if (res != 0)
        return res;

    char *channel = cJSON_GetObjectItem(json, "channel")->valuestring;
    res = channelValidation(channel);
    if (res != 0)
        return res;

    char *ssid_server = cJSON_GetObjectItem(json, "ssid_server")->valuestring;
    res = ssidServerValidation(ssid_server);
    if (res != 0)
        return res;

    char *password_server = cJSON_GetObjectItem(json, "password_server")->valuestring;
    res = passServerValidation(password_server);
    if (res != 0)
        return res;

    char *ip_server = cJSON_GetObjectItem(json, "ip_server")->valuestring;
    res = ipValidation(ip_server);
    if (res != 0)
        return res;

    char *port = cJSON_GetObjectItem(json, "port_server")->valuestring;
    res = portValidation(port);
    if (res != 0)
        return res;

    char *secret = cJSON_GetObjectItem(json, "secret")->valuestring;
    res = passwordApValidation(secret);
    if (res != 0)
        return res;

    printf("Validazione input eseguita.");
    int err;

    FILE *f = fopen("/spiffs/data.json", "w");
    fprintf(f, "%s", cJSON_Print(json));
    fclose(f);
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        cJSON *current_element = NULL;
        char *current_key = NULL;
        cJSON_ArrayForEach(current_element, json) {
            current_key = current_element->string;
            if (current_key != NULL) {
                printf("%s %s\n", current_key, current_element->valuestring);
                err = nvs_set_str(my_handle, current_key, current_element->valuestring);
            }
        }
        printf("Committing updates in NVS ... ");
        switch (err) {
            case ESP_OK:
                printf("Saved\n");
                // indica che i dati ricevuti sono stati salvati, solo in tale situazione il funzionamento della schedina viene sbloccato
                err = nvs_set_str(my_handle, "saved", "saved");
                switch (err) {
                    case ESP_OK:
                        break;
                    case ESP_ERR_NVS_NOT_FOUND:
                        printf("The value is not initialized yet!\n");
                        break;
                    default:
                        printf("Error (%s) reading!\n", esp_err_to_name(err));
                }
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default:
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    }
    return 0;
}
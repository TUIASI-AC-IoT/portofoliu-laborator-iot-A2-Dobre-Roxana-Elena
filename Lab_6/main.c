/*  WiFi softAP Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "esp_http_server.h"

#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "soft-ap.h"
#include "http-server.h"
#include "../mdns/include/mdns.h"

#define PROVISIONING_BUTTON_GPIO 2
/** Maximum networks to store */
#define MAX_APs 10

// Function declarations
bool save_credentials(const char* ssid, const char* pass);
void scan_networks(void);

// Global variables
static const char *TAG = "main";

/* Verify if there are some existing saved credentials */
bool check_saved_credentials() {
    nvs_handle_t nvs;
    if (nvs_open("wifi_store", NVS_READONLY, &nvs) != ESP_OK) return false;
    
    char ssid[32] = {0};
    size_t len = sizeof(ssid);
    if (nvs_get_str(nvs, "ssid", ssid, &len) != ESP_OK) {
        nvs_close(nvs);
        return false;
    }
    nvs_close(nvs);
    return true;
}

bool save_credentials(const char* ssid, const char* pass) {
    nvs_handle_t nvs;
    if (nvs_open("wifi_store", NVS_READWRITE, &nvs) != ESP_OK) {
        return false;
    }
    
    if (nvs_set_str(nvs, "ssid", ssid) != ESP_OK) {
        nvs_close(nvs);
        return false;
    }
    
    if (nvs_set_str(nvs, "pass", pass) != ESP_OK) {
        nvs_close(nvs);
        return false;
    }
    
    nvs_commit(nvs);
    nvs_close(nvs);
    return true;
}

/* Check reset button */
void button_task(void* arg) {
    gpio_set_direction(PROVISIONING_BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PROVISIONING_BUTTON_GPIO, GPIO_PULLUP_ONLY);
    
    while(1) {
        if (gpio_get_level(PROVISIONING_BUTTON_GPIO) == 0) { // Pressed button
            vTaskDelay(5000 / portTICK_PERIOD_MS); // wait 5 seconds
            if (gpio_get_level(PROVISIONING_BUTTON_GPIO) == 0) { // still pressed button
                nvs_handle_t nvs;
                ESP_ERROR_CHECK(nvs_open("wifi_store", NVS_READWRITE, &nvs));
                nvs_erase_key(nvs, "ssid");
                nvs_erase_key(nvs, "pass");
                nvs_commit(nvs);
                nvs_close(nvs);
                esp_restart();
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/* Scan wifi networks */
void scan_networks() {
    wifi_scan_config_t scan_conf = {
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
        .scan_time.active.min = 100,
        .scan_time.active.max = 300
    };
    
    esp_wifi_scan_start(&scan_conf, true);
    
    uint16_t number = MAX_APs;  // Declare as uint16_t variable
    esp_wifi_scan_get_ap_records(&number, ap_info);  // Pass address of 'number'
    esp_wifi_scan_get_ap_num(&ap_count);
    
    ESP_LOGI(TAG, "Found %d/%d networks", ap_count, MAX_APs);
}

// WiFi event handler for station mode
static void wifi_event_handler(void* arg, esp_event_base_t event_base, 
                             int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Connection failed, retrying...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Connected! IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

// Connect to WiFi using saved credentials
void connect_to_network() {
    nvs_handle_t nvs;
    char ssid[32] = {0};
    char password[64] = {0};
    size_t len = sizeof(ssid);

    // Open NVS and read credentials
    if (nvs_open("wifi_store", NVS_READONLY, &nvs) == ESP_OK) {
        nvs_get_str(nvs, "ssid", ssid, &len);
        len = sizeof(password);
        nvs_get_str(nvs, "pass", password, &len);
        nvs_close(nvs);
    }

    // Configure WiFi in station mode
    wifi_config_t wifi_config = {0};
    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    // Initialize WiFi
    esp_netif_create_default_wifi_sta();
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));
}

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Verify saved credentials
    if (check_saved_credentials()) {
        ESP_LOGI(TAG, "Credential found, connecting to the network...");
        connect_to_network();
        while(1) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }

    // Start SoftAP mode (from soft-ap.h/c)
    wifi_init_softap();

    // Scan wifi networks
    scan_networks();

    // Initialization of  mDNS
    mdns_init();
    mdns_hostname_set("setup");
    mdns_instance_name_set("ESP32 Provisioning");

    // Start HTTP server (from http-server.h/c)
    start_webserver();

    // task for checking if the button is pressed
    xTaskCreate(button_task, "button_task", 2048, NULL, 5, NULL);

    while(1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
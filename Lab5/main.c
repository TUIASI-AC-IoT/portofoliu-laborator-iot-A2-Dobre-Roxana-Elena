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


#include "lwip/err.h"
#include "lwip/sys.h"

#include "soft-ap.h"
#include "http-server.h"

#include "../mdns/include/mdns.h"

#define DEFAULT_SCAN_LIST_SIZE 10

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_event_loop_create_default());  // just once in the code  -> in ceva ssid scanning mode? 

    // TODO: 3. SSID scanning in STA mode 
    uint16_t number = DEFAULT_SCAN_LIST_SIZE;
    wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
    uint16_t ap_count = 0;
    memset(ap_info, 0, sizeof(ap_info));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI("WiFi Scan", "Scanning for available networks...");
    ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));

    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
    ESP_LOGI("WiFi Scan", "Total APs found: %d", ap_count);

    for (int i = 0; i < ap_count; i++) {
        ESP_LOGI("WiFi Scan", "SSID: %s, RSSI: %d", ap_info[i].ssid, ap_info[i].rssi);
    }

    // TODO: 1. Start the softAP mode
    wifi_init_softap();
    
     // TODO: 4. mDNS init (if there is time left)   
    mdns_init();
    mdns_hostname_set("esp32");
    mdns_instance_name_set("ESP32 WebServer");

    // TODO: 2. Start the web server 
    start_webserver();
}
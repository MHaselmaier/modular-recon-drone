#include "drone.h"
#include "esp_wpa2.h"
#include <stdlib.h>
#include <time.h>

static const char* TAG = "accesspoint";

void accesspoint_init(const char* ssid, const char* password)
{

    ESP_LOGI(TAG, "Initializing access point controller using SSID: %s", ssid);
    nvs_flash_init();
    tcpip_adapter_init();
    
    ESP_LOGI(TAG, "Initializing access point controller using SSID: %s", ssid);
   
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    ESP_LOGI(TAG, "Generating access point password");
    char generated_password[9] = {0};
    sprintf(generated_password, "%8u", ((uint32_t)esp_random()));
    ESP_LOGI(TAG, "Generated access point password is [%s]", generated_password);
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid=SSID,
            .ssid_len=0,
            .channel=0,
            .password="",
            .authmode=WIFI_AUTH_WPA_WPA2_PSK,
            .ssid_hidden=0,
            .max_connection=1,
            .beacon_interval=100
        }
    };
    memcpy(ap_cfg.ap.password, generated_password, 9);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg)); 
}

void accesspoint_start()
{
    ESP_LOGI(TAG, "Starting access point controller");
    ESP_ERROR_CHECK(esp_wifi_start());
}

void accesspoint_stop()
{
    ESP_ERROR_CHECK(esp_wifi_stop());
}
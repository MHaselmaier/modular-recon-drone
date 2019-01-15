#include "drone.h"

static const char* TAG = "accesspoint";

void accesspoint_init(const char* ssid, const char* password)
{
    ESP_LOGI(TAG, "Initializing access point controller using SSID: %s", ssid);
    nvs_flash_init();
    tcpip_adapter_init();
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid="ESP WIFI",
            .ssid_len=0,
            .password="",
            .channel=0,
            .authmode=WIFI_AUTH_OPEN,
            .ssid_hidden=0,
            .max_connection=1,
            .beacon_interval=100
        }
    };

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
#include "drone.h"

static const char* TAG = "drone";

// ###### Functions
TickType_t port_delay_ms(uint32_t ms)
{
    return ms / portTICK_PERIOD_MS;
}

void event_group_init()
{
    wifi_event_group = xEventGroupCreate();
}

// ###### MAIN
static esp_err_t system_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI(TAG, "System event: Access Point started. Start up UDP server for motor data");
            server_start();
            camera_module_start();
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "System event: New client connection");
            xEventGroupSetBits(wifi_event_group, CLIENT_CONNECTED);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "System event: Client disconnected from Access Point");
            xEventGroupClearBits(wifi_event_group, CLIENT_CONNECTED);
            break;
        default:
            //ESP_LOGI("WIFI_EVENT", "Unhandled event...");
            break;
    }
    return ESP_OK;
}

void init()
{
    ESP_LOGI(TAG, "Initializing event groups");
    event_group_init();

    ESP_ERROR_CHECK(esp_event_loop_init(&system_event_handler, NULL));
    udp_to_i2c_com_queue = xQueueCreate(10, sizeof(struct MotorData));
    ESP_LOGI(TAG, "Installing gpio interrupt service");
    gpio_install_isr_service(0);
    accesspoint_init("ESP Wifi", NULL);
    //i2c_master_init();
    camera_module_init();
}

void app_main()
{
    init();
    //i2c_master_bus_start();
    accesspoint_start();
}

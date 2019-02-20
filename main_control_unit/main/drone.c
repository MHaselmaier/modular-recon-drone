#include "drone.h"
#define MAX(a,b)({a > b ? a : b;})
#define MIN(a,b)({a > b ? b : a;})

static const char* TAG = "drone";
EventGroupHandle_t system_event_group;
QueueHandle_t udp_to_i2c_com_queue;

// ###### Functions
TickType_t port_delay_ms(uint32_t ms)
{
    return ms / portTICK_PERIOD_MS;
}

void event_group_init()
{
    system_event_group = xEventGroupCreate();
}

// ###### System Event Handler
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
            xEventGroupSetBits(system_event_group, CLIENT_CONNECTED);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "System event: Client disconnected from Access Point");
            xEventGroupClearBits(system_event_group, CLIENT_CONNECTED);
            break;
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "Started STA");
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Disconnected. Reconnecting");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            break;
        default:
            ESP_LOGI("WIFI_EVENT", "Unhandled event... Don't worry, be happy");
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
    i2c_master_init();
    camera_module_init();
    ESP_LOGI(TAG, "Init process done");
}

// Main
void app_main()
{
    init();
    i2c_master_bus_start();
    accesspoint_start();
    wifi_config_t conf;
    display_draw_hskl();
    display_update();
    vTaskDelay(port_delay_ms(4000));
    while(true){
        display_clear();
        if(xEventGroupGetBits(system_event_group) & CLIENT_CONNECTED){
            wifi_sta_list_t client_list;
            esp_wifi_ap_get_sta_list(&client_list);
            char info[20];
            sprintf(info, "\t%04d mdb",client_list.sta[0].rssi);
            display_set_text(1, "Client connected", NULL);
            display_set_text(2, "RSSI:", info);
            display_update();
        }else{
            display_set_text(1, "Station:", SSID);
            esp_wifi_get_config(WIFI_IF_AP, &conf);
            display_set_text(2, "PW:", (char*) conf.ap.password);
            display_update();
        }
        vTaskDelay(port_delay_ms(1000));
    }
}

/*#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "sdkconfig.h"

#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_log.h"

#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
*/

#include "drone.h"
/*
// ###### GPIO Defines
#define GPIO_INTR_DEFAULT 0
// ###### GPIO Defines END

// ###### Static components
static int lightLevel = 0;
static xQueueHandle gpio_intr_queue = NULL;
// ###### Static components END

// ###### WIFI EventGroup
//static EventGroupHandle_t wifi_event_group;
const int CLIENT_CONNECTED = BIT0;
const int CLIENT_DISCONNECTED = BIT1;
const int AP_STARTED = BIT2;
// ###### WIFI EventGroup END


// ###### Method definitions
void IRAM_ATTR handle_gpio_interrupt(void* arg);
void gpio_interrupt_processing_task(void* arg);
//esp_err_t wifi_event_handler(void *ctx, system_event_t *event);
//void access_point_initialize_task(void *param);
void tcp_server_task(void *params);
// ###### Method definitions END

// ###### Static functions
static void setup_gpio_pins()
{
    gpio_pad_select_gpio(GPIO_NUM_25);
    gpio_set_direction(GPIO_NUM_25, GPIO_MODE_OUTPUT);
    // Set the GPIO as a push/pull output

    gpio_pad_select_gpio(GPIO_NUM_32);
    gpio_set_direction(GPIO_NUM_32, GPIO_MODE_INPUT);
    gpio_set_intr_type(GPIO_NUM_32, GPIO_PIN_INTR_NEGEDGE);
    gpio_set_pull_mode(GPIO_NUM_32, GPIO_PULLUP_ENABLE);
    
}

static void initialize_static_components()
{
    gpio_intr_queue = xQueueCreate(1, sizeof(uint32_t));
    //wifi_event_group = xEventGroupCreate();
}

static void initialize_tasks()
{
    xTaskCreate(&gpio_interrupt_processing_task, "gpio_intr_processing_task", 2048, NULL, 10, NULL);
    //xTaskCreate(&access_point_initialize_task, "access_point_task", 2048, NULL, 10, NULL);
    //xTaskCreate(&tcp_server_task, "tcp_server_task", 4096, NULL, 10, NULL);
}

static void setup_interrupt_components()
{
    ESP_ERROR_CHECK(gpio_install_isr_service(GPIO_INTR_DEFAULT));
    ESP_ERROR_CHECK(gpio_isr_handler_add(GPIO_NUM_32, &handle_gpio_interrupt, (void*) GPIO_NUM_34));
}

// ###### Static functions END

// ###### Functions

void IRAM_ATTR handle_gpio_interrupt(void* arg){
    uint32_t io_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_intr_queue, &io_num, NULL);
}

void gpio_interrupt_processing_task(void* arg)
{
    uint32_t message;
    while(1)
    {
        if(xQueueReceive(gpio_intr_queue, &message, portMAX_DELAY))
        {
            gpio_set_level(GPIO_NUM_25, lightLevel);
            lightLevel = lightLevel == 0 ? 1 : 0;
            ESP_LOGI("GPIO_TASK", "Received interrupt...");
        }
    }
}*/

/*static esp_err_t esp_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI("WIFI_EVENT", "AP started");
            //xEventGroupSetBits(wifi_event_group, AP_STARTED);
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI("WIFI_EVENT", "New client connected...");
            //xEventGroupSetBits(wifi_event_group, CLIENT_CONNECTED);
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI("WIFI_EVENT", "Client disconnected...");
            //xEventGroupSetBits(wifi_event_group, CLIENT_DISCONNECTED);
        default:
            ESP_LOGI("WIFI_EVENT", "Another event...");
            break;
    }
    return ESP_OK;
}*/

/*
void access_point_initialize_task(void *param)
{
    nvs_flash_init();
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(&wifi_event_handler, NULL));
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t ap_cfg = {
        .ap = {
            .ssid="ESP32 Testapp",
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
    ESP_ERROR_CHECK(esp_wifi_start());

    vTaskDelete(NULL);
}*/

/*void tcp_server_task(void *params)
{
    //xEventGroupWaitBits(wifi_event_group, AP_STARTED, false, true, portMAX_DELAY);
    TcpServer server;

    server.start();

    ESP_LOGI("TCP_TASK", "Starting server...");
    struct sockaddr_in tcp_server_addr;
    tcp_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_server_addr.sin_family = AF_INET;
    tcp_server_addr.sin_port = htons(3030);

    int client_socket_descriptor, server_socket_descriptor;
    char message_buffer[64];

    ESP_LOGI("TCP_TASK", "Waiting for AP startup to receive socket...");
    xEventGroupWaitBits(wifi_event_group, AP_STARTED, false, true, portMAX_DELAY);
    server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    bind(server_socket_descriptor, (struct sockaddr *) &tcp_server_addr, sizeof(tcp_server_addr));

    listen(server_socket_descriptor, 1);

    struct sockaddr_in tcp_client_addr;
    unsigned int client_socket_len = sizeof(tcp_client_addr);
    int received_bytes;

    while(1)
    {
        ESP_LOGI("TCP_TASK", "Waiting for new client connection...");
        client_socket_descriptor = accept(server_socket_descriptor, (struct sockaddr *) &tcp_client_addr, &client_socket_len);
        ESP_LOGI("TCP_TASK", "Connection with client established...");

        do
        {
            bzero(message_buffer, sizeof(message_buffer));
            received_bytes = recv(client_socket_descriptor, message_buffer, sizeof(message_buffer) - 1, 0);

            for(int i = 0; i < received_bytes; i++)
            {
                putchar(message_buffer[i]);
            }
        }while(received_bytes > 0 || (xEventGroupGetBits(wifi_event_group) & CLIENT_CONNECTED));
        ESP_LOGI("TCP_TASK", "Client disconnected...");
    }
    
}*/

// ###### Functions
TickType_t port_delay_ms(uint32_t ms)
{
    return ms / portMAX_DELAY;
}

// ###### MAIN

static esp_err_t system_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id)
    {
        case SYSTEM_EVENT_AP_START:
            ESP_LOGI("WIFI_EVENT", "AP started");
            server_start();
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            ESP_LOGI("WIFI_EVENT", "New client connected... Starting UDP server...");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            ESP_LOGI("WIFI_EVENT", "Client disconnected...");
        default:
            ESP_LOGI("WIFI_EVENT", "Unhandled event...");
            break;
    }
    return ESP_OK;
}

void init()
{
    ESP_ERROR_CHECK(esp_event_loop_init(&system_event_handler, NULL));
    udp_to_i2c_com_queue = xQueueCreate(10, sizeof(struct MotorData));
    accesspoint_init("ESP Wifi", NULL);
    i2c_master_init();
    //motor_control_init();
}

void app_main()
{
    init();
    i2c_master_bus_start();
    //motor_control_start();
    accesspoint_start();
}

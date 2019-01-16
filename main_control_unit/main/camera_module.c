#include "drone.h"
#include "bitmap.h"
#include <errno.h>

static const char* TAG = "camera_module";

void camera_module_init(){
    camera_config_t camera_config = {
        .ledc_channel = LEDC_CHANNEL_0,
        .ledc_timer = LEDC_TIMER_0,
        .pin_d0 = CAMERA_D0,
        .pin_d1 = CAMERA_D1,
        .pin_d2 = CAMERA_D2,
        .pin_d3 = CAMERA_D3,
        .pin_d4 = CAMERA_D4,
        .pin_d5 = CAMERA_D5,
        .pin_d6 = CAMERA_D6,
        .pin_d7 = CAMERA_D7,
        .pin_xclk = CAMERA_XCLK,
        .pin_pclk = CAMERA_PCLK,
        .pin_vsync = CAMERA_VSYNC,
        .pin_href = CAMERA_HREF,
        .pin_sscb_sda = CAMERA_SDA,
        .pin_sscb_scl = CAMERA_SCL,
        .pin_reset = CAMERA_RESET,
        .xclk_freq_hz = CAMERA_XCLK_FREQ,
        .frame_size = CAMERA_FRAME_SIZE,
        .pixel_format = CAMERA_PIXEL_FORMAT,
    };

    camera_model_t camera_model;

    if(camera_probe(&camera_config, &camera_model) != ESP_OK){
        ESP_LOGE("camera_module_init::", "Camera probe failed");
        return;
    }

    if(camera_init(&camera_config) != ESP_OK){
        ESP_LOGE("camera_module_init::", "Could not successfully initialize module");
    }
    ESP_LOGI("camera_module_init::", "Initialized camera");
}

static void camera_module_task(void* args){
    
    int server_socket;
    int client_socket;

    struct sockaddr_in server_address;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(CAMERA_SOCKET_PORT);

    if((server_socket = socket(server_address.sin_family, SOCK_STREAM, 0)) < 0){
        ESP_LOGE(TAG, "Could not create camera module socket");
        vTaskDelete(NULL);
    }

    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    listen(server_socket, 0);

    ESP_LOGI(TAG, "Client socket created");

    while(true){

        ESP_LOGI(TAG, "Camera socket waiting for new client connection");
        if((client_socket = accept(server_socket, (struct sockaddr*) NULL, NULL)) < 0){
            ESP_LOGE(TAG, "Error while waiting for new connection to socket");
            continue;
        } 
        
        ESP_LOGI(TAG, "Client connected to socket. Start image data stream");
        while(xEventGroupGetBits(wifi_event_group) & CLIENT_CONNECTED){
            if(camera_run() != ESP_OK){
                ESP_LOGE(TAG,"Capture failed");
                continue;
            }
            
            if(write(client_socket, camera_get_fb(), camera_get_data_size()) < 0){
                ESP_LOGE(TAG, "Socket writing returned with an error");
                break;
            }

        }
    }


    vTaskDelete(NULL);
}

void camera_module_start(){
    xTaskCreate(camera_module_task, "Camera Module Task", 2048, NULL, 8, NULL);
}
#include "drone.h"
#include "bitmap.h"
#include <errno.h>


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
    server_address.sin_port = htons(4343);

    if((server_socket = socket(server_address.sin_family, SOCK_STREAM, 0)) < 0){
        ESP_LOGE("camera_module_task::", "Could not create camera module socket");
        vTaskDelete(NULL);
    }

    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    //bitmap_header_t* header = bmp_create_header(camera_get_fb_width(), camera_get_fb_height());
    char* pgm_header;
    asprintf(&pgm_header, "P5 %d %d %d\n", camera_get_fb_width(), camera_get_fb_height(), 255);

    listen(server_socket, 0);

    while(true){

        ESP_LOGI("camera_module_task::", "Camera socket waiting for new client connection");
        client_socket = accept(server_socket, (struct sockaddr*) NULL, NULL); 
        ESP_LOGI("camera_module_task::", "New client connected to socket. Sending bitmap header: %s ", pgm_header);
        write(client_socket, pgm_header, strlen(pgm_header));
        ESP_LOGI("camera_module_task::", "Start streaming bitmap data section");
        while(xEventGroupGetBits(wifi_event_group) & CLIENT_CONNECTED){
            if(camera_run() != ESP_OK){
                ESP_LOGE("camera_module_task::","Capture failed");
                continue;
            }
            write(client_socket, camera_get_fb(), camera_get_data_size());

            //vTaskDelay(port_delay_ms(10));
            //ESP_LOGI("Sent::", "%d", camera_get_data_size());
        }
    }


    free(pgm_header);
    vTaskDelete(NULL);
}

void camera_module_start_sending(){
    xTaskCreate(camera_module_task, "Camera Module Task", 2048, NULL, 5, NULL);
}
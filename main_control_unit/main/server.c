#include "drone.h"

static void server_task()
{
    struct sockaddr_in host_addr;
    int server_socket_desc;
    int err = 0;
    char rx_buffer[2];    

    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(SERVER_PORT);

    server_socket_desc = socket(host_addr.sin_family, SOCK_DGRAM, IPPROTO_IP);
    err = bind(server_socket_desc, (struct sockaddr*) &host_addr, sizeof(host_addr));

    if(err < 0)
    {
        ESP_LOGE("server_task()::", "Unable to bind socket");
        vTaskDelete(NULL);
    }

    struct sockaddr_in client_addr;
    socklen_t client_socket_len = sizeof(client_addr);
    
    struct MotorData motor_data;

    while(1)
    {

        int len = recvfrom(server_socket_desc, rx_buffer, sizeof(rx_buffer), 0, (struct sockaddr *) &client_addr, &client_socket_len);
        if(len < 0)
        {
            ESP_LOGE("server_task()::", "Error while trying to receive data");
            break;
        }
        else
        {
            if(len == 2)
            {
                ESP_LOGI("server_task()::", "Received data");
                uint motor_a_direction = (rx_buffer[0] & (1 << 7)) >> 7;
                int motor_a_speed_percent = (rx_buffer[0] & ~(1 << 7));
                uint motor_b_direction = (rx_buffer[1] & (1 << 7)) >> 7;
                int motor_b_speed_percent = (rx_buffer[1] & ~(1 << 7));

                motor_data.motor_a_speed_percent = motor_a_speed_percent;
                motor_data.motor_a_direction = motor_a_direction;
                motor_data.motor_b_speed_percent = motor_b_speed_percent;
                motor_data.motor_b_direction = motor_b_direction;

                xQueueSend(udp_to_i2c_com_queue, &motor_data, 0);
            }
        }

        vTaskDelay(port_delay_ms(25));
    }

    if(server_socket_desc != -1)
    {
        ESP_LOGI("server_task()::", "Shutting down socket");
        shutdown(server_socket_desc, 0);
        close(server_socket_desc);
    }
    esp_wifi_deauth_sta(0);
    vTaskDelete(NULL);
}

void server_start()
{
    xTaskCreate(server_task, "server", 4096, NULL, 5, NULL);
}

void server_stop()
{

}
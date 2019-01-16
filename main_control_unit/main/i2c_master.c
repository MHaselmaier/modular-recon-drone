#include "drone.h"

static const char* TAG = "i2c_master";

void i2c_master_init()
{
    ESP_LOGI(TAG, "Initializing i2c master bus using ports [sda:%d, scl:%d]", I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    i2c_config_t config;
    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = I2C_MASTER_SDA_IO;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_io_num = I2C_MASTER_SCL_IO;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_param_config(I2C_NUM_0, &config);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, config.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
}

static void i2c_master_send_data(char address, uint direction, uint speed)
{
    i2c_cmd_handle_t command = i2c_cmd_link_create();
    i2c_master_start(command);
    i2c_master_write_byte(command, I2C_MASTER_WRITE | (address << 1), I2C_ACK);
    i2c_master_write_byte(command, speed | (direction << 7), I2C_ACK);
    i2c_master_stop(command);
    i2c_master_cmd_begin(I2C_NUM_0, command, 25 / portMAX_DELAY);
    i2c_cmd_link_delete(command);
}

static void i2c_master_task(void* p)
{
    ESP_LOGI(TAG, "Start sending incoming data from queue to slave controller");
    struct MotorData motor_data;
    motor_data.motor_a_speed_percent = 0;
    motor_data.motor_a_direction = 0;
    motor_data.motor_b_speed_percent = 0;
    motor_data.motor_b_direction = 0;

    while(1)
    {
        if(xQueueReceive(udp_to_i2c_com_queue, &motor_data, 0))
        {
            ESP_LOGI(TAG, "i2c task read from queue: L[dir:%d,speed:%d], R[dir:%d,speed:%d]", motor_data.motor_a_direction, motor_data.motor_a_speed_percent, motor_data.motor_b_direction, motor_data.motor_b_speed_percent);
        }

        i2c_master_send_data(MOTOR_A, motor_data.motor_a_direction, motor_data.motor_a_speed_percent);
        i2c_master_send_data(MOTOR_B, motor_data.motor_b_direction, motor_data.motor_b_speed_percent);

        vTaskDelay(port_delay_ms(25));
    }

    vTaskDelete(NULL);
}

void i2c_master_bus_start()
{
    ESP_LOGI(TAG, "Creating i2c task");
    xTaskCreate(i2c_master_task, "i2c master", 4096, NULL, 5, NULL);
}
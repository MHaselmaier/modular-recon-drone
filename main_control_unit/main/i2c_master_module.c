#include "drone.h"

static const char* TAG = "i2c_master";

u8g2_t display_handler;
static void init_display(){
    ESP_LOGI(TAG, "Initializing oled display");
    u8g2_esp32_hal_t u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.sda   = I2C_MASTER_SDA_IO;
	u8g2_esp32_hal.scl  = I2C_MASTER_SCL_IO;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

    u8g2_Setup_ssd1306_i2c_128x32_univision_f(
	&display_handler,
	U8G2_R0,
	u8g2_esp32_i2c_byte_cb,
	u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
	u8x8_SetI2CAddress(&display_handler.u8x8,0x78);
    u8g2_InitDisplay(&display_handler); 
    ESP_LOGI(TAG, "Waking up display");
    u8g2_SetPowerSave(&display_handler, 0); // wake up display
}

void display_set_text(uint line, const char* desc, const char* text){
    if(xEventGroupGetBits(system_event_group) & I2C_INITIALIZED){
        ESP_LOGI(TAG, "Clearing display buffer");
        u8g2_SetFont(&display_handler, u8g2_font_t0_11_tf);
        u8g2_DrawStr(&display_handler, 2, line * 10, desc);
        if(text)
            u8g2_DrawStr(&display_handler, 50, line * 10, text);
    }
}

void display_draw_hskl(){
    if(xEventGroupGetBits(system_event_group) & I2C_INITIALIZED){
        u8g2_SetFont(&display_handler, u8g2_font_profont22_tf);
        u8g2_DrawStr(&display_handler, 40, 15, "HSKL");
        u8g2_SetFont(&display_handler, u8g2_font_5x7_tf);
        u8g2_DrawStr(&display_handler, 5, 22, "University of");
        u8g2_DrawStr(&display_handler, 10, 30, "Applied Sciences");
    }
}

void display_update(){
    if(xEventGroupGetBits(system_event_group) & I2C_INITIALIZED)
        u8g2_SendBuffer(&display_handler);
}

void display_clear(){
    if(xEventGroupGetBits(system_event_group) & I2C_INITIALIZED)
        u8g2_ClearBuffer(&display_handler);
}

void i2c_master_init()
{
    ESP_LOGI(TAG, "Initializing i2c master bus using ports [sda:%d, scl:%d]", I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO);
    i2c_config_t i2c_bus_config;
    i2c_bus_config.mode = I2C_MODE_MASTER;
    i2c_bus_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_bus_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_bus_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_bus_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_bus_config.master.clk_speed = I2C_MASTER_FREQ_HZ;

    i2c_param_config(I2C_MASTER_NUM, &i2c_bus_config);
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, i2c_bus_config.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0));
    init_display();
    xEventGroupSetBits(system_event_group, I2C_INITIALIZED);
}

static void i2c_master_send_data(char address, uint direction, uint speed)
{
    if(xEventGroupGetBits(system_event_group) & I2C_INITIALIZED){
        i2c_cmd_handle_t command = i2c_cmd_link_create();
        i2c_master_start(command);
        i2c_master_write_byte(command, I2C_MASTER_WRITE | (address << 1), I2C_ACK);
        i2c_master_write_byte(command, speed | (direction << 7), I2C_ACK);
        i2c_master_stop(command);
        i2c_master_cmd_begin(I2C_MASTER_NUM, command, 25 / portMAX_DELAY);
        i2c_cmd_link_delete(command);
    }
}

static void i2c_master_motor_task(void* p)
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
    xTaskCreate(i2c_master_motor_task, "i2c master motor task", 2048, NULL, 5, NULL);
}
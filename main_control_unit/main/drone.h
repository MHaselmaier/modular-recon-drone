#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

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

#include "camera.h"

#include "u8g2.h"
#include "u8g2_esp32_hal.h"

//---- Event Groups -----
#define CLIENT_CONNECTED BIT0
#define I2C_INITIALIZED BIT1

extern EventGroupHandle_t system_event_group;

void event_group_init();

//---- GPIO -------------
#define RED 32
#define GREEN 33
#define BLUE 25

void led_init();
void led_enable(int led);
void led_disable();

//---- Queues -----------
extern QueueHandle_t udp_to_i2c_com_queue;

struct MotorData
{
    char motor_a_speed_percent;
    char motor_a_direction;
    char motor_b_speed_percent;
    char motor_b_direction;
};

// --- Camera Module ----
#define CAMERA_SOCKET_PORT 1234

#define CAMERA_PIXEL_FORMAT CAMERA_PF_GRAYSCALE
#define CAMERA_FRAME_SIZE CAMERA_FS_QQVGA
#define CAMERA_XCLK_FREQ 20000000
#define CAMERA_D0 35
#define CAMERA_D1 17
#define CAMERA_D2 34
#define CAMERA_D3 5
#define CAMERA_D4 39
#define CAMERA_D5 18
#define CAMERA_D6 36
#define CAMERA_D7 19
#define CAMERA_XCLK 27
#define CAMERA_PCLK 21
#define CAMERA_VSYNC 22
#define CAMERA_HREF 26
#define CAMERA_SDA 25
#define CAMERA_SCL 23
#define CAMERA_RESET 15

void camera_module_init();
void camera_module_start();

//---- I2C --------------
#define I2C_NO_ACK 0x0
#define I2C_ACK 0x1
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SCL_IO 33               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 32 
#define I2C_MASTER_FREQ_HZ 100000           /* frequency of the i2c bus*/
#define I2C_MASTER_TX_BUF_DISABLE 0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0  

#define FORWARD 1
#define REVERSE 0

#define MAX_SPEED_VALUE 127

#define MOTOR_A 0x01
#define MOTOR_B 0x02

void i2c_master_init();
void i2c_master_bus_start();
void display_set_text(uint line, const char* desc, const char* text);
void display_clear();

//---- Accesspoint ------
#define SSID "Recon Drone"

void accesspoint_init(const char* ssid, const char* password);
void accesspoint_start();
void accesspoint_stop();

//---- UDP Server -------
#define SERVER_PORT 4242

void server_start();

//---- Utility functions ------
TickType_t port_delay_ms(uint32_t ms);
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

//---- GPIO -------------
#define RED 32
#define GREEN 33
#define BLUE 25

void led_init();
void led_enable(int led);
void led_disable();

//---- Queues -----------
QueueHandle_t udp_to_i2c_com_queue;

struct MotorData
{
    char motor_a_speed_percent;
    char motor_a_direction;
    char motor_b_speed_percent;
    char motor_b_direction;
};

// --- Motor Control - PWM ----
#define MOTOR_A_PWM 32               /*!< gpio number for I2C master clock */
#define MOTOR_B_PWM 33
#define MOTOR_A_LEFT 25
#define MOTOR_A_RIGHT 26
#define MOTOR_B_LEFT 27
#define MOTOR_B_RIGHT 14

void motor_control_init();
void motor_control_start();

//---- I2C --------------
#define I2C_NO_ACK 0x0
#define I2C_ACK 0x1
#define I2C_MASTER_SCL_IO 33               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 32
#define I2C_MASTER_FREQ_HZ 100000        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0  

#define FORWARD 1
#define REVERSE 0

#define MAX_SPEED_VALUE 127

#define MOTOR_A 0x01
#define MOTOR_B 0x02

void i2c_master_init();
void i2c_master_bus_start();

//---- Accesspoint ------
void accesspoint_init(const char* ssid, const char* password);
void accesspoint_start();
void accesspoint_stop();

//---- UDP Server -------
void server_start();
void server_stop();

//---- Utility functions ------
TickType_t port_delay_ms(uint32_t ms);
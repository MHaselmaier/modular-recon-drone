#include "drone.h"

#include "esp_attr.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "driver/gpio.h"

void motor_control_init(){

    gpio_pad_select_gpio(MOTOR_A_LEFT);
    gpio_set_direction(MOTOR_A_LEFT, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(MOTOR_A_RIGHT);
    gpio_set_direction(MOTOR_A_RIGHT, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(MOTOR_B_LEFT);
    gpio_set_direction(MOTOR_B_LEFT, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(MOTOR_B_RIGHT);
    gpio_set_direction(MOTOR_B_RIGHT, GPIO_MODE_OUTPUT);

    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR_A_PWM);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR_B_PWM);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
}

static void motor_control_task(void* arg){
    
    
    struct MotorData motor_data;
    motor_data.motor_a_speed_percent = 0;
    motor_data.motor_a_direction = 0;
    motor_data.motor_b_speed_percent = 0;
    motor_data.motor_b_direction = 0;

    while(1)
    {
        if(xQueueReceive(udp_to_i2c_com_queue, &motor_data, 0)){
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, motor_data.motor_a_speed_percent);
            mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, motor_data.motor_b_speed_percent);

            gpio_set_level(MOTOR_A_LEFT, motor_data.motor_a_direction);
            gpio_set_level(MOTOR_A_RIGHT, !motor_data.motor_a_direction);
            gpio_set_level(MOTOR_B_LEFT, motor_data.motor_b_direction);
            gpio_set_level(MOTOR_B_RIGHT, !motor_data.motor_b_direction);
        }

        vTaskDelay(port_delay_ms(10));
    }
}

void motor_control_start(){
    xTaskCreate(motor_control_task, "Motor Control Task", 2048, NULL, 5, NULL);
}
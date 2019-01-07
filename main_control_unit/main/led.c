#include "drone.h"

void led_init()
{
    gpio_pad_select_gpio(RED);
    gpio_pad_select_gpio(GREEN);
    gpio_pad_select_gpio(BLUE);

    gpio_set_direction(RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLUE, GPIO_MODE_OUTPUT);
}

void led_enable(int led)
{
    if(led == RED)
    {
        gpio_set_level(RED, 1);
        gpio_set_level(GREEN, 0);
        gpio_set_level(BLUE, 0);
    }else if(led == GREEN)
    {
        gpio_set_level(RED, 0);
        gpio_set_level(GREEN, 1);
        gpio_set_level(BLUE, 0);
    }else if(led == BLUE)
    {
        gpio_set_level(RED, 0);
        gpio_set_level(GREEN, 0);
        gpio_set_level(BLUE, 1);
    }
}

void led_disable()
{
    gpio_set_level(RED, 0);
    gpio_set_level(GREEN, 0);
    gpio_set_level(BLUE, 0);
}
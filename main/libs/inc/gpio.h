#ifndef __GPIO_H__
#define __GPIO_H__

#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"
#include <freertos/queue.h>
#include "esp_log.h"

#define VALVE               GPIO_NUM_12
#define BUZZ                GPIO_NUM_16
#define SIM800_PWKEY        GPIO_NUM_5
#define SIM800_POWER        GPIO_NUM_23

#define RED_LED             GPIO_NUM_13
#define GREEN_LED           GPIO_NUM_15


void gpio_init(void);

void buzzer_on(void);
void buzzer_off(void);

void green_led_on(void);
void green_led_off(void);

void red_led_on(void);
void red_led_off(void);

void valve_control(void);

void sim800_turnon(void);
void sim800_turnoff(void);




#endif /* __GPIO_H__ */
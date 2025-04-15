#include "gpio.h"

#define set_sim800_pwkey() gpio_set_level(SIM800_PWKEY,1)
#define clear_sim800_pwkey() gpio_set_level(SIM800_PWKEY,0)

#define set_sim800_pwrsrc() gpio_set_level(SIM800_POWER,1)
#define clear_sim800_pwrsrc() gpio_set_level(SIM800_POWER,0)

static const char *TAG = "GPIO";

void sim800_turnon(void)
{
    

    clear_sim800_pwrsrc();
    vTaskDelay(100);
    set_sim800_pwkey();
    vTaskDelay(pdMS_TO_TICKS(1000));
    set_sim800_pwrsrc();

    ESP_LOGI(TAG, "Initialization...");
    vTaskDelay(pdMS_TO_TICKS(1000));

    clear_sim800_pwkey();
    vTaskDelay(pdMS_TO_TICKS(1000));
    set_sim800_pwkey();
}

void sim800_turnoff(void){
    clear_sim800_pwkey();
    vTaskDelay(pdMS_TO_TICKS(100));
    clear_sim800_pwrsrc();
}

void gpio_init(void){
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1<<SIM800_PWKEY)+(1<<SIM800_POWER)+(1<<VALVE)+(1<<RED_LED)+(1<<GREEN_LED)+(1<<BUZZ);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.intr_type  = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
}

void buzzer_on(void){
    gpio_set_level(BUZZ, 1);
}


void buzzer_off(void){
    gpio_set_level(BUZZ, 0);
}


void valve_control(void){
    gpio_set_level(VALVE, 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(VALVE, 0);
}

void green_led_on(void){
    gpio_set_level(GREEN_LED,1);
}
void green_led_off(void){
    gpio_set_level(GREEN_LED,0);
}

void red_led_on(void){
    gpio_set_level(RED_LED,1);
}
void red_led_off(void){
    gpio_set_level(RED_LED,0);
}
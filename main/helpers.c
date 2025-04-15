#include "iot_button.h"
#include "esp_log.h"
#include "main.h"
static const char *TAG = "TEST_BUTTON";

SSD1306_t dev;
QueueHandle_t gas_queue_clone;
TaskHandle_t gasTaskHandle_clone;

void oled_display_init(void){
    i2c_master_init(&dev, GPIO_NUM_18, GPIO_NUM_19, (-1));
    ssd1306_init(&dev, 128, 64);
    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
    ssd1306_display_text(&dev, 0, " STARTING...", 13, false);
    ESP_ERROR_CHECK(i2cdev_init());
}

void clear_display(void){
    ssd1306_clear_screen(&dev, false);
}


void write_text(char* buffer,float value, ads111x_mux_t mux, int page){
    switch (mux)

    {
    case NATURAL_GAS:
        sprintf(buffer," CH4: :%.02f",value);
        break;
    
    case CO_GAS:
        sprintf(buffer," CO: :%.02f",value);
        break;

    case BATTERY_VALUE:
        sprintf(buffer," Battery :%.02f",value);
        break;

    case BUS_VALUE:
        sprintf(buffer," Bus :%.02f",value);
        break;

    default:
        break;
    }
    ssd1306_display_text(&dev, page, buffer, strlen(buffer), false);
}

void write_info(char* buffer){
    ssd1306_display_text(&dev, 1, buffer, strlen(buffer), false);
}


static button_handle_t g_btns[1] = {0};

static int get_btn_index(button_handle_t btn)
{
    for (size_t i = 0; i < 1; i++) {
        if (btn == g_btns[i]) {
            return i;
        }
    }
    return -1;
}

static void button_long_press_hold_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "BTN%d: BUTTON_LONG_PRESS_HOLD[%d],count is [%d]", get_btn_index((button_handle_t)arg), iot_button_get_ticks_time((button_handle_t)arg), iot_button_get_long_press_hold_cnt((button_handle_t)arg));
    // gas_t main_obj = {0};
    // main_obj.reason = SETTINGS;
    // xQueueSend(gas_queue_clone, &main_obj, portMAX_DELAY);
}

static void button_single_click_cb(void *arg, void *data)
{
    ESP_LOGI(TAG, "BTN%d: BUTTON_SINGLE_CLICK", get_btn_index((button_handle_t)arg));
    // gas_t main_obj = {0};
    // main_obj.reason = TEST_BUTTON;
    // xQueueSend(gas_queue_clone, &main_obj, portMAX_DELAY);
}

static esp_err_t custom_button_gpio_init(void *param)
{
    button_gpio_config_t *cfg = (button_gpio_config_t *)param;

    return button_gpio_init(cfg);
}

static uint8_t custom_button_gpio_get_key_value(void *param)
{
    button_gpio_config_t *cfg = (button_gpio_config_t *)param;

    return button_gpio_get_key_level((void *)cfg->gpio_num);
}

static esp_err_t custom_button_gpio_deinit(void *param)
{
    button_gpio_config_t *cfg = (button_gpio_config_t *)param;

    return button_gpio_deinit(cfg->gpio_num);
}


void button_init(QueueHandle_t queueClone, TaskHandle_t taskHandleClone){
    gas_queue_clone = queueClone;
    gasTaskHandle_clone = taskHandleClone;
    button_gpio_config_t *gpio_cfg = calloc(1, sizeof(button_gpio_config_t));
    gpio_cfg->active_level = 0;
    gpio_cfg->gpio_num = 35;

    button_config_t cfg = {
        .type = BUTTON_TYPE_CUSTOM,
        .long_press_time = 3000,
        .short_press_time = CONFIG_BUTTON_SHORT_PRESS_TIME_MS,
        .custom_button_config = {
            .button_custom_init = custom_button_gpio_init,
            .button_custom_deinit = custom_button_gpio_deinit,
            .button_custom_get_key_value = custom_button_gpio_get_key_value,
            .active_level = 0,
            .priv = gpio_cfg,
        },
    };
    g_btns[0] = iot_button_create(&cfg);
    // TEST_ASSERT_NOT_NULL(g_btns[0]);
    iot_button_register_cb(g_btns[0], BUTTON_LONG_PRESS_HOLD, button_long_press_hold_cb, NULL);
    iot_button_register_cb(g_btns[0], BUTTON_SINGLE_CLICK, button_single_click_cb, NULL);

}

void device_ready(void){
    buzzer_on();
    vTaskDelay(pdMS_TO_TICKS(300));
    buzzer_off();
    vTaskDelay(pdMS_TO_TICKS(500));
    buzzer_on();
    vTaskDelay(pdMS_TO_TICKS(300));
    buzzer_off();
}


void gas_checker(gas_t * obj,ads111x_mux_t mux){
    switch (mux)
    {
    case NATURAL_GAS:
        /* code */
        if (obj->ch4_value > CH4_CRITICAL && obj->ch4_value < 7.0)
        {
            valve_control();
            buzzer_on();
            red_led_on();
            obj->notificate = true;
            obj->reason = NATURAL_GAS_DETECTED;
            ESP_LOGI("ALARM","Natural gas detected");
        }else{
            buzzer_off();
            red_led_off(); 
        }
        break;
    case CO_GAS:
        /* code */
        if (obj->co_value > CO_CRITICAL && obj->co_value < 7.0)
        {
            valve_control();
            buzzer_on();
            red_led_on();
            obj->notificate = true;
            obj->reason = CO_GAS_DETECTED;
            ESP_LOGI("ALARM","CO gas detected");
        }else{
            buzzer_off();
            red_led_off(); 
        }
        break;
    default:
        break;
    }
}

void adc_module_init(i2c_dev_t *dev, float *gain, i2c_port_t port) {
    // Setup ICs
    memset(dev, 0, sizeof(*dev));
    *gain = ads111x_gain_values[ADS111X_GAIN_6V144];
    ESP_ERROR_CHECK(ads111x_init_desc(dev, ADS111X_ADDR_GND, port, SDA_ADS, SCL_ADS));
    ESP_ERROR_CHECK(ads111x_set_mode(dev, ADS111X_MODE_SINGLE_SHOT));    // Single shot mode
    ESP_ERROR_CHECK(ads111x_set_data_rate(dev, ADS111X_DATA_RATE_16)); // 32 samples per second
    ESP_ERROR_CHECK(ads111x_set_gain(dev, ADS111X_GAIN_6V144));
}


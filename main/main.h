#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "esp_http_server.h"
#include "ssd1306.h"
#include <ads111x.h>

#include "gpio.h"
#include "cJSON.h"

#define SSID                       "kartech"
#define PASSWORD                    "12345678"
#define WIFI_CHANNEL                1
#define WIFI_AP_SSID_HIDDEN         0
#define WIFI_AP_MAX_CONNECTIONS     4
#define WIFI_AP_BEACON_INTERVAL     100
#define WIFI_AP_IP					"192.168.0.1"		// AP default IP
#define WIFI_AP_GATEWAY				"192.168.0.1"		// AP default Gateway (should be the same as the IP)
#define WIFI_AP_NETMASK				"255.255.255.0"		// AP netmask
#define WIFI_AP_BANDWIDTH			WIFI_BW_HT20		// AP bandwidth 20 MHz (40 MHz is the other option)
#define WIFI_STA_POWER_SAVE			WIFI_PS_NONE		// Power save not used
#define MAX_SSID_LENGTH				32					// IEEE standard maximum
#define MAX_PASSWORD_LENGTH			64					// IEEE standard maximum
#define MAX_CONNECTION_RETRIES		5					// Retry number on disconnect


#define APN                 "internet.beeline.uz"
#define GSM                 13

#define CH4_CRITICAL        4.0
#define CO_CRITICAL         4.0


#define SDA_ADS             21
#define SCL_ADS             22

#define SMS_C4H            "GAZ IYISI(METAN yaki PROPAN) SEZILDI!"
#define SMS_C0             "GAZ IYISI (CO) SEZILDI!"
#define CONTACT            "+998901289515"


typedef enum
{
	NATURAL_GAS_DETECTED = 2,
    CO_GAS_DETECTED,
    TEST_BUTTON,
	POWER_SUPPLY_STATE_CHANGED,
    GAS_DETECTED_POWER_SUPPLY_STATE_CHANGED,
    SETTINGS,
    SETTINGS_SERVER,
} reason_t;


typedef struct{
    float ch4_value;
    float co_value;
    float v_battery;
    float v_external;
    uint8_t reason;
    bool notificate;
} gas_t;


void oled_display_init(void);
void clear_display(void);
void write_text(char* buffer,float value, ads111x_mux_t mux, int page);
void write_info(char* buffer);
void button_init(QueueHandle_t queueClone, TaskHandle_t taskHandleClone);
void device_ready(void);
void gas_checker(gas_t * obj,ads111x_mux_t mux);
void adc_module_init(i2c_dev_t *dev, float *gain, i2c_port_t port);
void settings_init(QueueHandle_t queueClone);
void http_server_start(void);
httpd_handle_t http_server_configure(void);

#endif
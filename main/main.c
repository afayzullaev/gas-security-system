#include "main.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "mqtt_client.h"
#include "esp_modem_api.h"
#include "sdkconfig.h"


#define I2C_PORT 0

static const char *TAG = "GAS_DETECTOR";

esp_modem_dce_t *dce = NULL;
esp_netif_t *esp_netif = NULL;

QueueHandle_t gas_queue;
TaskHandle_t gasTaskHandle;

static void on_ppp_changed(void *arg, esp_event_base_t event_base,
                           int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "PPP state changed event %" PRIu32, event_id);
    if (event_id == NETIF_PPP_ERRORUSER) {
        /* User interrupted event from esp-netif */
        esp_netif_t **p_netif = event_data;
        ESP_LOGI(TAG, "User interrupted event from netif:%p", *p_netif);
    }
}

static void on_ip_event(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "IP event! %" PRIu32, event_id);
    if (event_id == IP_EVENT_PPP_GOT_IP) {
        esp_netif_dns_info_t dns_info;

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        esp_netif_t *netif = event->esp_netif;

        ESP_LOGI(TAG, "Modem Connect to PPP Server");
        ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
        ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
        ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
        esp_netif_get_dns_info(netif, 0, &dns_info);
        ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        esp_netif_get_dns_info(netif, 1, &dns_info);
        ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        ESP_LOGI(TAG, "~~~~~~~~~~~~~~");

        ESP_LOGI(TAG, "GOT ip event!!!");
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
    } else if (event_id == IP_EVENT_GOT_IP6) {
        ESP_LOGI(TAG, "GOT IPv6 event!");

        ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
    }
}

void send_sms(char* sms){
    buzzer_on();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, NULL));

    esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG(APN);
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();
    esp_netif = esp_netif_new(&netif_ppp_config);
    assert(esp_netif);

    esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();

    dte_config.uart_config.tx_io_num = GPIO_NUM_27;
    dte_config.uart_config.rx_io_num = GPIO_NUM_26;
    dte_config.uart_config.rts_io_num = (-1);
    dte_config.uart_config.cts_io_num = (-1);
    dte_config.uart_config.flow_control = ESP_MODEM_FLOW_CONTROL_NONE;
    dte_config.uart_config.rx_buffer_size = 1024;
    dte_config.uart_config.tx_buffer_size = 512;
    dte_config.uart_config.event_queue_size = 30;
    dte_config.task_stack_size = 2048;
    dte_config.task_priority = 5;
    dte_config.dte_buffer_size = 1024 / 2;

    ESP_LOGI(TAG, "Initializing esp_modem for the SIM800 module...");
    dce = esp_modem_new_dev(ESP_MODEM_DCE_SIM800, &dte_config, &dce_config, esp_netif);
    assert(dce);
    int rssi, ber;
    esp_modem_sync(dce);
    esp_err_t err = esp_modem_get_signal_quality(dce, &rssi, &ber);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_modem_get_signal_quality failed with %d %s", err, esp_err_to_name(err));
    }
    int count = 0;
    
    while (1)
    {
        err = esp_modem_get_signal_quality(dce, &rssi, &ber);
        ESP_LOGI(TAG, "Signal quality: rssi=%d, ber=%d", rssi, ber);
        vTaskDelay(pdMS_TO_TICKS(1000));
        if(rssi > 4 && rssi <= 31){
            break;
        }else{
            count++;
            if(count > 100) break;
        }

    }
    char at_data[16] = {0};
    esp_modem_at(dce,"AT+CGSN ", at_data, portMAX_DELAY);
	ESP_LOGI(TAG, "IMEI %s", at_data);

    if (esp_modem_sms_txt_mode(dce, true) != ESP_OK || esp_modem_sms_character_set(dce) != ESP_OK) {
        ESP_LOGE(TAG, "Setting text mode or GSM character set failed");
        for (size_t i = 0; i < 10; i++)
        {
            /* code */
            if (esp_modem_sms_txt_mode(dce, true) != ESP_OK || esp_modem_sms_character_set(dce) != ESP_OK){
                ESP_LOGE(TAG, "Setting text mode or GSM character set failed");
            }else{
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(500));
        } 
    }
    
    err = esp_modem_send_sms(dce, CONTACT, "GAZ IYISI SEZILDI! KLAPAN JAWILDI");
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_modem_send_sms() failed with %d", err);
        for(int i = 0; i < 5; i++){
            err = esp_modem_send_sms(dce, CONTACT, "GAZ IYISI SEZILDI! KLAPAN JAWILDI");
            if(err != ESP_OK){
                ESP_LOGE(TAG, "esp_modem_send_sms() failed with %d", err);
            }else{
                ESP_LOGI(TAG, "SMS is SENT");
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(30000));
        vTaskResume(gasTaskHandle);
    }else{
        ESP_LOGI(TAG, "SMS is SENT");
        vTaskDelay(pdMS_TO_TICKS(30000));
        vTaskResume(gasTaskHandle);
    }   
}


// Notification task
void dispatcher_task(void *pvParameters)
{
    ESP_LOGI(TAG, "SMS APP");
    gas_t main_obj = {0};

    while (true)
    {
        if (xQueueReceive(gas_queue, &main_obj, portMAX_DELAY))
        {
            ESP_LOGI("SMS_TASK", "REASON %d", main_obj.reason);
            switch (main_obj.reason)
            {
            case NATURAL_GAS_DETECTED:
                ESP_LOGI(TAG, "NATURAL_GAS_DETECTED"); 
                send_sms(SMS_C4H);
                if( dce != NULL){
                    esp_modem_destroy(dce);
                }
                if( esp_netif != NULL){
                    esp_netif_destroy(esp_netif);
                }
                ESP_ERROR_CHECK(esp_event_loop_delete_default());
                break;
            
            case CO_GAS_DETECTED:
                ESP_LOGI(TAG, "CO_GAS_DETECTED"); 
                send_sms(SMS_C0);
                if( dce != NULL){
                    esp_modem_destroy(dce);
                }
                if( esp_netif != NULL){
                    esp_netif_destroy(esp_netif);
                }
                ESP_ERROR_CHECK(esp_event_loop_delete_default());
                break;

            case TEST_BUTTON:
                ESP_LOGI(TAG, "TEST_BUTTON");
                // valve_control();
                // vTaskDelay(pdMS_TO_TICKS(1000));
                break;

            case SETTINGS:
                ESP_LOGI(TAG, "SETTINGS");
                // vTaskSuspend(gasTaskHandle);
                // settings_init(gas_queue);
                break;

            case SETTINGS_SERVER:
                ESP_LOGI(TAG, "SETTINGS_SERVER");
                break;

            default:
                break;
            }
        }
    }
    
}



// ADC task
void ads111x_task(void *pvParameters)
{    
    ESP_LOGI(TAG, "GAS ANALYSE TASK");
    gas_queue = xQueueCreate(1, sizeof(gas_t));
    button_init(gas_queue, gasTaskHandle);
    
    float gain_val;// Gain value
    i2c_dev_t device;// Descriptor
    adc_module_init(&device, &gain_val, I2C_PORT);   
    //Calibrating
    vTaskDelay(pdMS_TO_TICKS(250)); 
    gas_t main_obj = {0};

    main_obj.ch4_value = get_gas_value(&device, NATURAL_GAS, gain_val, 30);
    device_ready();

    vTaskDelay(pdMS_TO_TICKS(500));

    while (1)
    {
        // Natural gas value
        main_obj.ch4_value = get_gas_value(&device, NATURAL_GAS, gain_val, 10);
        gas_checker(&main_obj, NATURAL_GAS);

        // CO value
        main_obj.co_value = get_gas_value(&device, CO_GAS, gain_val, 5);
        gas_checker(&main_obj, CO_GAS);  

        if (main_obj.notificate)
        {
            main_obj.notificate = false;
            ESP_LOGI("NOTIFICATE", "........................................");
            xQueueSend(gas_queue, &main_obj, portMAX_DELAY);
            vTaskSuspend(gasTaskHandle);
        }
        
    }
}



void app_main()
{
    ESP_LOGI(TAG, "BEGINNING APP");
    gpio_init();
    
    
    oled_display_init();
    green_led_on();
    buzzer_off();
    sim800_turnon();
    vTaskDelay(pdMS_TO_TICKS(1000));   

    // Start task
    ESP_LOGI(TAG, "Task creating");
    xTaskCreate(ads111x_task, "ads111x_task", 4096, NULL, 5, &gasTaskHandle);


    xTaskCreate(dispatcher_task, "dispatcher_task", 9192, NULL, 6, NULL);
    
    vTaskDelete(NULL);
}

/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "Config.h"


#define RELAY1 GPIO_NUM_0 //0
#define RELAY2 GPIO_NUM_1
#define RELAY3 GPIO_NUM_3
#define RELAY4 GPIO_NUM_19

static const char *TAG_RELAY = "RELAY";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.*/

static void set_relay(gpio_num_t gpio_num, uint32_t level)
{
    /* Set the relay level according to the state */
    gpio_set_level(gpio_num, level);
}

static void configure_relay(gpio_num_t pin)
{
    ESP_LOGI(TAG_RELAY, "Configuration relay:%d",pin);
    gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
}


void init_relay()
{
    configure_relay(RELAY1);
    configure_relay(RELAY2);
    configure_relay(RELAY3);
    configure_relay(RELAY4);
}



void parse_messageRelay(char* topic, char*data)
{
    // check wanted state
    int state=0;
    if(strcmp(data,"1")==0)
    {
        state=1;
    }

    // Relay 1 validation
    if(strcmp(topic,TOPIC_RELAY1)==0)
    {
        ESP_LOGI(TAG_RELAY, "SET Relay1 to %d",state);
        gpio_set_level(RELAY1, state);
    }

    // Relay 2 validation
    if(strcmp(topic,TOPIC_RELAY2)==0)
    {
        ESP_LOGI(TAG_RELAY, "SET Relay2 to %d",state);
        gpio_set_level(RELAY2, state);
    }

    // Relay 3 validation
    if(strcmp(topic,TOPIC_RELAY3)==0)
    {
        ESP_LOGI(TAG_RELAY, "SET Relay3 to %d",state);
        gpio_set_level(RELAY3,state);
    }

    // Relay 4 validation
    if(strcmp(topic,TOPIC_RELAY4)==0)
    {
        ESP_LOGI(TAG_RELAY, "SET Relay4 to %d",state);
        gpio_set_level(RELAY4,state);
    }

}

void set_default_value()
{
    gpio_set_level(RELAY1, 0);
    gpio_set_level(RELAY2, 0);
    gpio_set_level(RELAY3, 0);
    gpio_set_level(RELAY4, 0);
}


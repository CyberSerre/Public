
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "driver/gpio.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include <string.h>

#include "CS_Relay.h"
#include "Config.h"





static const char *TAG_MQTT = "MQTT";


static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG_MQTT, "Last error %s: 0x%x", message, error_code);
    }
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    //ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    ESP_LOGD(TAG_MQTT, "Event dispatched from event loop base=%s, event_id=%li", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
   switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_CONNECTED");
        
        //subscribe to relay1 channel;
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_RELAY1, 0);
        ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id=%d", msg_id);
        //subscribe to relay2 channel;
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_RELAY2, 0);
        ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id=%d", msg_id);
        //subscribe to relay3 channel;
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_RELAY3, 0);
        ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id=%d", msg_id);
        //subscribe to relay3 channel;
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_RELAY4 ,0);
        ESP_LOGI(TAG_MQTT, "sent subscribe successful, msg_id=%d", msg_id);   

        break;

    case MQTT_EVENT_DISCONNECTED:
        set_default_value();
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);

        
        int lenght=event->data_len;
        char topic [50];
        char data[50];
        strncpy(topic,event->topic,event->topic_len);
        strncpy(data,event->data,lenght);

        //for an unknow reason, we need to add the null character at the end... 
        //it seem it's not copied ...
        topic[event->topic_len]='\0';
        data[event->data_len]='\0';


        ESP_LOGI(TAG_MQTT, "Topic:%s",topic);
        ESP_LOGI(TAG_MQTT, "data:%s",data);
        ESP_LOGI(TAG_MQTT, "lenght:%d",lenght);

        parse_messageRelay(topic,data);
        
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG_MQTT, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG_MQTT, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG_MQTT, "Other event id:%d", event->event_id);
        break;
    }/* */
}

void mqtt_app_start(char* broker_url)
{
    init_relay();

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = broker_url,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    //The last argument may be used to pass data to the event handler, in this example mqtt_event_handler 
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    
}//*/
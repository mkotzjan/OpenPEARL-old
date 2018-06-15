#include "WifiWrapper.h"

#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"

int connected = 0;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
   switch(event->event_id) {
   case SYSTEM_EVENT_STA_START:
      printf("Esp32Wifi: SYSTEM_EVENT_STA_START");
      break;
   case SYSTEM_EVENT_STA_GOT_IP:
      printf("Esp32Wifi: SYSTEM_EVENT_STA_GOT_IP");
      printf("look, an ip!\n");
      //pearlrt::Log::info("Esp32Wifi: got ip:%s\n",
         //ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
      printf("but I don't know which one :/\n");
      connected = true;
      break;
   case SYSTEM_EVENT_STA_DISCONNECTED:
      printf("Esp32Wifi: SYSTEM_EVENT_STA_DISCONNECTED");
      ESP_ERROR_CHECK(esp_wifi_connect());
      break;
   default:
      break;
   }
   return ESP_OK;
}

void initWiFi(char* ssid, char* password) {
      nvs_flash_init();
      tcpip_adapter_init();
      ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL));
      wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
      
      ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
      ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
      ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
      wifi_config_t sta_config = {
         .sta = {
            .ssid = "test",
            .password = "test1234",
            .bssid_set = false
         }
      };

      ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
      ESP_ERROR_CHECK( esp_wifi_start() );
      ESP_ERROR_CHECK( esp_wifi_connect() );
}

void waitForConnection() {
// Wait for successful conection
      while(!connected) {
         vTaskDelay(1);
      }
}

/*
 [A "BSD license"]
 Copyright (c) 2015      Rainer Mueller
 Copyright (c) 2018      Michael Kotzjan
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Esp32Wifi.h"

extern "C" {
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"
}
#include "Log.h"

char *ssid;
char *password;
bool connected;

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        printf("SYSTEM_EVENT_STA_STARTED\n");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        printf("got ip:%s\n",
                 ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        connected = true;
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        printf("station:"MACSTR" join, AID=%d\n",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        printf("station:"MACSTR"leave, AID=%d\n",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        printf("DISCONNECTED\n");
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

/**
 \brief Implementation of Esp32Wifi Systemdation


*/

namespace pearlrt {

   Esp32Wifi::Esp32Wifi(char* s, char* p) {
      dationStatus = CLOSED;
      connected = false;
      ssid = s;
      password = p;
   }

   SystemDationB* Esp32Wifi::dationOpen(const char* idf,
         int openParam) {
      if (idf != 0) {
         Log::error("IDF not allowed for Esp32Wifi device");
         throw theDationParamSignal;
      }

      if (openParam != IN) {
         Log::error("No open parameters except IN allowed for Esp32Wifi device");
         throw theDationParamSignal;
      }

      if (dationStatus != CLOSED) {
         Log::error("Esp32Wifi: Dation already open");
         throw theOpenFailedSignal;
      }

      //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    //wifi_config_t wifi_config = {
    //    .sta = {
    //        .ssid = ssid,
    //        .password = password
    //        .bssid_set = false
    //    },
    //};
        wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));
	memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
	memcpy(wifi_config.sta.password, password, strlen(password));
wifi_config.sta.bssid_set = 0;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );
    ESP_ERROR_CHECK(esp_wifi_connect());

    printf("wifi_init_sta finished.\n");
   
      // Wait for successful conection
      while(!connected) {
         vTaskDelay(1);
      }

      dationStatus = OPENED;
      return this;
   }

   void Esp32Wifi::dationClose(int closeParam) {

      dationStatus = CLOSED;
   }

   void Esp32Wifi::dationWrite(void* data, size_t size) {
      Log::error("Esp32Wifi: dationWrite not allowed");
      throw theInternalDationSignal;
   }

   void Esp32Wifi::dationRead(void* data, size_t size) {
      Log::error("Esp32Wifi: dationRead not allowed");
      throw theInternalDationSignal;

   }

   int Esp32Wifi::capabilities() {
      return IN;
   }

}




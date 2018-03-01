/*
 [A "BSD license"]
 Copyright (c) 2017 Rainer Mueller
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

#include "chip.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "allTaskPriorities.h"

#include "Lpc17xxUsb.h"
#include "Lpc17xxISRProxy.h"
#include "Log.h"
#include "USB.h"

static CompleteEnumeration_t completeEnumeration = NULL;
static Disconnect_t          disconnectDevice = NULL;
static PollDevice_t pollDevice = NULL;

static void uniCode2String(uint8_t nbrUniCodeChars,
                           uint8_t * src,
                           uint8_t *dest) {
   uint8_t i;

   for (i = 0; i < nbrUniCodeChars; i++) {
      *dest = *src;
      dest++;
      src += 2;
   }

   *dest = 0;
}

extern "C" {
   /* ---------- hook function for the USB stack --------- */
   // the hook functions are called from the usb interrupt handler
   void EVENT_USB_Host_DeviceAttached(const uint8_t corenum) {
      // do nothing - the state machine starts enumeratiopn automatically
      // printf("Lpc17xxUsb: EVENT_USB_Host_DeviceAttached\n");
   }

   void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum) {
      //pearlrt::Log::info("Lpc17xxUsb: Device Unattached");
      // printf("Lpc17xxUsb: Device Unattached\n");
      if (disconnectDevice) {
         (*disconnectDevice)();
      }
   }

   void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum) {
      uint16_t configDescriptorSize;

      // we use this buffer for the config descriptor
      // and the device descriptor
      uint8_t  descriptorData[512];

      uint8_t mi, pi, si;  // Manufacturer, product, serial index
      USB_Descriptor_Device_t * dd; // dd=device-descriptor
      uint8_t ret;


      ret = USB_Host_GetDeviceDescriptor(corenum, descriptorData);

      if (ret  != HOST_GETCONFIG_Successful) {
         pearlrt::Log::error("error retrieving device descriptor");
         return;
      }

      dd = (USB_Descriptor_Device_t*)descriptorData;
      // pearlrt::Log::info("Spec: %x", dd->USBSpecification);  // not helpful
      pearlrt::Log::info("ID(Vendor:Product) %x:%x (Release %x.%x)",
                         dd->VendorID, dd->ProductID,
                         (dd->ReleaseNumber >> 8) & 0x0ff,
                         dd->ReleaseNumber & 0x0ff);

      mi = dd->ManufacturerStrIndex;
      pi = dd->ProductStrIndex;
      si = dd->SerialNumStrIndex;

      //pearlrt::Log::info("index Manufacturer/Product/Serial:%d/%d/%d",
      //  mi, pi, si);  // not helpful

      if (mi > 0) {
         ret = USB_Host_GetDeviceStringDescriptor(corenum,
               mi, descriptorData, 255);
         uniCode2String((descriptorData[0] - 2) / 2,
                        &(descriptorData[2]), &descriptorData[256]);
         pearlrt::Log::info("Manufacturer: '%s'", &(descriptorData[256]));
      }

      if (pi > 0) {
         ret = USB_Host_GetDeviceStringDescriptor(corenum,
               pi, descriptorData, 255);
         uniCode2String((descriptorData[0] - 2) / 2,
                        &(descriptorData[2]), &descriptorData[256]);
         pearlrt::Log::info("Product: '%s'", &(descriptorData[256]));
      }

      if (si > 0) {
         ret = USB_Host_GetDeviceStringDescriptor(corenum,
               si, descriptorData, 255);
         uniCode2String((descriptorData[0] - 2) / 2,
                        &(descriptorData[2]), &descriptorData[256]);
         pearlrt::Log::info("SerialNumber: '%s'", &(descriptorData[256]));
      }


      ret = USB_Host_GetDeviceConfigDescriptor(corenum,
            1, &configDescriptorSize,
            descriptorData, sizeof(descriptorData));

      if (ret  != HOST_GETCONFIG_Successful) {
         pearlrt::Log::error("error retrieving config descriptor");
         return;
      }

      //USB_Descriptor_Configuration_Header_t *cd;  // config descriptor
      //cd = (USB_Descriptor_Configuration_Header_t *)descriptorData;
      // pearlrt::Log::info("Config Descr Type: %x", cd->Header.Type);

      (*completeEnumeration)(corenum, configDescriptorSize, descriptorData);

   }

   void EVENT_USB_Host_HostError(const uint8_t corenum,
                                 const uint8_t errorCode) {
      pearlrt::Log::error("USB-HostError core=%d  errorCode=%d",
                          corenum, errorCode);
   }

   void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
         const uint8_t errorCode, const uint8_t subErrorCode) {
      pearlrt::Log::error("USB-DeviceEnumError core=%d  errorCode=%d sub=%d",
                          corenum, errorCode, subErrorCode);
   }


   static void usbTask(void* pvParameters) {
      TickType_t nextActivation;
      const TickType_t period = 1 / portTICK_PERIOD_MS;

      nextActivation = xTaskGetTickCount();
      // Log::info("USB-Poll Task started");

      while (1) {
         vTaskDelayUntil(&nextActivation, period);

         (*pollDevice)();
      }
   }

   int registerUsbDevice(PollDevice_t pd,
                         CompleteEnumeration_t ce,
                         Disconnect_t cd) {
      int ret = 0;
      static TCB_t pollTaskTCB;
      static xTaskHandle pollTaskHandle;

#define STACK_SIZE_USBTASK  1000
      static StackType_t pollTaskStack[STACK_SIZE_USBTASK];
      StructParameters_t createParameters;

      createParameters.pvParameter = NULL;
      createParameters.stack = pollTaskStack;
      createParameters.tcb = &pollTaskTCB;

      pollDevice = pd;
      completeEnumeration = ce;
      disconnectDevice = cd;

      // init usb-stack and hardware
      Chip_USB_Init();
      USB_Init(0, USB_MODE_Host);

      // create poll task
      ret = xTaskCreate(usbTask, "USBPollTask", STACK_SIZE_USBTASK,
                        &createParameters, PRIO_TASK_SERVICE, &pollTaskHandle);

      if (ret != pdPASS) {
         pearlrt::Log::error("Lpc17xxUsb: could not create poll task");
         return (1);
      }

      extern void theUSB_IRQHandler(void);

      pearlrt::Lpc17xxISRProxy::set_USB_IRQHandler(theUSB_IRQHandler);
      return 0;
   }

};



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
#include "Lpc17xxUsbKeyboard.h"
#include "Log.h"
#include "USB.h"

static bool enumerationComplete = false;
static xQueueHandle kbQueue;

#define HAS_UNGETCHAR 1

static USB_ClassInfo_HID_Host_t keyboardHidInterface;


extern "C" {
   void Lpc17xxUsbKeyboardCompleteEnumeration(uint8_t corenum, size_t
         configDescriptorSize, void * descriptorData) {
      //uint8_t ret;
      //USB_Descriptor_Configuration_Header_t *cd;  // config descriptor
      // cd = (USB_Descriptor_Configuration_Header_t *)descriptorData;
      // pearlrt::Log::info("Config Descr Type: %x", cd->Header.Type);

      keyboardHidInterface.Config.PortNumber = corenum;

      if (HID_Host_ConfigurePipes(&keyboardHidInterface, configDescriptorSize,
                                  descriptorData) != HID_ENUMERROR_NoError) {
         pearlrt::Log::error("device is no keyboard");
         return;
      }

      if (USB_Host_SetDeviceConfiguration(corenum, 1)
            != HOST_SENDCONTROL_Successful) {
         pearlrt::Log::error("error setting device configuration");
         return;
      }

      if (HID_Host_SetBootProtocol(&keyboardHidInterface) != 0) {
         pearlrt::Log::error("error setting boot protocol");
         USB_Host_SetDeviceConfiguration(corenum, 0);
         return;
      }

      pearlrt::Log::debug("USB keyboard ready");

      enumerationComplete = true;
   }

};

namespace pearlrt {

   bool Lpc17xxUsbKeyboard::doNewLineTranslation = false;

   static void disconnectDevice(void) {
      enumerationComplete = false;
   }

   static void Lpc17xxUsbKeyboardPoll() {
      HID_Host_USBTask(&keyboardHidInterface);
      Lpc17xxUsbKeyboard::kbHostTask(&keyboardHidInterface);
   }

   void Lpc17xxUsbKeyboard::kbHostTask(void * interface) {
      /* scan code to ascii mapping is treated for german keyboard only */

      USB_ClassInfo_HID_Host_t* keyboardHidInterface =
         (USB_ClassInfo_HID_Host_t*) interface;
      uint8_t keycode;
      uint8_t modifier;
      uint8_t ascii;

      static uint8_t ledState = HID_KEYBOARD_LED_NUMLOCK;

      static uint8_t lastByte = 0;
      // ..[0] normal  ..[1] shift ..[2] ALT gr
      static uint8_t lookup1[1 + HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS - HID_KEYBOARD_SC_1_AND_EXCLAMATION][3] = {
         {'1', '!', '1'}, {'2', '"', '2'}, {'3', 0xa7 /* § */, '3'},
         {'4', '$', '4'} , {'5', '%', '5'}, {'6', '&', '6'} ,
         {'7', '/', '{'}, {'8', '(', '['}, {'9', ')', ']'},
         {'0', '=', '}'}
      };

      // ENTER..SPACE
      static uint8_t lookup2[1 + HID_KEYBOARD_SC_SPACE - HID_KEYBOARD_SC_ENTER] = {
         0x0d, // ENTER
         0x1b, // ESC
         0x08, // BS
         0x09, // TAB
         ' '   // Space
      };

      // open/close bracket ...
      static uint8_t lookup3[1 +
                             HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK -
                             HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE][2] = {
         {0xdf, '?'}, {0x60, 0xb4} , {0xfc, 0xdc} , {'+', '*'}, // ß`ü+
         {0, 0}, {'#', 0x27} , {0xf6, 0xd6}, {0xe4, 0xc4},    //.#öä
         {'^', 0xb0}, {',', ';'}, {'.', ':'},		    //^
         {'-', '_'}
      };

      // keypad -- ignore numlock -- is always expeteded to be on
      static uint8_t lookup4[1 +
                             HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE -
                             HID_KEYBOARD_SC_KEYPAD_SLASH] = {
         '/', '*', '-', '+', 0x0d,
         '1', '2' , '3', '4', '5',
         '6', '7', '8', '9', '0', ','
      };

      USB_USBTask(keyboardHidInterface->Config.PortNumber, USB_MODE_Host);

      if (USB_HostState[keyboardHidInterface->Config.PortNumber] ==
            HOST_STATE_Configured) {
         if (HID_Host_IsReportReceived(keyboardHidInterface)) {
            USB_KeyboardReport_Data_t kbReport;
            HID_Host_ReceiveReport(keyboardHidInterface, &kbReport);

            modifier = kbReport.Modifier;
            keycode = kbReport.KeyCode[0];

            if (lastByte != keycode) { // || lastModifier != modifier) {
               lastByte = keycode;

               if (keycode != 0) {
                  ascii = 0;

                  // new key pressed --> translate key code
                  if (keycode >= HID_KEYBOARD_SC_A &&
                        keycode <= HID_KEYBOARD_SC_Z) {
                     if (keycode == HID_KEYBOARD_SC_Y) {
                        keycode = HID_KEYBOARD_SC_Z;
                     } else if (keycode == HID_KEYBOARD_SC_Z) {
                        keycode = HID_KEYBOARD_SC_Y;
                     }

                     ascii = keycode - HID_KEYBOARD_SC_A + 'a';

                     // test for CTRL
                     if (modifier & (HID_KEYBOARD_MODIFER_LEFTCTRL | HID_KEYBOARD_MODIFER_RIGHTCTRL)) {
                        ascii -= 0x60;
                     } else {
                        // treat shift
                        bool shiftStatus = false;

                        if (modifier & (HID_KEYBOARD_MODIFER_LEFTSHIFT | HID_KEYBOARD_MODIFER_RIGHTSHIFT)) {
                           shiftStatus = !shiftStatus;
                        }

                        if (ledState & HID_KEYBOARD_LED_CAPSLOCK) {
                           shiftStatus = !shiftStatus;
                        }

                        if (shiftStatus) {
                           ascii -= 0x20; // capital letter
                        }

                        if (keycode == HID_KEYBOARD_SC_Q && (modifier & HID_KEYBOARD_MODIFER_RIGHTALT)) {
                           ascii = '@';
                        }
                     }
                  } else if (keycode >= HID_KEYBOARD_SC_1_AND_EXCLAMATION &&
                             keycode <= HID_KEYBOARD_SC_0_AND_CLOSING_PARENTHESIS) {
                     uint8_t shift = 0;

                     if (modifier & (HID_KEYBOARD_MODIFER_LEFTSHIFT | HID_KEYBOARD_MODIFER_RIGHTSHIFT)) {
                        shift = 1;
                     }

                     if (modifier & HID_KEYBOARD_MODIFER_RIGHTALT) {
                        shift = 2;
                     }

                     ascii = lookup1[keycode - HID_KEYBOARD_SC_1_AND_EXCLAMATION][shift];
                  } else if (keycode >= HID_KEYBOARD_SC_ENTER && keycode <= HID_KEYBOARD_SC_SPACE) {
                     ascii = lookup2[keycode - HID_KEYBOARD_SC_ENTER];
                  } else if (keycode >= HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE &&
                             keycode <= HID_KEYBOARD_SC_SLASH_AND_QUESTION_MARK) {
                     uint8_t shift = 0;

                     if (modifier & (HID_KEYBOARD_MODIFER_LEFTSHIFT | HID_KEYBOARD_MODIFER_RIGHTSHIFT)) {
                        shift = 1;
                     }

                     ascii = lookup3[keycode - HID_KEYBOARD_SC_MINUS_AND_UNDERSCORE][shift];
                  } else if (keycode == HID_KEYBOARD_SC_CAPS_LOCK) {
                     if (ledState & HID_KEYBOARD_LED_CAPSLOCK) {
                        ledState &= ~HID_KEYBOARD_LED_CAPSLOCK;
                     } else {
                        ledState |= HID_KEYBOARD_LED_CAPSLOCK;
                     }

                     HID_Host_SendReportByID(keyboardHidInterface, 0, HID_REPORT_ITEM_Out, &ledState, 1);
                  } else if (keycode >= HID_KEYBOARD_SC_F1 && keycode <= HID_KEYBOARD_SC_UP_ARROW) {
                     // ignore
                  } else if (keycode == HID_KEYBOARD_SC_NUM_LOCK) {
                     if (ledState & HID_KEYBOARD_LED_NUMLOCK) {
                        ledState &= ~HID_KEYBOARD_LED_NUMLOCK;
                     } else {
                        ledState |= HID_KEYBOARD_LED_NUMLOCK;
                     }

                     HID_Host_SendReportByID(keyboardHidInterface, 0, HID_REPORT_ITEM_Out, &ledState, 1);
                  } else if (keycode >= HID_KEYBOARD_SC_KEYPAD_SLASH &&
                             keycode <= HID_KEYBOARD_SC_KEYPAD_DOT_AND_DELETE) {
                     ascii = lookup4[keycode - HID_KEYBOARD_SC_KEYPAD_SLASH];
                  } else if (keycode == HID_KEYBOARD_SC_NON_US_BACKSLASH_AND_PIPE) {
                     if (modifier & (HID_KEYBOARD_MODIFER_RIGHTALT)) {
                        ascii = '|';
                     } else {
                        if (modifier & (HID_KEYBOARD_MODIFER_LEFTSHIFT | HID_KEYBOARD_MODIFER_RIGHTSHIFT)) {
                           ascii = '>';
                        } else {
                           ascii = '<';
                        }
                     }
                  } else {
                     Log::debug("untreated  key=%x (modifier=%x)", keycode, modifier);
                  }

                  if (ascii & 0x80) {
                     ascii = 0;  // do not transport codes > 7FH
                  }

                  if (ascii == '\r' && doNewLineTranslation) {
                     ascii = '\n';
                  }


                  if (ascii) {
                     //Log::info("keycode %x --> ascii 0x%x", keycode, ascii);
                     xQueueSendToBackFromISR(kbQueue, &ascii, NULL);
                  }
               }
            }
         }
      }
   }

   Lpc17xxUsbKeyboard::Lpc17xxUsbKeyboard() {
      BaseType_t ret;


      keyboardHidInterface.Config.DataINPipeNumber	= 1;
      keyboardHidInterface.Config.DataINPipeDoubleBank  = false;
      keyboardHidInterface.Config.DataOUTPipeNumber	= 2;
      keyboardHidInterface.Config.DataOUTPipeDoubleBank = false;
      keyboardHidInterface.Config.HIDInterfaceProtocol  =
         HID_CSCP_KeyboardBootProtocol;
      keyboardHidInterface.Config.PortNumber = 0;

      doNewLineTranslation = false;

      mutex.name("Lpc17xxUsbKeyboard");
      nbrOpenUserDations = 0;

      kbQueue = xQueueCreate(50, sizeof(char));

      if (!kbQueue) {
         Log::error("Lpc17xxUsbKeyboard: could not create queue");
         // this exception terminates the application,
         // since the ctor is called before main()
         throw theInternalDationSignal;
      }

      ret = registerUsbDevice(Lpc17xxUsbKeyboardPoll,
                              Lpc17xxUsbKeyboardCompleteEnumeration,
                              disconnectDevice);

      if (ret != 0) {
         throw theInternalDationSignal;
      }

      status = 0;   // no unget char present
   }


   int Lpc17xxUsbKeyboard::capabilities() {
      return (Dation::FORWARD | Dation::IN | Dation::PRM | Dation::ANY);
   }

   Lpc17xxUsbKeyboard* Lpc17xxUsbKeyboard::dationOpen(const char * idfValue,
         int openParams) {
      if (openParams & (Dation::IDF | Dation::CAN)) {
         Log::error("Lpc17xxUsbKeyboard: does not support IDF and CAN");
         throw theDationParamSignal;
      }

      mutex.lock();

      if (!enumerationComplete) {
         Log::error("Lpc17xxUsbKeyboard: enumeration not completed");
         mutex.unlock();
         throw theOpenFailedSignal;
      }

      nbrOpenUserDations++;

      mutex.unlock();

      return this;
   }

   void Lpc17xxUsbKeyboard::dationClose(int closeParams) {

      mutex.lock();

      if (nbrOpenUserDations == 0) {
         Log::error("Lpc17xxUsbKeyboard: no dation opened");
         mutex.unlock();
         throw theCloseFailedSignal;
      }


      if (closeParams & Dation::CAN) {
         Log::error("Lpc17xxUsbKeyboard: CAN not supported");
         mutex.unlock();
         throw theDationParamSignal;
      }

      nbrOpenUserDations--;

      mutex.unlock();
   }

   void Lpc17xxUsbKeyboard::dationRead(void * destination, size_t size) {
      char* dataPtr = (char*) destination;

      if (nbrOpenUserDations == 0) {
         Log::error("Lpc17xxUsbKeyboard: not opened");
         throw theDationNotOpenSignal;
      }

      if (!enumerationComplete) {
         Log::error("Lpc17xxUsbKeyboard: lost connection");
         throw theReadingFailedSignal;
      }

      mutex.lock();


      if (status & HAS_UNGETCHAR) {
         status &= ~ HAS_UNGETCHAR;
         *dataPtr = unGetChar;
         size --;
         dataPtr++;
      }

      while (size > 0) {
         xQueueReceive(kbQueue, dataPtr, portMAX_DELAY);
         dataPtr ++;
         size --;
      }

      mutex.unlock();
   }

   void Lpc17xxUsbKeyboard::dationWrite(void * destination, size_t size) {
      Log::error("Lpc17xxUsbKeyboard: write not supported");
      throw theInternalDationSignal;
   }

   void Lpc17xxUsbKeyboard::translateNewLine(bool doNewLineTranslation) {
      this->doNewLineTranslation = doNewLineTranslation;
   };

   void Lpc17xxUsbKeyboard::dationUnGetChar(const char c) {
      mutex.lock();
      unGetChar = c;
      status |= HAS_UNGETCHAR;
      mutex.unlock();
   }


}


//
// Created by nikhil on 11/12/21.
//

#ifndef _BLUETOOTH_H_
#define _BLUETOOTH_H_
#include "Arduino.h"
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
#include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

class Bluetooth {
 public:
    static String GetTerminalInput();
    static char* GetInput();
    static void InitBtModule();
    static void Error(const __FlashStringHelper* err);

};
#endif //_BLUETOOTH_H_

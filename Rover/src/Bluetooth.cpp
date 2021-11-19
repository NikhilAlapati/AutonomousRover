/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include "Bluetooth.h"

Adafruit_BluefruitLE_UART mySerial(Serial1, BLUEFRUIT_UART_MODE_PIN);

void Bluetooth::Error(const __FlashStringHelper* err) {
    Serial.println(err);
    while (1);
}

/*void setup(void) {
    InitBtModule();

}*/
/**
 * Connects to the BT module and initializes it
 */
void Bluetooth::InitBtModule() {
    while (!Serial);  // required for Flora & Micro
    delay(500);

    Serial.begin(9600);
    Serial.println(F("Adafruit Bluefruit Command Mode Example"));
    Serial.println(F("---------------------------------------"));

    /* Initialise the module */
    Serial.print(F("Initialising the Bluefruit LE module: "));

    if (!mySerial.begin(VERBOSE_MODE)) {
        Error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
    }
    Serial.println(F("OK!"));

    if (FACTORYRESET_ENABLE) {
        /* Perform a factory reset to make sure everything is in a known state */
        Serial.println(F("Performing a factory reset: "));
        if (!mySerial.factoryReset()) {
            Error(F("Couldn't factory reset"));
        }
    }

    /* Disable command echo from Bluefruit */
    mySerial.echo(false);

    Serial.println("Requesting Bluefruit info:");
    /* Print Bluefruit information */
    mySerial.info();

    Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
    Serial.println(F("Then Enter characters to send to Bluefruit"));
    Serial.println();

    mySerial.verbose(false);  // debug info is a little annoying after this point!

    /* Wait for connection */
    while (!mySerial.isConnected()) {
        delay(500);
    }

    // LED Activity command is only supported from 0.6.6
    if (mySerial.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION)) {
        // Change Mode LED Activity
        Serial.println(F("******************************"));
        Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
        mySerial.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
        Serial.println(F("******************************"));
    }
    pinMode(LED_BUILTIN, OUTPUT);
}
/**
 * Gets input from the bluetooth module
 * @return string of the input
 */
char* Bluetooth::GetInput() {
    mySerial.println("AT+BLEUARTRX");
    mySerial.readline();
    char* string = mySerial.buffer;
    // Some data was found, its in the buffer
    if (strcmp(string, "OK") == 0) {
        // no data
        return (char*)"\0";
    }
    Serial.println(string);
    return string;
}
/**
 * Gets user input from the serial monitor and sends it to the bluetooth
 * @return returns the string from the terminal
 */
String Bluetooth::GetTerminalInput() {
    String input = "";
    while (Serial.available() != 0) {
        input = Serial.readString();
        Serial.println(input);
        mySerial.print("AT+BLEUARTTX=");
        mySerial.println(input);
    }
    return input;
}



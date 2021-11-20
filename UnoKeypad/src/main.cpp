#include <Arduino.h>
#include "keypad.h"

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
    { '1', '2', '3', 'A' },
    { '4', '5', '6', 'B' },
    { '7', '8', '9', 'C' },
    { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 9, 8, 7, 6 }; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 5, 4, 3, 2 }; //connect to the column pinouts of the keypad
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
String msg = "";
void setup() {
    Serial.begin(9600);
}

void loop() {
    if (kpd.getKeys()) {
        for (int i = 0; i < LIST_MAX; i++)   // Scan the whole key list.
        {
            if (kpd.key[i].stateChanged)   // Only find keys that have changed state.
            {
                switch (kpd.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                case PRESSED:
                    Serial.println(kpd.key[i].kchar);
                    break;
                case HOLD:
                    break;
                case RELEASED:
                    break;
                case IDLE:
                    break;
                }
            }
        }
    }
}

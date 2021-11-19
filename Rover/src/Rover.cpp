#include "Arduino.h"
#include "Bluetooth.h"
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor* leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor* rightMotor = AFMS.getMotor(2);
#define trigPin 3
#define echoPin 2
bool manualDrive = false;
float getUltrasonic();
void setup() {
//     Bluetooth::InitBtModule();
    /*if (!AFMS.begin()) {
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }*/
    Serial.println("Motor Shield found.");
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);

}
void moveForward() {
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
}
void moveBackwards() {
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
}
void moveRightwards() {
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
}
void moveLeftwards() {
    leftMotor->setSpeed(150);
    rightMotor->setSpeed(150);
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
}
/*void stop() {
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(0);
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}*/
void loop() {
    Serial.println("loop");
//     Bluetooth::GetTerminalInput();
//     char input = Bluetooth::GetInput()[0];
    //Autonomous Mode
    if (!manualDrive) {
        Serial.println("inside");
        float distance = getUltrasonic();
        Serial.println(distance);
    }
//     switch (input) {
//     case 'w':
//         moveForward();
//         manualDrive = true;
//     case 's':
//         moveBackwards();
//         manualDrive = true;
//     case 'd':
//         moveRightwards();
//         manualDrive = true;
//     case 'a':
//         moveLeftwards();
//         manualDrive = true;
//     case 'b':
//         stop();
//         manualDrive = true;
//     default:
//         break;
//     }
}
float getUltrasonic() {
    float duration, distance;
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
//     distance = (duration / 2.0f) / 29.14f;
    float soundSpeed = 331.3f + 0.606f * ((analogRead(A0) * 5.0f / 1024.0f - 0.5f) * 100.0f);
    distance = (duration / 20000.0f) * soundSpeed;
    /*if (distance >= 200 || distance <= 0) {
        Serial.println("Out of range");
    } else {
        Serial.print(distance);
        Serial.println(" cm");
    }*/

    return distance;
}

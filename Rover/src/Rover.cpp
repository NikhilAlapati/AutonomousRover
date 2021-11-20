#include "Arduino.h"
#include "Bluetooth.h"
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor* leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor* rightMotor = AFMS.getMotor(2);
#define trigPin 3
#define echoPin 2
#define speed 100
#define turnSpeed 75
bool manualDrive = true;

float getUltrasonic();
void setup() {
    Bluetooth::InitBtModule();
    // Begins the motor shield if not found then loops until it's found
    if (!AFMS.begin()) {
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}
/**
 * Moves the motor forward, back, left, and right by manipulating the left and right motor's directions
 */
void moveForward() {
    leftMotor->setSpeed(speed);
    rightMotor->setSpeed(speed);
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
}
void moveBackwards() {
    Serial.println("B");
    leftMotor->setSpeed(speed);
    rightMotor->setSpeed(speed);
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
}
void moveLeft() {
    Serial.println("R");
    leftMotor->setSpeed(turnSpeed);
    rightMotor->setSpeed(turnSpeed);
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
}
void moveRight() {
    Serial.println("L");
    leftMotor->setSpeed(turnSpeed);
    rightMotor->setSpeed(turnSpeed);
    leftMotor->run(BACKWARD);
    rightMotor->run(FORWARD);
}
void stop() {
    leftMotor->setSpeed(0);
    rightMotor->setSpeed(0);
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
}

void loop() {
    Bluetooth::GetTerminalInput();
    char input = Bluetooth::GetInput()[0];
    //Autonomous Mode
    float distance = getUltrasonic();
    Serial.println(distance);
    if (!manualDrive) {
        moveForward();
        float distance = getUltrasonic();
        float followDistance = 20.0f;
        if (distance < followDistance) {
            stop();
            unsigned long direction = random(1);
            while (true) {
                // Determining what direction to turn in
                if (direction == 0) {
                    moveRight();
                } else {
                    moveLeft();
                }
                delay(250);
                stop();
                delay(250);
                distance = getUltrasonic();
                if (distance > followDistance) {
                    stop();
                    moveForward();
                    break;
                }

            }
        }
    }
    switch (input) {
    case 'w':
        moveForward();
        manualDrive = true;
        break;
    case 's':
        moveBackwards();
        manualDrive = true;
        break;
    case 'a':
        moveLeft();
        manualDrive = true;
        break;
    case 'd':
        moveRight();
        manualDrive = true;
        break;
    case 'b':
        stop();
        manualDrive = true;
        break;
    case 'm':
        manualDrive = !manualDrive;
        break;
    default:
        break;
    }
}
float getUltrasonic() {
    float duration, distance;
    digitalWrite(trigPin, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    float soundSpeed = 331.3f + 0.606f * ((analogRead(A0) * 5.0f / 1024.0f - 0.5f) * 100.0f);
    distance = (duration / 20000.0f) * soundSpeed;

    return distance;
}

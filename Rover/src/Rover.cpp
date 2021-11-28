#include "Arduino.h"
#include "Bluetooth.h"
#include <Adafruit_MotorShield.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_Accel.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
Adafruit_DCMotor* leftMotor = AFMS.getMotor(1);
Adafruit_DCMotor* rightMotor = AFMS.getMotor(2);
#define TRIG_PIN 3
#define ECHO_PIN 2
#define SPEED 100
#define TURN_SPEED 75
#define PHOTOCELL_PIN 1
#define HEADLIGHTS 10
#define BRAKE_LIGHTS 13
#define HORN_PIN 48
bool manualDrive = true;
float getUltrasonic();
float getTemperature();
int getPhotocellReading();
bool activateBrakeLights();
void activateHeadlightsIfDark();
void sendSensorInformation();
void setup() {
    Bluetooth::InitBtModule();
    // Begins the motor shield if not found then loops until it's found
    if (!AFMS.begin()) {
        Serial.println("Could not find Motor Shield. Check wiring.");
        while (1);
    }
    Serial.println("Motor Shield found.");
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    if (!accel.begin()) {
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
        while (1);
    }
    accel.setRange(LSM303_RANGE_4G);
    Serial.print("Range set to: ");
    lsm303_accel_range_t new_range = accel.getRange();
    switch (new_range) {
    case LSM303_RANGE_2G:
        Serial.println("+- 2G");
        break;
    case LSM303_RANGE_4G:
        Serial.println("+- 4G");
        break;
    case LSM303_RANGE_8G:
        Serial.println("+- 8G");
        break;
    case LSM303_RANGE_16G:
        Serial.println("+- 16G");
        break;
    }

    accel.setMode(LSM303_MODE_NORMAL);
    Serial.print("Mode set to: ");
    lsm303_accel_mode_t new_mode = accel.getMode();
    switch (new_mode) {
    case LSM303_MODE_NORMAL:
        Serial.println("Normal");
        break;
    case LSM303_MODE_LOW_POWER:
        Serial.println("Low Power");
        break;
    case LSM303_MODE_HIGH_RESOLUTION:
        Serial.println("High Resolution");
        break;
    }
}
/**
 * Moves the motor forward, back, left, and right by manipulating the left and right motor's directions
 */
void moveForward() {
    leftMotor->setSpeed(SPEED);
    rightMotor->setSpeed(SPEED);
    leftMotor->run(FORWARD);
    rightMotor->run(FORWARD);
}
void moveBackwards() {
    Serial.println("B");
    leftMotor->setSpeed(SPEED);
    rightMotor->setSpeed(SPEED);
    leftMotor->run(BACKWARD);
    rightMotor->run(BACKWARD);
}
void moveLeft() {
    Serial.println("R");
    leftMotor->setSpeed(TURN_SPEED);
    rightMotor->setSpeed(TURN_SPEED);
    leftMotor->run(FORWARD);
    rightMotor->run(BACKWARD);
}
void moveRight() {
    Serial.println("L");
    leftMotor->setSpeed(TURN_SPEED);
    rightMotor->setSpeed(TURN_SPEED);
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
    activateHeadlightsIfDark();
    activateBrakeLights();
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
    case '2':
        moveForward();
        manualDrive = true;
        break;
    case '5':
        moveBackwards();
        manualDrive = true;
        break;
    case '4':
        moveLeft();
        manualDrive = true;
        break;
    case '6':
        moveRight();
        manualDrive = true;
        break;
    case 'B':
        stop();
        manualDrive = true;
        break;
    case 'A':
        manualDrive = !manualDrive;
        break;
    case '*':
        tone(HORN_PIN, 500, 1000);
        break;
    case '0':
        sendSensorInformation();
        break;
    default:
        break;
    }
}
void sendSensorInformation() {
    sensors_event_t event;
    accel.getEvent(&event);
    Bluetooth::SendMessages("Ultrasonic Distance in cm: " + (String)getUltrasonic() + "\n");
    Bluetooth::SendMessages("Temperature Reading in C: " + (String)getTemperature() + "\n");
    Bluetooth::SendMessages("Accelerometer Reading in m/s^2 (X, Y, Z): " + (String)event.acceleration.x + " " + (String)event.acceleration.y + " "
                            + (String)event.acceleration.z + "\n");
    Bluetooth::SendMessages("Photocell Reading: " + (String)getPhotocellReading() + "\n");

}

void activateHeadlightsIfDark() {
    if (getPhotocellReading() < 150) {
        analogWrite(HEADLIGHTS, 255);
    } else {
        analogWrite(HEADLIGHTS, 0);
    }
}
float getUltrasonic() {
    float duration, distance;
    digitalWrite(TRIG_PIN, LOW);  // Added this line
    delayMicroseconds(2); // Added this line
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10); // Added this line
    digitalWrite(TRIG_PIN, LOW);
    duration = pulseIn(ECHO_PIN, HIGH);
    float soundSpeed = 331.3f + 0.606f * getTemperature();
    distance = (duration / 20000.0f) * soundSpeed;

    return distance;
}
float getTemperature() {
    return ((analogRead(A0) * 5.0f / 1024.0f - 0.5f) * 100.0f);
}
int getPhotocellReading() {
    return analogRead(PHOTOCELL_PIN);
}
bool activateBrakeLights() {
    sensors_event_t event;
    accel.getEvent(&event);
    if (event.acceleration.x <= -2) {
        analogWrite(BRAKE_LIGHTS, 150);
    } else {
        analogWrite(BRAKE_LIGHTS, 0);
    }
}

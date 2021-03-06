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
#define SPEED 150
#define TURN_SPEED 125
#define PHOTOCELL_PIN 1
#define HEADLIGHTS 10
#define BRAKE_LIGHTS 13
#define HORN_PIN 48
#define SENSOR_INTERVAL 10000

#define HORN_DURATION 1000
bool manualDrive = true;
bool headlights = false;
bool headlightOn = true;
unsigned long prevHornMillis = 0;
unsigned long prevMillis = 0;
bool periodicalCollection = true;
bool horn = false;
float getUltrasonic();
float getTemperature();
int getPhotocellReading();
void activateBrakeLights();
void activateHeadlightsIfDark();
void sendSensorInformation();
void turnOnBrakeLight();
void turnOffBrakeLight();
void activateHeadlights();
void disableHeadlights();
void toggleHeadlights();
void checkHeadLightToggle();
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
    checkHeadLightToggle();
    activateBrakeLights();
    if (periodicalCollection) {
        unsigned long currMillis = millis();
        if (prevMillis == 0 || (currMillis - prevMillis) >= SENSOR_INTERVAL) {
            sendSensorInformation();
            prevMillis = currMillis;
        }
    }
    if (horn) {
        unsigned long currMillis = millis();
        if ((currMillis - prevHornMillis) >= HORN_DURATION) {
            analogWrite(HORN_PIN, 0);
            horn = false;
        }
    }
    if (!manualDrive) {
        moveForward();
        float distance = getUltrasonic();
        float followDistance = 20.0f;
        if (distance < followDistance) {
            checkHeadLightToggle();
            activateBrakeLights();
            stop();
            unsigned long direction = rand() % 2;
            while (true) {
                checkHeadLightToggle();
                // Determining what direction to turn in
                if (direction == 0) {
                    moveRight();
                } else {
                    moveLeft();
                }
                delay(200);
                stop();
                turnOnBrakeLight();
                delay(200);
                turnOffBrakeLight();
                distance = getUltrasonic();
                if (distance > followDistance) {
                    stop();
                    activateBrakeLights();
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
        horn = true;
        prevHornMillis = millis();
        analogWrite(HORN_PIN, 255);
//         tone(HORN_PIN, 500, 1000);
        break;
    case '0':
        sendSensorInformation();
        break;
    case '#':
        periodicalCollection = !periodicalCollection;
        Bluetooth::SendMessages("Periodical Collection is " + (String)(!periodicalCollection ? "disabled" : "enabled"));
        break;
    case '7':
        headlights = true;
        toggleHeadlights();
        Bluetooth::SendMessages("Headlight Toggled Automatic headlights off");
        break;
    case '9':
        headlights = false;
        Bluetooth::SendMessages("Automatic Headlights Turned On");
        break;
    default:
        break;
    }
}
void checkHeadLightToggle() {
    if (!headlights) {
        activateHeadlightsIfDark();
    }
}
void toggleHeadlights() {
    if (headlightOn) {
        activateHeadlights();
    } else {
        disableHeadlights();
    }
    headlightOn = !headlightOn;
}
void turnOffBrakeLight() {
    analogWrite(BRAKE_LIGHTS, 0);
}
void turnOnBrakeLight() {
    analogWrite(BRAKE_LIGHTS, 92);
}
void sendSensorInformation() {
    sensors_event_t event;
    accel.getEvent(&event);
    Bluetooth::SendMessages("Dist: " + (String)getUltrasonic() + " Temp: " + (String)getTemperature()
                            + "C Accel:" + (String)event.acceleration.x + " " + (String)event.acceleration.y + " "
                            + (String)event.acceleration.z + " light:" + (String)getPhotocellReading());
}

void activateHeadlightsIfDark() {
    if (getPhotocellReading() < 200) {
        activateHeadlights();
    } else {
        disableHeadlights();
    }
}
void disableHeadlights() {
    analogWrite(HEADLIGHTS, 0);
}
void activateHeadlights() {
    analogWrite(HEADLIGHTS, 164);
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
void activateBrakeLights() {
    sensors_event_t event;
    accel.getEvent(&event);
    if (event.acceleration.x <= -2) {
        turnOnBrakeLight();
    } else {
        turnOffBrakeLight();
    }
}

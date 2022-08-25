#include <Arduino.h>
#include "CurieIMU.h"

int ax, ay, az;  // accelerometer values
int gx, gy, gz;  // gyrometer values
const int ledPin = 13;  // activity LED pin
bool blinkState = false;  // state of the LED
int calibrateOffsets = 1;  // int to determine whether calibration takes place or not

void setup() {
    Serial.begin(9600);  // initialize Serial communication
    while (!Serial)
        ;  // wait for the serial port to open
    // initialize device
    Serial.println("Initializing IMU device...");
    CurieIMU.begin();
    // verify connection
    Serial.println("Testing device connections...");
    if (CurieIMU.begin()) {
        Serial.println("CurieIMU connection successful");
    } else {
        Serial.println("CurieIMU connection failed");
    }
    // use the code below to calibrate accel/gyro offset values
    if (calibrateOffsets == 1) {
        Serial.println("Internal sensor offsets BEFORE calibration...");
        Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
        Serial.print("\t");  // -76
        Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
        Serial.print("\t");  // -235
        Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
        Serial.print("\t");  // 168
        Serial.print(CurieIMU.getGyroOffset(X_AXIS));
        Serial.print("\t");  // 0
        Serial.print(CurieIMU.getGyroOffset(Y_AXIS));
       Serial.print("\t");  // 0
        Serial.println(CurieIMU.getGyroOffset(Z_AXIS));


        Serial.println("About to calibrate. Make sure your board is stable and upright");

        delay(5000);

        Serial.print("Starting Gyroscope calibration and enabling offset compensation...");
        CurieIMU.autoCalibrateGyroOffset();
        Serial.println(" Done");
        Serial.print("Starting Acceleration calibration and enabling offset compensation...");
        CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
        CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
        CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
        Serial.println(" Done");
        Serial.println("Internal sensor offsets AFTER calibration...");
        Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
        Serial.print("\t");  // -76
        Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
        Serial.print("\t");  // -2359
        Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
        Serial.print("\t");  // 1688
        Serial.print(CurieIMU.getGyroOffset(X_AXIS));
        Serial.print("\t");  // 0
        Serial.print(CurieIMU.getGyroOffset(Y_AXIS));
        Serial.print("\t");  // 0
        Serial.println(CurieIMU.getGyroOffset(Z_AXIS));
    }

    // configure Arduino LED for activity indicator

    pinMode(ledPin, OUTPUT);
}

void loop() {
    // read raw accel/gyro measurements from device

    CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);

    Serial.print("a/g:\t");

    Serial.print(ax);

    Serial.print("\t");

    Serial.print(ay);

    Serial.print("\t");

    Serial.print(az);

    Serial.print("\t");

    Serial.print(gx);

    Serial.print("\t");

    Serial.print(gy);

    Serial.print("\t");

    Serial.println(gz);

    // blink LED to indicate activity

    blinkState = !blinkState;

    digitalWrite(ledPin, blinkState);
}
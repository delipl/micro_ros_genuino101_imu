#include <Arduino.h>

#include "CurieIMU.h"

int raw_acceleration_x, raw_acceleration_y, raw_acceleration_z;  // accelerometer values
int raw_gyration_x, raw_gyration_y, raw_gyration_z;              // gyrometer values
const int ledPin = 13;                                           // activity LED pin
bool blinkState = false;                                         // state of the LED
int calibrateOffsets = 1;                                        // int to determine whether calibration takes place or not
float dt = 0.0;

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
    dt = millis();
}

float acceleration_x;
float acceleration_y;
float acceleration_z;
float gyration_x;
float gyration_y;
float gyration_z;

float yaw = 0.0;
float pitch = 0.0;
float roll = 0.0;

int counter = 0;


void read_and_accumulate_measurements(){
    int read_acceleration_x, read_acceleration_y, read_acceleration_z;
    int read_gyration_x, read_gyration_y, read_gyration_z;

    CurieIMU.readAccelerometer(read_acceleration_x, read_acceleration_y, read_acceleration_z);
    CurieIMU.readGyro(read_gyration_x, read_gyration_y, read_gyration_z);
    raw_acceleration_x += read_acceleration_x;
    raw_acceleration_y += read_acceleration_y;
    raw_acceleration_z += read_acceleration_z;

    raw_gyration_x += read_gyration_x;
    raw_gyration_y += read_gyration_y;
    raw_gyration_z += read_gyration_z;
}

void calculate_acceleration_and_gyration(){
    if (counter == 50) {
        dt = (millis() - dt) / 1000;
        acceleration_x = (raw_acceleration_x / 32768.0 / counter) * CurieIMU.getAccelerometerRange() * 9.80665;
        acceleration_y = (raw_acceleration_y / 32768.0 / counter) * CurieIMU.getAccelerometerRange() * 9.80665;
        acceleration_z = (raw_acceleration_z / 32768.0 / counter) * CurieIMU.getAccelerometerRange() * 9.80665;
        gyration_x = (raw_gyration_x / 32768.9 / counter) * CurieIMU.getGyroRange() * 3.14159265359 / 180.0;
        gyration_y = (raw_gyration_y / 32768.9 / counter) * CurieIMU.getGyroRange() * 3.14159265359 / 180.0;
        gyration_z = (raw_gyration_z / 32768.9 / counter) * CurieIMU.getGyroRange() * 3.14159265359 / 180.0;

        Serial.print("a/g:\t");
        Serial.print(gyration_x);
        Serial.print("\t");
        Serial.print(gyration_y);
        Serial.print("\t");
        Serial.print(gyration_z);
        Serial.print("\t");
        Serial.print(acceleration_x);
        Serial.print("\t");
        Serial.print(acceleration_y);
        Serial.print("\t");
        Serial.println(acceleration_z);

        blinkState = !blinkState;
        digitalWrite(ledPin, blinkState);
        ;

        raw_gyration_x = 0;
        raw_gyration_y = 0;
        raw_gyration_z = 0;
        raw_acceleration_x = 0;
        raw_acceleration_y = 0;
        raw_acceleration_z = 0;
        counter = 0;
    }
}

void loop() {
    read_and_accumulate_measurements();
    calculate_acceleration_and_gyration();

    delay(10);
    ++counter;
}
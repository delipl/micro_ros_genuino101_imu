#include <CurieIMU.h>
#include <MadgwickAHRS.h>

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;

float convertRawAcceleration(int aRaw);
float convertRawGyro(int gRaw);

void setup() {
    Serial1.begin(9600);
    Serial.begin(9600);

    // start the IMU and filter

    CurieIMU.begin();

    CurieIMU.setGyroRate(25);

    CurieIMU.setAccelerometerRate(25);

    filter.begin(25);

    // Set the accelerometer range to 2G

    CurieIMU.setAccelerometerRange(2);

    // Set the gyroscope range to 250 degrees/second

    CurieIMU.setGyroRange(250);

    // initialize variables to pace updates to correct rate

    microsPerReading = 1000000 / 25;

    microsPrevious = micros();
}

void loop() {
    int aix, aiy, aiz;

    int gix, giy, giz;

    float ax, ay, az;

    float gx, gy, gz;

    float roll, pitch, heading;

    unsigned long microsNow;

    // check if it's time to read data and update the filter

    microsNow = micros();

    if (microsNow - microsPrevious >= microsPerReading) {
        // read raw data from CurieIMU

        CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

        // convert from raw data to gravity and degrees/second units

        ax = convertRawAcceleration(aix);

        ay = convertRawAcceleration(aiy);

        az = convertRawAcceleration(aiz);

        gx = convertRawGyro(gix);

        gy = convertRawGyro(giy);

        gz = convertRawGyro(giz);

        // update the filter, which computes orientation

        filter.updateIMU(gx, gy, gz, ax, ay, az);

        // print the heading, pitch and roll

        roll = filter.getRoll() * 3.14/180;

        pitch = filter.getPitch() * 3.14 / 180;

        heading = filter.getYaw() * 3.14 / 180 - 3.14;
        heading *= -1;

        Serial1.print(heading);
        Serial1.print("\t");
        Serial1.print(pitch);
        Serial1.println();

        Serial.print(heading);
        Serial.print("\t");
        Serial.print(pitch);
        Serial.println();

        // increment previous time, so we keep proper pace

        microsPrevious = microsPrevious + microsPerReading;
    }
}

float convertRawAcceleration(int aRaw) {
    // since we are using 2G range

    // -2g maps to a raw value of -32768

    // +2g maps to a raw value of 32767

    float a = (aRaw * 2.0) / 32768.0;

    return a;
}

float convertRawGyro(int gRaw) {
    // since we are using 250 degrees/seconds range

    // -250 maps to a raw value of -32768

    // +250 maps to a raw value of 32767

    float g = (gRaw * 250.0) / 32768.0;

    return g;
}
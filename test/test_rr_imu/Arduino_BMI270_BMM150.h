// Mock Arduino_BMI270_BMM150.h for native testing
// This file provides stubs for the BMI270/BMM150 IMU sensor

#ifndef MOCK_ARDUINO_BMI270_BMM150_H
#define MOCK_ARDUINO_BMI270_BMM150_H

// Mock IMU class
class MockBMI270_BMM150 {
public:
    bool begin() { return true; }
    bool accelerationAvailable() { return accel_available; }
    bool gyroscopeAvailable() { return gyro_available; }

    int readAcceleration(float& x, float& y, float& z) {
        x = mock_ax;
        y = mock_ay;
        z = mock_az;
        return 1;
    }

    int readGyroscope(float& x, float& y, float& z) {
        x = mock_gx;
        y = mock_gy;
        z = mock_gz;
        return 1;
    }

    // Mock control variables
    bool accel_available = true;
    bool gyro_available = true;
    float mock_ax = 0.0f;
    float mock_ay = 0.0f;
    float mock_az = 1.0f;  // Gravity on z-axis
    float mock_gx = 0.0f;
    float mock_gy = 0.0f;
    float mock_gz = 0.0f;
};

extern MockBMI270_BMM150 IMU;

#endif // MOCK_ARDUINO_BMI270_BMM150_H

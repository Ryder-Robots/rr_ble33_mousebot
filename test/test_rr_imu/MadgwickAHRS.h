// Mock MadgwickAHRS.h for native testing
// This file provides stubs for the Madgwick filter

#ifndef MOCK_MADGWICKAHRS_H
#define MOCK_MADGWICKAHRS_H

// Mock Madgwick filter
class MockMadgwick {
public:
    void begin(float freq) { sample_freq = freq; }
    void updateIMU(float gx, float gy, float gz, float ax, float ay, float az) {
        // Simple mock - just store the values
        last_gx = gx; last_gy = gy; last_gz = gz;
        last_ax = ax; last_ay = ay; last_az = az;
    }
    float getRoll() { return mock_roll; }
    float getPitch() { return mock_pitch; }
    float getYaw() { return mock_yaw; }

    // Mock control variables
    float sample_freq = 0.0f;
    float mock_roll = 0.0f;
    float mock_pitch = 0.0f;
    float mock_yaw = 0.0f;
    float last_gx = 0.0f, last_gy = 0.0f, last_gz = 0.0f;
    float last_ax = 0.0f, last_ay = 0.0f, last_az = 0.0f;
};

// Create alias for compatibility
typedef MockMadgwick Madgwick;

#endif // MOCK_MADGWICKAHRS_H

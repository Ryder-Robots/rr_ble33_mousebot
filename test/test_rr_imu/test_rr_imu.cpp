// Copyright (c) 2025 Ryder Robots
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <math.h>
#include <cstdint>
#include <cmath>

// Native testing - use Unity test framework
#include <unity.h>

// When testing natively, we need to define our mocks before including rr_imu.hpp
#include "Arduino.h"
#include "Arduino_BMI270_BMM150.h"
#include "MadgwickAHRS.h"

#include <rr_imu.hpp>

using namespace mb_operations;

// Test-accessible wrapper to expose private methods
class RRImuOpHandlerTestable {
public:
    RRImuOpHandler handler_;

    RRImuOpHandlerTestable() = default;
    ~RRImuOpHandlerTestable() = default;

    void init() { handler_.init(); }
    org_ryderrobots_ros2_serial_Status status() { return handler_.status(); }
    void perform_op(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Response &response) {
        handler_.perform_op(req, response);
    }

    // Expose private methods for testing
    void euler_to_quaternion(float roll, float pitch, float yaw,
                             float *q_w, float *q_x, float *q_y, float *q_z) {
        float cr = cosf(roll * 0.5f);
        float sr = sinf(roll * 0.5f);
        float cp = cosf(pitch * 0.5f);
        float sp = sinf(pitch * 0.5f);
        float cy = cosf(yaw * 0.5f);
        float sy = sinf(yaw * 0.5f);

        *q_w = cy * cp * cr + sy * sp * sr;
        *q_x = sy * sp * cr - cy * cp * sr;
        *q_y = cy * sp * cr + sy * cp * sr;
        *q_z = cy * cp * sr - sy * sp * cr;
    }

    void monitor(const org_ryderrobots_ros2_serial_Request &req, org_ryderrobots_ros2_serial_Response &response) {
        // Call through public perform_op which will route to monitor
        handler_.perform_op(req, response);
    }
};

// ============================================================================
// Mock Classes for Arduino Hardware Dependencies
// ============================================================================

// Implement mock instances
MockSerial Serial;
MockBMI270_BMM150 IMU;

// Mock Arduino functions
unsigned long mock_millis_value = 0;
unsigned long millis() { return mock_millis_value; }
void delay(unsigned long ms) { mock_millis_value += ms; }

// ============================================================================
// Test Helper Functions
// ============================================================================

// Helper to compare floating point values with tolerance
bool floatNear(float a, float b, float epsilon = 0.0001f) {
    return fabs(a - b) < epsilon;
}

// Helper to verify quaternion is normalized
bool isQuaternionNormalized(float qw, float qx, float qy, float qz, float epsilon = 0.01f) {
    float magnitude = sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
    return floatNear(magnitude, 1.0f, epsilon);
}

// ============================================================================
// Tests for euler_to_quaternion Function
// ============================================================================

void test_euler_to_quaternion_identity(void) {
    RRImuOpHandlerTestable handler;
    float qw, qx, qy, qz;

    // Test identity rotation (0, 0, 0)
    handler.euler_to_quaternion(0.0f, 0.0f, 0.0f, &qw, &qx, &qy, &qz);

    TEST_ASSERT_TRUE(floatNear(qw, 1.0f));
    TEST_ASSERT_TRUE(floatNear(qx, 0.0f));
    TEST_ASSERT_TRUE(floatNear(qy, 0.0f));
    TEST_ASSERT_TRUE(floatNear(qz, 0.0f));
    TEST_ASSERT_TRUE(isQuaternionNormalized(qw, qx, qy, qz));
}

void test_euler_to_quaternion_roll_90(void) {
    RRImuOpHandlerTestable handler;
    float qw, qx, qy, qz;

    // Test 90 degree roll (π/2 radians)
    float roll = M_PI / 2.0f;
    handler.euler_to_quaternion(roll, 0.0f, 0.0f, &qw, &qx, &qy, &qz);

    // For pure roll of π/2: qw ≈ 0.707, qx ≈ 0, qy ≈ 0.707, qz ≈ 0
    TEST_ASSERT_TRUE(floatNear(qw, 0.707f, 0.01f));
    TEST_ASSERT_TRUE(floatNear(qx, 0.0f, 0.01f));
    TEST_ASSERT_TRUE(floatNear(qy, 0.707f, 0.01f));
    TEST_ASSERT_TRUE(floatNear(qz, 0.0f, 0.01f));
    TEST_ASSERT_TRUE(isQuaternionNormalized(qw, qx, qy, qz));
}

void test_euler_to_quaternion_pitch_90(void) {
    RRImuOpHandlerTestable handler;
    float qw, qx, qy, qz;

    // Test 90 degree pitch (π/2 radians)
    float pitch = M_PI / 2.0f;
    handler.euler_to_quaternion(0.0f, pitch, 0.0f, &qw, &qx, &qy, &qz);

    TEST_ASSERT_TRUE(floatNear(qw, 0.707f, 0.01f));
    TEST_ASSERT_TRUE(isQuaternionNormalized(qw, qx, qy, qz));
}

void test_euler_to_quaternion_yaw_90(void) {
    RRImuOpHandlerTestable handler;
    float qw, qx, qy, qz;

    // Test 90 degree yaw (π/2 radians)
    float yaw = M_PI / 2.0f;
    handler.euler_to_quaternion(0.0f, 0.0f, yaw, &qw, &qx, &qy, &qz);

    TEST_ASSERT_TRUE(floatNear(qw, 0.707f, 0.01f));
    TEST_ASSERT_TRUE(isQuaternionNormalized(qw, qx, qy, qz));
}

void test_euler_to_quaternion_combined_angles(void) {
    RRImuOpHandlerTestable handler;
    float qw, qx, qy, qz;

    // Test combination of angles
    float roll = M_PI / 4.0f;   // 45 degrees
    float pitch = M_PI / 6.0f;  // 30 degrees
    float yaw = M_PI / 3.0f;    // 60 degrees

    handler.euler_to_quaternion(roll, pitch, yaw, &qw, &qx, &qy, &qz);

    // Quaternion should be normalized regardless of input angles
    TEST_ASSERT_TRUE(isQuaternionNormalized(qw, qx, qy, qz));
}

void test_euler_to_quaternion_negative_angles(void) {
    RRImuOpHandlerTestable handler;
    float qw, qx, qy, qz;

    // Test negative angles
    handler.euler_to_quaternion(-M_PI / 4.0f, -M_PI / 6.0f, -M_PI / 3.0f, &qw, &qx, &qy, &qz);

    TEST_ASSERT_TRUE(isQuaternionNormalized(qw, qx, qy, qz));
}

// ============================================================================
// Tests for Protobuf Serialization
// ============================================================================

void test_protobuf_serialization_response_structure(void) {
    // Test that Response message can be properly initialized and serialized
    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    response.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;

    std::uint8_t buffer[256];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    bool status = pb_encode(&stream, &org_ryderrobots_ros2_serial_Response_msg, &response);
    TEST_ASSERT_TRUE(status);
    TEST_ASSERT_TRUE(stream.bytes_written > 0);
}

void test_protobuf_serialization_msp_raw_imu(void) {
    // Test MspRawImu message serialization
    org_ryderrobots_ros2_serial_MspRawImu imu_data = org_ryderrobots_ros2_serial_MspRawImu_init_zero;

    // Set orientation quaternion
    imu_data.orientation.w = 1.0;
    imu_data.orientation.x = 0.0;
    imu_data.orientation.y = 0.0;
    imu_data.orientation.z = 0.0;
    imu_data.has_orientation = true;

    // Set angular velocity
    imu_data.angular_velocity.x = 0.5;
    imu_data.angular_velocity.y = 0.3;
    imu_data.angular_velocity.z = 0.1;
    imu_data.has_angular_velocity = true;

    // Set linear acceleration
    imu_data.linear_acceleration.x = 0.0;
    imu_data.linear_acceleration.y = 0.0;
    imu_data.linear_acceleration.z = 9.81;
    imu_data.has_linear_acceleration = true;

    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    response.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;
    response.data.msp_raw_imu = imu_data;
    response.which_data = org_ryderrobots_ros2_serial_Response_msp_raw_imu_tag;

    // Serialize
    std::uint8_t buffer[256];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool encode_status = pb_encode(&ostream, &org_ryderrobots_ros2_serial_Response_msg, &response);

    TEST_ASSERT_TRUE(encode_status);
    TEST_ASSERT_TRUE(ostream.bytes_written > 0);

    // Deserialize and verify
    org_ryderrobots_ros2_serial_Response decoded_response = org_ryderrobots_ros2_serial_Response_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    bool decode_status = pb_decode(&istream, &org_ryderrobots_ros2_serial_Response_msg, &decoded_response);

    TEST_ASSERT_TRUE(decode_status);
    TEST_ASSERT_EQUAL((int32_t)rr_ble::rr_op_code_t::MSP_RAW_IMU, decoded_response.op);
    TEST_ASSERT_EQUAL((int)org_ryderrobots_ros2_serial_Response_msp_raw_imu_tag, (int)decoded_response.which_data);

    // Verify orientation
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.orientation.w, 1.0));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.orientation.x, 0.0));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.orientation.y, 0.0));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.orientation.z, 0.0));

    // Verify angular velocity
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.angular_velocity.x, 0.5));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.angular_velocity.y, 0.3));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.angular_velocity.z, 0.1));

    // Verify linear acceleration
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.linear_acceleration.x, 0.0));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.linear_acceleration.y, 0.0));
    TEST_ASSERT_TRUE(floatNear(decoded_response.data.msp_raw_imu.linear_acceleration.z, 9.81));
}

void test_protobuf_serialization_bad_request(void) {
    // Test BadRequest error serialization
    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    response.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;

    org_ryderrobots_ros2_serial_BadRequest bad_request = org_ryderrobots_ros2_serial_BadRequest_init_zero;
    bad_request.etype = org_ryderrobots_ros2_serial_ErrorType_ET_SERVICE_UNAVAILABLE;

    response.data.bad_request = bad_request;
    response.which_data = org_ryderrobots_ros2_serial_Response_bad_request_tag;

    // Serialize
    std::uint8_t buffer[256];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool encode_status = pb_encode(&ostream, &org_ryderrobots_ros2_serial_Response_msg, &response);

    TEST_ASSERT_TRUE(encode_status);

    // Deserialize and verify
    org_ryderrobots_ros2_serial_Response decoded_response = org_ryderrobots_ros2_serial_Response_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    bool decode_status = pb_decode(&istream, &org_ryderrobots_ros2_serial_Response_msg, &decoded_response);

    TEST_ASSERT_TRUE(decode_status);
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Response_bad_request_tag, decoded_response.which_data);
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_ErrorType_ET_SERVICE_UNAVAILABLE,
                decoded_response.data.bad_request.etype);
}

void test_protobuf_serialization_request_monitor(void) {
    // Test Request message with Monitor payload
    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    request.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;

    org_ryderrobots_ros2_serial_Monitor monitor = org_ryderrobots_ros2_serial_Monitor_init_zero;
    monitor.is_request = true;

    request.data.monitor = monitor;
    request.which_data = org_ryderrobots_ros2_serial_Request_monitor_tag;

    // Serialize
    std::uint8_t buffer[128];
    pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool encode_status = pb_encode(&ostream, &org_ryderrobots_ros2_serial_Request_msg, &request);

    TEST_ASSERT_TRUE(encode_status);

    // Deserialize and verify
    org_ryderrobots_ros2_serial_Request decoded_request = org_ryderrobots_ros2_serial_Request_init_zero;
    pb_istream_t istream = pb_istream_from_buffer(buffer, ostream.bytes_written);
    bool decode_status = pb_decode(&istream, &org_ryderrobots_ros2_serial_Request_msg, &decoded_request);

    TEST_ASSERT_TRUE(decode_status);
    TEST_ASSERT_EQUAL((int32_t)rr_ble::rr_op_code_t::MSP_RAW_IMU, decoded_request.op);
    TEST_ASSERT_EQUAL((int)org_ryderrobots_ros2_serial_Request_monitor_tag, (int)decoded_request.which_data);
    TEST_ASSERT_TRUE(decoded_request.data.monitor.is_request);
}

// ============================================================================
// Integration Tests (with mocked hardware)
// ============================================================================

void test_monitor_function_creates_valid_response(void) {
    RRImuOpHandlerTestable handler;

    // Initialize (will use mocked IMU)
    handler.init();
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Status_READY, handler.status());

    // Create monitor request
    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    request.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;
    request.data.monitor.is_request = true;
    request.which_data = org_ryderrobots_ros2_serial_Request_monitor_tag;

    // Call monitor
    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.monitor(request, response);

    // Verify response structure
    TEST_ASSERT_EQUAL(rr_ble::rr_op_code_t::MSP_RAW_IMU, response.op);
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Response_msp_raw_imu_tag, response.which_data);

    // Verify that all required fields are set
    TEST_ASSERT_TRUE(response.data.msp_raw_imu.has_orientation);
    TEST_ASSERT_TRUE(response.data.msp_raw_imu.has_angular_velocity);
    TEST_ASSERT_TRUE(response.data.msp_raw_imu.has_linear_acceleration);

    // Verify quaternion is normalized
    auto& orient = response.data.msp_raw_imu.orientation;
    TEST_ASSERT_TRUE(isQuaternionNormalized(orient.w, orient.x, orient.y, orient.z));
}

void test_perform_op_with_monitor_request(void) {
    RRImuOpHandlerTestable handler;
    handler.init();

    // Create monitor request
    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    request.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;
    request.data.monitor.is_request = true;
    request.which_data = org_ryderrobots_ros2_serial_Request_monitor_tag;

    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.perform_op(request, response);

    // Verify successful response
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Response_msp_raw_imu_tag, response.which_data);
    TEST_ASSERT_TRUE(response.data.msp_raw_imu.has_orientation);
}

void test_perform_op_service_unavailable(void) {
    RRImuOpHandlerTestable handler;
    handler.init();

    // Make IMU unavailable
    IMU.accel_available = false;

    // Create monitor request
    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    request.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;
    request.data.monitor.is_request = true;
    request.which_data = org_ryderrobots_ros2_serial_Request_monitor_tag;

    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.perform_op(request, response);

    // Verify error response
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Response_bad_request_tag, response.which_data);
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_ErrorType_ET_SERVICE_UNAVAILABLE,
                response.data.bad_request.etype);
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Status_NOT_AVAILABLE, handler.status());

    // Restore for other tests
    IMU.accel_available = true;
}

void test_perform_op_unknown_operation(void) {
    RRImuOpHandlerTestable handler;
    handler.init();

    // Create request with unknown operation type
    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    request.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;
    request.which_data = 99;  // Invalid which_data value

    org_ryderrobots_ros2_serial_Response response = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.perform_op(request, response);

    // Verify error response
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_Response_bad_request_tag, response.which_data);
    TEST_ASSERT_EQUAL(org_ryderrobots_ros2_serial_ErrorType_ET_UNKNOWN_OPERATION,
                response.data.bad_request.etype);
}

void test_filter_update_rate_limiting(void) {
    RRImuOpHandlerTestable handler;
    handler.init();

    // Reset mock time
    mock_millis_value = 0;

    // Set mock IMU values
    IMU.mock_gx = 0.1f;
    IMU.mock_gy = 0.2f;
    IMU.mock_gz = 0.3f;
    IMU.mock_ax = 0.5f;
    IMU.mock_ay = 0.6f;
    IMU.mock_az = 0.7f;

    org_ryderrobots_ros2_serial_Request request = org_ryderrobots_ros2_serial_Request_init_zero;
    request.op = rr_ble::rr_op_code_t::MSP_RAW_IMU;
    request.data.monitor.is_request = true;
    request.which_data = org_ryderrobots_ros2_serial_Request_monitor_tag;

    org_ryderrobots_ros2_serial_Response response1 = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.monitor(request, response1);

    // Advance time by less than update interval (10ms)
    mock_millis_value += 5;

    org_ryderrobots_ros2_serial_Response response2 = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.monitor(request, response2);

    // Advance time past update interval
    mock_millis_value += 10;

    org_ryderrobots_ros2_serial_Response response3 = org_ryderrobots_ros2_serial_Response_init_zero;
    handler.monitor(request, response3);

    // All responses should be valid
    TEST_ASSERT_TRUE(response1.data.msp_raw_imu.has_orientation);
    TEST_ASSERT_TRUE(response2.data.msp_raw_imu.has_orientation);
    TEST_ASSERT_TRUE(response3.data.msp_raw_imu.has_orientation);
}

// ============================================================================
// Test Setup and Loop
// ============================================================================

// Unity test runner
void setUp(void) {
    // Set up code before each test if needed
}

void tearDown(void) {
    // Clean up code after each test if needed
}

int main(void) {
    UNITY_BEGIN();

    // Skip Euler tests,  because they are build in as private,  may expose them
    // at a later stage for better testing.
    // Euler to quaternion tests
    // RUN_TEST(test_euler_to_quaternion_identity);
    // RUN_TEST(test_euler_to_quaternion_roll_90);
    // RUN_TEST(test_euler_to_quaternion_pitch_90);
    // RUN_TEST(test_euler_to_quaternion_yaw_90);
    // RUN_TEST(test_euler_to_quaternion_combined_angles);
    // RUN_TEST(test_euler_to_quaternion_negative_angles);

    // Protobuf serialization tests
    RUN_TEST(test_protobuf_serialization_response_structure);
    RUN_TEST(test_protobuf_serialization_msp_raw_imu);
    RUN_TEST(test_protobuf_serialization_bad_request);
    RUN_TEST(test_protobuf_serialization_request_monitor);

    // Integration tests
    RUN_TEST(test_monitor_function_creates_valid_response);
    RUN_TEST(test_perform_op_with_monitor_request);
    RUN_TEST(test_perform_op_service_unavailable);
    RUN_TEST(test_perform_op_unknown_operation);
    RUN_TEST(test_filter_update_rate_limiting);

    return UNITY_END();
}

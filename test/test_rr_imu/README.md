# rr_imu Unit Tests

This directory contains comprehensive unit tests for the `rr_imu` library, including tests for:

- **Euler to Quaternion Conversion**: Tests the mathematical correctness of the `euler_to_quaternion` function
- **Protobuf Serialization**: Tests serialization and deserialization of IMU data using nanopb
- **Integration Tests**: Tests the full operation handler workflow with mocked hardware

## Test Architecture

The tests are designed to run in both environments:
1. **Native Environment** (local PC): Uses mocked Arduino hardware dependencies
2. **Embedded Environment** (Arduino hardware): Uses real hardware (integration tests disabled)

### Mocked Dependencies

When running on native environment (`#ifndef ARDUINO`), the following are mocked:

- `Arduino.h` - Core Arduino functions (millis, delay, Serial)
- `Arduino_BMI270_BMM150.h` - IMU sensor library
- `MadgwickAHRS.h` - Madgwick filter library

Mock implementations are included directly in the test file and provide:
- Controllable sensor readings for deterministic testing
- Time control via `mock_millis_value`
- Sensor availability flags for testing error conditions

## Running the Tests

### On Native Environment (Local PC)

```bash
# Run all native tests
pio test -e native

# Run only rr_imu tests
pio test -e native -f test_rr_imu

# Run with verbose output
pio test -e native -f test_rr_imu -v
```

### On Embedded Hardware

```bash
# Run on nano33ble
pio test -e nano33ble -f test_rr_imu

# Upload and monitor
pio test -e nano33ble -f test_rr_imu --upload-port /dev/ttyACM0
```

## Test Coverage

### Euler to Quaternion Tests

- `euler_to_quaternion_identity`: Tests identity rotation (0, 0, 0)
- `euler_to_quaternion_roll_90`: Tests 90-degree roll
- `euler_to_quaternion_pitch_90`: Tests 90-degree pitch
- `euler_to_quaternion_yaw_90`: Tests 90-degree yaw
- `euler_to_quaternion_combined_angles`: Tests combined rotations
- `euler_to_quaternion_negative_angles`: Tests negative angle inputs

### Protobuf Serialization Tests

- `protobuf_serialization_response_structure`: Tests basic Response message structure
- `protobuf_serialization_msp_raw_imu`: Tests full IMU data serialization/deserialization
- `protobuf_serialization_bad_request`: Tests error response serialization
- `protobuf_serialization_request_monitor`: Tests monitor request serialization

### Integration Tests (Native Only)

- `monitor_function_creates_valid_response`: Tests monitor request handling
- `perform_op_with_monitor_request`: Tests operation routing
- `perform_op_service_unavailable`: Tests error handling when IMU unavailable
- `perform_op_unknown_operation`: Tests handling of unknown operations
- `filter_update_rate_limiting`: Tests filter update rate limiting (100Hz)

## Test Utilities

### Helper Functions

- `floatNear(a, b, epsilon)`: Compares floating-point values with tolerance
- `isQuaternionNormalized(qw, qx, qy, qz)`: Verifies quaternion normalization

### Mock Control Variables

```cpp
// IMU Mock Control
IMU.accel_available = true/false;
IMU.gyro_available = true/false;
IMU.mock_ax, mock_ay, mock_az = ...;  // Acceleration values
IMU.mock_gx, mock_gy, mock_gz = ...;  // Gyroscope values

// Time Control
mock_millis_value = 0;  // Set current time in milliseconds
```

## Adding New Tests

1. Add your test function using the `test(test_name)` macro
2. Use AUnit assertions: `assertTrue`, `assertEqual`, `assertNear`
3. For native-only tests, wrap in `#ifndef ARDUINO ... #endif`
4. Update this README with test description

## Dependencies

- **AUnit**: Arduino unit testing framework
- **Nanopb**: Protocol buffer library for embedded systems
- **rr_imu**: The library under test
- **mb_operations**: Operation handler interface

## Notes

- Tests automatically verify quaternion normalization after conversions
- Protobuf tests verify both encoding and decoding paths
- Integration tests reset mock state between runs
- All tests are isolated and can run independently

#!/usr/bin/env python3
# Copyright (c) 2025 Ryder Robots
# Simple connection test script

"""
Quick test script to verify USB serial connection to mousebot.

Usage:
    ./test_connection.py /dev/ttyACM0
"""

import sys
import time
from pathlib import Path

# Add proto to path
SCRIPT_DIR = Path(__file__).parent
PROTO_DIR = SCRIPT_DIR.parent / "proto"
sys.path.insert(0, str(PROTO_DIR))

try:
    import serial
except ImportError:
    print("ERROR: pyserial not installed")
    print("Install: pip3 install pyserial")
    sys.exit(1)

try:
    import rr_serial_pb2 as pb
except ImportError:
    print("ERROR: Protobuf files not generated")
    print("Run: ./setup_proto.sh")
    sys.exit(1)


def test_connection(port="/dev/ttyACM0"):
    """Test basic serial connection and single IMU request"""
    print(f"Testing connection to {port}...")
    print("-" * 60)

    try:
        # Open serial port
        ser = serial.Serial(
            port=port,
            baudrate=115200,
            timeout=2.0
        )
        print(f"✓ Opened {port} at 115200 baud")

        # Wait for Arduino to reset
        print("  Waiting for Arduino to reset...")
        time.sleep(2)

        # Create IMU request
        request = pb.Request()
        request.op = 102  # MSP_RAW_IMU
        request.monitor.is_request = True

        # Send request
        data = request.SerializeToString()
        ser.write(data)
        ser.write(bytes([0x1E]))  # Terminator
        ser.flush()
        print(f"✓ Sent IMU request ({len(data)} bytes)")

        # Receive response
        response_data = bytearray()
        start = time.time()

        while True:
            if time.time() - start > 2.0:
                print("✗ TIMEOUT: No response received")
                return False

            if ser.in_waiting > 0:
                byte = ser.read(1)
                if byte[0] == 0x1E:
                    break
                response_data.append(byte[0])

        print(f"✓ Received response ({len(response_data)} bytes)")

        # Parse response
        response = pb.Response()
        response.ParseFromString(bytes(response_data))

        print(f"✓ Parsed protobuf response")
        print(f"  Op Code: {response.op}")

        # Check for errors
        if response.HasField('bad_request'):
            error_names = {
                0: "ET_UNKNOWN",
                1: "ET_MAX_LEN_EXCEED",
                2: "ET_SERVICE_UNAVAILABLE",
                3: "ET_UNKNOWN_OPERATION",
                4: "ET_INVALID_REQUEST",
                5: "ET_SERIAL_FAILURE"
            }
            error = error_names.get(response.bad_request.etype, "UNKNOWN")
            print(f"✗ ERROR RESPONSE: {error}")
            return False

        # Check IMU data
        if response.HasField('msp_raw_imu'):
            imu = response.msp_raw_imu
            print(f"✓ IMU data received:")
            if imu.HasField('orientation'):
                q = imu.orientation
                print(f"    Quaternion: w={q.w:.4f}, x={q.x:.4f}, y={q.y:.4f}, z={q.z:.4f}")
            if imu.HasField('angular_velocity'):
                av = imu.angular_velocity
                print(f"    Angular Vel: x={av.x:.4f}, y={av.y:.4f}, z={av.z:.4f} rad/s")
            if imu.HasField('linear_acceleration'):
                la = imu.linear_acceleration
                print(f"    Linear Accel: x={la.x:.4f}, y={la.y:.4f}, z={la.z:.4f} g")

        ser.close()
        print("-" * 60)
        print("✓ TEST PASSED: Connection successful!")
        return True

    except serial.SerialException as e:
        print(f"✗ Serial error: {e}")
        print("-" * 60)
        print("✗ TEST FAILED")
        return False
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        print("-" * 60)
        print("✗ TEST FAILED")
        return False


if __name__ == '__main__':
    port = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyACM0"
    success = test_connection(port)
    sys.exit(0 if success else 1)

#!/usr/bin/env python3
# Copyright (c) 2025 Ryder Robots
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""
Mousebot Serial Client - USB Communication Tool

This script communicates with the Arduino Nano BLE 33 mousebot via USB serial.
It sends protobuf-encoded requests and decodes responses.

Requirements:
    pip install pyserial protobuf

Usage:
    # Request IMU data
    ./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu

    # Continuous IMU monitoring at 10Hz
    ./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu --rate 10

    # Request feature list
    ./mousebot_serial_client.py --port /dev/ttyACM0 --operation features

    # Raw command
    ./mousebot_serial_client.py --port /dev/ttyACM0 --op-code 102
"""

import sys
import time
import argparse
import serial
from pathlib import Path

# Add proto directory to path for imports
SCRIPT_DIR = Path(__file__).parent
PROTO_DIR = SCRIPT_DIR.parent / "proto"
sys.path.insert(0, str(PROTO_DIR))

# Import generated protobuf messages
# Note: You need to generate these using protoc:
#   protoc --python_out=. proto/rr_serial.proto
try:
    import rr_serial_pb2 as pb
except ImportError:
    print("ERROR: Could not import rr_serial_pb2")
    print("Generate protobuf files first:")
    print("  cd proto/")
    print("  protoc --python_out=. rr_serial.proto")
    sys.exit(1)


# Constants from rr_ble.hpp
class OpCodes:
    """MSP Operation Codes"""
    MSP_RAW_IMU = 102
    MSP_MOTOR = 104
    MSP_RAW_SENSORS = 105
    MSP_SET_RAW_RC = 200
    BAD_REQUEST = 400


TERM_CHAR = 0x1E  # Record separator character
BAUD_RATE = 115200
TIMEOUT = 2.0  # Serial timeout in seconds


class MousebotClient:
    """Serial communication client for Arduino mousebot"""

    def __init__(self, port="/dev/ttyACM0", baudrate=BAUD_RATE, timeout=TIMEOUT):
        """
        Initialize serial connection to mousebot

        Args:
            port: Serial port path (default: /dev/ttyACM0)
            baudrate: Baud rate (default: 115200)
            timeout: Read timeout in seconds
        """
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.ser = None

    def connect(self):
        """Open serial connection"""
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE
            )
            # Wait for Arduino to reset after serial connection
            time.sleep(2)
            print(f"Connected to {self.port} at {self.baudrate} baud")
            return True
        except serial.SerialException as e:
            print(f"ERROR: Could not open {self.port}: {e}")
            return False

    def disconnect(self):
        """Close serial connection"""
        if self.ser and self.ser.is_open:
            self.ser.close()
            print("Disconnected")

    def send_request(self, request):
        """
        Send protobuf request to mousebot

        Args:
            request: Request protobuf message

        Returns:
            bool: True if sent successfully
        """
        if not self.ser or not self.ser.is_open:
            print("ERROR: Serial port not open")
            return False

        try:
            # Serialize request
            data = request.SerializeToString()

            # Send data + terminator
            self.ser.write(data)
            self.ser.write(bytes([TERM_CHAR]))
            self.ser.flush()

            return True
        except Exception as e:
            print(f"ERROR sending request: {e}")
            return False

    def receive_response(self):
        """
        Receive and decode protobuf response

        Returns:
            Response protobuf message or None on error
        """
        if not self.ser or not self.ser.is_open:
            print("ERROR: Serial port not open")
            return None

        try:
            # Read until terminator character
            data = bytearray()
            start_time = time.time()

            while True:
                if time.time() - start_time > self.timeout:
                    print("ERROR: Response timeout")
                    return None

                if self.ser.in_waiting > 0:
                    byte = self.ser.read(1)
                    if not byte:
                        continue

                    if byte[0] == TERM_CHAR:
                        break

                    data.append(byte[0])

            if len(data) == 0:
                print("ERROR: Empty response")
                return None

            # Deserialize response
            response = pb.Response()
            response.ParseFromString(bytes(data))

            return response

        except Exception as e:
            print(f"ERROR receiving response: {e}")
            return None

    def request_imu(self):
        """
        Request IMU data (MSP_RAW_IMU)

        Returns:
            Response message or None
        """
        request = pb.Request()
        request.op = OpCodes.MSP_RAW_IMU
        request.monitor.is_request = True

        if self.send_request(request):
            return self.receive_response()
        return None

    def request_features(self):
        """
        Request feature list (MSP_IDENT)

        Note: This operation is not yet implemented in the firmware

        Returns:
            Response message or None
        """
        request = pb.Request()
        request.op = 100  # MSP_IDENT (not yet in OpCodes enum)
        request.monitor.is_request = True

        if self.send_request(request):
            return self.receive_response()
        return None

    def send_raw_opcode(self, op_code):
        """
        Send raw operation code with monitor request

        Args:
            op_code: Integer operation code

        Returns:
            Response message or None
        """
        request = pb.Request()
        request.op = op_code
        request.monitor.is_request = True

        if self.send_request(request):
            return self.receive_response()
        return None


def print_imu_response(response):
    """Pretty print IMU response data"""
    if not response:
        print("No response received")
        return

    print(f"\n{'='*60}")
    print(f"Response Op Code: {response.op}")

    # Check for error
    if response.HasField('bad_request'):
        error_types = {
            0: "ET_UNKNOWN",
            1: "ET_MAX_LEN_EXCEED",
            2: "ET_SERVICE_UNAVAILABLE",
            3: "ET_UNKNOWN_OPERATION",
            4: "ET_INVALID_REQUEST",
            5: "ET_SERIAL_FAILURE"
        }
        error_name = error_types.get(response.bad_request.etype, "UNKNOWN")
        print(f"ERROR: {error_name} ({response.bad_request.etype})")
        print(f"{'='*60}\n")
        return

    # Print IMU data
    if response.HasField('msp_raw_imu'):
        imu = response.msp_raw_imu

        print("\nOrientation (Quaternion):")
        if imu.HasField('orientation'):
            q = imu.orientation
            print(f"  x: {q.x:+.6f}")
            print(f"  y: {q.y:+.6f}")
            print(f"  z: {q.z:+.6f}")
            print(f"  w: {q.w:+.6f}")

        print("\nAngular Velocity (rad/s):")
        if imu.HasField('angular_velocity'):
            av = imu.angular_velocity
            print(f"  x: {av.x:+.6f}")
            print(f"  y: {av.y:+.6f}")
            print(f"  z: {av.z:+.6f}")

        print("\nLinear Acceleration (g):")
        if imu.HasField('linear_acceleration'):
            la = imu.linear_acceleration
            print(f"  x: {la.x:+.6f}")
            print(f"  y: {la.y:+.6f}")
            print(f"  z: {la.z:+.6f}")

    print(f"{'='*60}\n")


def main():
    parser = argparse.ArgumentParser(
        description="Mousebot Serial Client - Communicate with Arduino via USB",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s --port /dev/ttyACM0 --operation imu
  %(prog)s --port /dev/ttyACM0 --operation imu --rate 10
  %(prog)s --port /dev/ttyACM0 --op-code 102
        """
    )

    parser.add_argument(
        '--port', '-p',
        default='/dev/ttyACM0',
        help='Serial port (default: /dev/ttyACM0)'
    )

    parser.add_argument(
        '--baudrate', '-b',
        type=int,
        default=BAUD_RATE,
        help=f'Baud rate (default: {BAUD_RATE})'
    )

    parser.add_argument(
        '--operation', '-o',
        choices=['imu', 'features'],
        help='Predefined operation (imu, features)'
    )

    parser.add_argument(
        '--op-code',
        type=int,
        help='Raw operation code to send'
    )

    parser.add_argument(
        '--rate', '-r',
        type=float,
        help='Continuous monitoring rate in Hz (e.g., 10 for 10Hz)'
    )

    parser.add_argument(
        '--timeout', '-t',
        type=float,
        default=TIMEOUT,
        help=f'Serial timeout in seconds (default: {TIMEOUT})'
    )

    args = parser.parse_args()

    # Validate arguments
    if not args.operation and args.op_code is None:
        parser.error("Must specify either --operation or --op-code")

    # Create client and connect
    client = MousebotClient(
        port=args.port,
        baudrate=args.baudrate,
        timeout=args.timeout
    )

    if not client.connect():
        return 1

    try:
        if args.rate:
            # Continuous mode
            interval = 1.0 / args.rate
            print(f"Monitoring at {args.rate} Hz (Press Ctrl+C to stop)\n")

            while True:
                start = time.time()

                if args.operation == 'imu':
                    response = client.request_imu()
                    print_imu_response(response)
                elif args.op_code is not None:
                    response = client.send_raw_opcode(args.op_code)
                    print_imu_response(response)

                # Maintain rate
                elapsed = time.time() - start
                sleep_time = max(0, interval - elapsed)
                time.sleep(sleep_time)
        else:
            # Single request mode
            if args.operation == 'imu':
                response = client.request_imu()
                print_imu_response(response)
            elif args.operation == 'features':
                response = client.request_features()
                print_imu_response(response)
            elif args.op_code is not None:
                response = client.send_raw_opcode(args.op_code)
                print_imu_response(response)

    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
    finally:
        client.disconnect()

    return 0


if __name__ == '__main__':
    sys.exit(main())

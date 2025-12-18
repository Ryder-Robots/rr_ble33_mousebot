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
Mousebot ROS2 Client - Topic-based Communication Tool

This script communicates with the Arduino Nano BLE 33 mousebot via ROS2 topics.
It sends protobuf-encoded requests to /serial_write and receives responses from /serial_read.

Requirements:
    pip install protobuf
    ROS2 installation with std_msgs

Usage:
    # Request IMU data
    ./mousebot_ros2_client.py --operation imu

    # Continuous IMU monitoring at 10Hz
    ./mousebot_ros2_client.py --operation imu --rate 10

    # Request feature list
    ./mousebot_ros2_client.py --operation features

    # Raw command
    ./mousebot_ros2_client.py --op-code 102
"""

import sys
import time
import argparse
from pathlib import Path

import rclpy
from rclpy.node import Node
from std_msgs.msg import UInt8MultiArray

# Add proto directory to path for imports
SCRIPT_DIR = Path(__file__).parent
PROTO_DIR = SCRIPT_DIR.parent / "proto"
sys.path.insert(0, str(PROTO_DIR))

# Import generated protobuf messages
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
RESPONSE_TIMEOUT = 2.0  # Timeout for receiving response in seconds


class MousebotROS2Client(Node):
    """ROS2 topic-based communication client for Arduino mousebot"""

    def __init__(self):
        super().__init__('mousebot_ros2_client')

        # Publisher for sending messages to serial port
        self.publisher = self.create_publisher(
            UInt8MultiArray,
            '/serial_write',
            10
        )

        # Subscriber for receiving messages from serial port
        self.subscription = self.create_subscription(
            UInt8MultiArray,
            '/serial_read',
            self.serial_read_callback,
            10
        )

        # Response tracking
        self.response_data = bytearray()
        self.response_received = False
        self.response_complete = False

        self.get_logger().info('Mousebot ROS2 Client initialized')
        self.get_logger().info('Publishing to: /serial_write')
        self.get_logger().info('Subscribed to: /serial_read')

    def serial_read_callback(self, msg):
        """
        Callback for messages received from /serial_read topic

        Args:
            msg: UInt8MultiArray message containing serial data
        """
        # Accumulate data until we receive the terminator character
        for byte in msg.data:
            if byte == TERM_CHAR:
                self.response_complete = True
                self.response_received = True
                break
            self.response_data.append(byte)

    def send_request(self, request):
        """
        Send protobuf request via /serial_write topic

        Args:
            request: Request protobuf message

        Returns:
            bool: True if sent successfully
        """
        try:
            # Serialize request
            data = request.SerializeToString()

            # Create UInt8MultiArray message with data + terminator
            msg = UInt8MultiArray()
            msg.data = list(data) + [TERM_CHAR]

            # Publish to /serial_write topic
            self.publisher.publish(msg)
            self.get_logger().debug(f'Sent {len(msg.data)} bytes to /serial_write')

            return True
        except Exception as e:
            self.get_logger().error(f'Error sending request: {e}')
            return False

    def receive_response(self, timeout=RESPONSE_TIMEOUT):
        """
        Wait for and decode protobuf response from /serial_read topic

        Args:
            timeout: Maximum time to wait for response in seconds

        Returns:
            Response protobuf message or None on error
        """
        # Reset response state
        self.response_data = bytearray()
        self.response_received = False
        self.response_complete = False

        start_time = time.time()

        # Spin until we receive a complete response or timeout
        while not self.response_complete:
            if time.time() - start_time > timeout:
                self.get_logger().error('Response timeout')
                return None

            # Process callbacks
            rclpy.spin_once(self, timeout_sec=0.1)

        if len(self.response_data) == 0:
            self.get_logger().error('Empty response received')
            return None

        try:
            # Deserialize response
            response = pb.Response()
            response.ParseFromString(bytes(self.response_data))
            return response
        except Exception as e:
            self.get_logger().error(f'Error decoding response: {e}')
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


def print_imu_response(response, logger):
    """
    Pretty print IMU response data

    Args:
        response: Response protobuf message
        logger: ROS2 logger instance
    """
    if not response:
        logger.info("No response received")
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


def main(args=None):
    parser = argparse.ArgumentParser(
        description="Mousebot ROS2 Client - Communicate with Arduino via ROS2 topics",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s --operation imu
  %(prog)s --operation imu --rate 10
  %(prog)s --op-code 102
        """
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
        default=RESPONSE_TIMEOUT,
        help=f'Response timeout in seconds (default: {RESPONSE_TIMEOUT})'
    )

    cli_args = parser.parse_args()

    # Validate arguments
    if not cli_args.operation and cli_args.op_code is None:
        parser.error("Must specify either --operation or --op-code")

    # Initialize ROS2
    rclpy.init(args=args)

    # Create client node
    client = MousebotROS2Client()

    # Give some time for publishers/subscribers to connect
    time.sleep(0.5)

    try:
        if cli_args.rate:
            # Continuous mode
            interval = 1.0 / cli_args.rate
            client.get_logger().info(f"Monitoring at {cli_args.rate} Hz (Press Ctrl+C to stop)\n")

            while rclpy.ok():
                start = time.time()

                if cli_args.operation == 'imu':
                    response = client.request_imu()
                    print_imu_response(response, client.get_logger())
                elif cli_args.op_code is not None:
                    response = client.send_raw_opcode(cli_args.op_code)
                    print_imu_response(response, client.get_logger())

                # Maintain rate
                elapsed = time.time() - start
                sleep_time = max(0, interval - elapsed)
                time.sleep(sleep_time)
        else:
            # Single request mode
            if cli_args.operation == 'imu':
                response = client.request_imu()
                print_imu_response(response, client.get_logger())
            elif cli_args.operation == 'features':
                response = client.request_features()
                print_imu_response(response, client.get_logger())
            elif cli_args.op_code is not None:
                response = client.send_raw_opcode(cli_args.op_code)
                print_imu_response(response, client.get_logger())

    except KeyboardInterrupt:
        client.get_logger().info("\nInterrupted by user")
    finally:
        client.destroy_node()
        rclpy.shutdown()

    return 0


if __name__ == '__main__':
    sys.exit(main())

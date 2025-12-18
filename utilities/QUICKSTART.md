# Quick Start Guide

Get up and running with mousebot USB serial communication in 5 minutes.

## Step 1: Install Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install python3-pip protobuf-compiler
pip3 install -r requirements.txt

# macOS
brew install protobuf
pip3 install -r requirements.txt
```

## Step 2: Generate Protobuf Files

```bash
./setup_proto.sh
```

**Expected output:**
```
Generating Python protobuf files...
Proto directory: /path/to/proto
✓ Generated rr_serial_pb2.py successfully
```

## Step 3: Set Serial Port Permissions (Linux only)

```bash
# Find your device
ls -l /dev/ttyACM*

# Set permissions
sudo chmod 666 /dev/ttyACM0

# OR add yourself to dialout group (permanent)
sudo usermod -a -G dialout $USER
# Then log out and back in
```

## Step 4: Test Connection

```bash
./test_connection.py /dev/ttyACM0
```

**Expected output:**
```
Testing connection to /dev/ttyACM0...
------------------------------------------------------------
✓ Opened /dev/ttyACM0 at 115200 baud
  Waiting for Arduino to reset...
✓ Sent IMU request (4 bytes)
✓ Received response (76 bytes)
✓ Parsed protobuf response
  Op Code: 102
✓ IMU data received:
    Quaternion: w=0.9876, x=0.0123, y=-0.0234, z=0.0345
    Angular Vel: x=0.0123, y=-0.0234, z=0.0345 rad/s
    Linear Accel: x=0.0012, y=0.0023, z=0.9987 g
------------------------------------------------------------
✓ TEST PASSED: Connection successful!
```

## Step 5: Request IMU Data

### Option A: Direct USB Serial Communication

```bash
# Single request
./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu

# Continuous at 10Hz
./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu --rate 10
```

### Option B: ROS2 Topic Communication

For ROS2 environments, use the topic-based client:

```bash
# Prerequisites: ROS2 must be running with a serial bridge node
# The bridge should publish to /serial_read and subscribe to /serial_write

# Single request
./mousebot_ros2_client.py --operation imu

# Continuous at 10Hz
./mousebot_ros2_client.py --operation imu --rate 10
```

**Note:** The ROS2 client requires:
- ROS2 installation with `std_msgs`
- A serial bridge node that forwards messages between `/serial_write`, `/serial_read` topics and the physical serial port
- Generated protobuf files (same as serial client)

## Troubleshooting

### Permission Denied

```bash
sudo chmod 666 /dev/ttyACM0
```

### Device Not Found

```bash
# List all USB serial devices
ls -l /dev/tty* | grep -i usb
ls -l /dev/tty* | grep -i acm
```

### Timeout / No Response

1. Check Arduino is powered and running firmware
2. Reset Arduino (unplug/replug USB)
3. Wait 2 seconds after connecting
4. Verify correct baud rate (115200)

### Import Error: rr_serial_pb2

```bash
./setup_proto.sh
```

## Next Steps

- See [README.md](README.md) for full documentation
- Check [mousebot_serial_client.py](mousebot_serial_client.py) for API usage
- Review [proto/rr_serial.proto](../proto/rr_serial.proto) for protocol details

## Common Commands

### Serial Client

```bash
# Get help
./mousebot_serial_client.py --help

# IMU at 100Hz (matches filter rate)
./mousebot_serial_client.py -p /dev/ttyACM0 -o imu -r 100

# Raw op code
./mousebot_serial_client.py -p /dev/ttyACM0 --op-code 102

# Different port (macOS)
./mousebot_serial_client.py -p /dev/tty.usbmodem14201 -o imu
```

### ROS2 Client

```bash
# Get help
./mousebot_ros2_client.py --help

# IMU at 100Hz (matches filter rate)
./mousebot_ros2_client.py -o imu -r 100

# Raw op code
./mousebot_ros2_client.py --op-code 102

# Features request
./mousebot_ros2_client.py -o features
```

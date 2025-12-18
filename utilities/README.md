# Mousebot Communication Utilities

Python tools for communicating with the Arduino Nano BLE 33 mousebot via USB serial or ROS2 topics.

## Setup

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install python3 python3-pip protobuf-compiler python3-protobuf python3.12-venv
python3 -m venv venv
source venv/bin/activate 
pip3 install pyserial protobuf
python utilities/protoc_prebuild.py
```

**macOS:**
```bash
brew install protobuf python3.12-venv
python3 -m venv venv
source venv/bin/activate 
pip3 install pyserial protobuf
python utilities/protoc_prebuild.py
```

### 2. Generate Protobuf Files

```bash
cd utilities/
./setup_proto.sh
```

This will generate `rr_serial_pb2.py` in the `proto/` directory.

### 3. Find Your Serial Port

**Linux:**
```bash
# List USB serial devices
ls -l /dev/ttyACM* /dev/ttyUSB*

# Common device: /dev/ttyACM0
```

**macOS:**
```bash
ls -l /dev/tty.usb*
```

### 4. Set Permissions (Linux only)

```bash
# Add your user to dialout group
sudo usermod -a -G dialout $USER

# Or set temporary permissions
sudo chmod 666 /dev/ttyACM0

# Then log out and back in
```

## Available Clients

This package provides two communication methods:

| Client | Communication Method | Use Case |
|--------|---------------------|----------|
| `mousebot_serial_client.py` | Direct USB serial | Standalone testing, debugging, direct hardware access |
| `mousebot_ros2_client.py` | ROS2 topics (`/serial_write`, `/serial_read`) | ROS2 integration, distributed systems, topic-based architecture |

## Usage

### Serial Client (Direct USB)

#### Basic IMU Request

Request IMU data once:

```bash
./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu
```

**Expected Output:**
```
Connected to /dev/ttyACM0 at 115200 baud

============================================================
Response Op Code: 102

Orientation (Quaternion):
  x: +0.123456
  y: -0.234567
  z: +0.345678
  w: +0.876543

Angular Velocity (rad/s):
  x: +0.012345
  y: -0.023456
  z: +0.034567

Linear Acceleration (g):
  x: +0.001234
  y: +0.002345
  z: +0.998765
============================================================

Disconnected
```

#### Continuous Monitoring

Monitor IMU at 10Hz (10 samples per second):

```bash
./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu --rate 10
```

Press `Ctrl+C` to stop.

#### Raw Operation Codes

Send raw MSP operation code:

```bash
# Request IMU data using raw op code
./mousebot_serial_client.py --port /dev/ttyACM0 --op-code 102
```

#### All Options

```bash
./mousebot_serial_client.py --help
```

**Options:**
- `--port`, `-p`: Serial port (default: `/dev/ttyACM0`)
- `--baudrate`, `-b`: Baud rate (default: `115200`)
- `--operation`, `-o`: Predefined operation (`imu`, `features`)
- `--op-code`: Raw operation code to send
- `--rate`, `-r`: Continuous monitoring rate in Hz
- `--timeout`, `-t`: Serial timeout in seconds (default: `2.0`)

## Operation Codes (MSP Protocol)

Currently implemented:

| Op Code | Constant        | Description          |
|---------|-----------------|----------------------|
| 102     | MSP_RAW_IMU     | IMU sensor data      |

Planned for future:

| Op Code | Constant        | Description          |
|---------|-----------------|----------------------|
| 104     | MSP_MOTOR       | Motor control/status |
| 105     | MSP_RAW_SENSORS | Range sensors        |
| 200     | MSP_SET_RAW_RC  | Set motor speeds     |

## Error Handling

The client will display error responses from the mousebot:

**Error Types:**
- `ET_UNKNOWN`: Unknown error
- `ET_MAX_LEN_EXCEED`: Request too large (exceeds buffer size)
- `ET_SERVICE_UNAVAILABLE`: Sensor/service not available
- `ET_UNKNOWN_OPERATION`: Operation code not recognized
- `ET_INVALID_REQUEST`: Malformed protobuf request
- `ET_SERIAL_FAILURE`: Communication failure

**Example Error:**
```
============================================================
Response Op Code: 102
ERROR: ET_SERVICE_UNAVAILABLE (2)
============================================================
```

This means the IMU sensor is not ready or failed initialization.

## Troubleshooting

### Serial Client Issues

#### "Could not open /dev/ttyACM0"

**Causes:**
1. Device not connected
2. Wrong port name
3. Permission denied

**Solutions:**
```bash
# Check if device exists
ls -l /dev/ttyACM*

# Check permissions
ls -l /dev/ttyACM0

# Add to dialout group (Ubuntu)
sudo usermod -a -G dialout $USER
# Log out and back in

# Or temporary fix
sudo chmod 666 /dev/ttyACM0
```

#### "Could not import rr_serial_pb2"

Run the setup script to generate protobuf files:
```bash
./setup_proto.sh
```

#### "Response timeout"

**Causes:**
1. Arduino not responding
2. Firmware not loaded
3. Wrong baud rate

**Solutions:**
```bash
# Increase timeout
./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu --timeout 5

# Check Arduino is running firmware (look for LED activity)
# Re-upload firmware via PlatformIO if needed
```

#### Empty or Corrupted Response

1. Reset the Arduino (disconnect/reconnect USB)
2. Wait 2 seconds after connecting before sending commands
3. Check baud rate matches firmware (115200)

### ROS2 Client Issues

#### "No response received" or timeout

**Causes:**
1. Serial bridge node not running
2. Topics not properly connected
3. Serial bridge not configured correctly

**Solutions:**
```bash
# Check if ROS2 topics exist
ros2 topic list | grep serial

# Check if serial bridge is publishing/subscribing
ros2 topic info /serial_read
ros2 topic info /serial_write

# Monitor topics for activity
ros2 topic echo /serial_read
ros2 topic echo /serial_write

# Verify bridge node is running
ros2 node list
```

#### Cannot import rclpy or std_msgs

**Solution:**
```bash
# Source ROS2 environment
source /opt/ros/<distro>/setup.bash

# For ROS2 Humble
source /opt/ros/humble/setup.bash

# For ROS2 Iron
source /opt/ros/iron/setup.bash

# Verify installation
python3 -c "import rclpy; import std_msgs"
```

#### Messages not being received by bridge

**Check message format:**
- Ensure the serial bridge expects `std_msgs::msg::UInt8MultiArray`
- Verify the bridge correctly handles the terminator character (0x1E)
- Check that the bridge is subscribing to `/serial_write` and publishing to `/serial_read`

**Debug with topic echo:**
```bash
# In one terminal, monitor what's being sent
ros2 topic echo /serial_write

# In another terminal, run the client
./mousebot_ros2_client.py --operation imu
```

## Python API Usage

You can also use the client in your own Python scripts:

```python
#!/usr/bin/env python3
from mousebot_serial_client import MousebotClient, print_imu_response

# Create client
client = MousebotClient(port="/dev/ttyACM0")

# Connect
if client.connect():
    # Request IMU data
    response = client.request_imu()
    print_imu_response(response)

    # Access data directly
    if response.HasField('msp_raw_imu'):
        imu = response.msp_raw_imu
        quat = imu.orientation
        print(f"Quaternion: w={quat.w}, x={quat.x}, y={quat.y}, z={quat.z}")

    client.disconnect()
```

## ROS2 Topic-Based Client

For ROS2 environments, use `mousebot_ros2_client.py` which communicates via topics instead of direct serial:

### Setup for ROS2

```bash
# Install ROS2 dependencies (if not already installed)
# Follow ROS2 installation instructions for your platform

# Ensure std_msgs is available
source /opt/ros/<distro>/setup.bash

# Generate protobuf files (same as serial client)
./setup_proto.sh
```

### Usage

The ROS2 client publishes to `/serial_write` and subscribes to `/serial_read`:

```bash
# Single IMU request
./mousebot_ros2_client.py --operation imu

# Continuous monitoring at 10Hz
./mousebot_ros2_client.py --operation imu --rate 10

# Raw operation code
./mousebot_ros2_client.py --op-code 102

# Get help
./mousebot_ros2_client.py --help
```

**Options:**
- `--operation`, `-o`: Predefined operation (`imu`, `features`)
- `--op-code`: Raw operation code to send
- `--rate`, `-r`: Continuous monitoring rate in Hz
- `--timeout`, `-t`: Response timeout in seconds (default: `2.0`)

### Prerequisites

The ROS2 client requires a serial bridge node that:
1. Subscribes to `/serial_write` topic (`std_msgs::msg::UInt8MultiArray`)
2. Forwards data to the physical serial port
3. Reads responses from serial port
4. Publishes to `/serial_read` topic (`std_msgs::msg::UInt8MultiArray`)

### Message Format

Messages on both topics use `std_msgs::msg::UInt8MultiArray`:
- **Data:** Serialized protobuf message bytes followed by terminator (`0x1E`)
- **Layout:** `[Protobuf bytes...][0x1E]`

### Integration Example

Using the serial client directly in ROS2 nodes:

```python
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu
from mousebot_serial_client import MousebotClient

class MousebotImuNode(Node):
    def __init__(self):
        super().__init__('mousebot_imu')
        self.publisher = self.create_publisher(Imu, 'imu/data', 10)
        self.client = MousebotClient(port="/dev/ttyACM0")
        self.client.connect()

        # Timer for 100Hz IMU updates
        self.timer = self.create_timer(0.01, self.timer_callback)

    def timer_callback(self):
        response = self.client.request_imu()
        if response and response.HasField('msp_raw_imu'):
            imu_msg = Imu()
            # Fill in IMU message from response
            # ... (convert protobuf to ROS2 message)
            self.publisher.publish(imu_msg)
```

## Protocol Details

### Communication Format

1. **Request:** `[Protobuf Request][0x1E]`
2. **Response:** `[Protobuf Response][0x1E]`

Where `0x1E` is the terminator character (ASCII Record Separator).

### Protobuf Schema

See [proto/rr_serial.proto](../proto/rr_serial.proto) for the complete protocol definition.

### Timing

- **Baud Rate:** 115200
- **Main Loop Throttle:** 5ms (200Hz max request rate)
- **IMU Filter Rate:** 100Hz
- **Recommended Request Rate:** ≤100Hz for IMU

## Quick Reference

### Command Comparison

| Task | Serial Client | ROS2 Client |
|------|--------------|-------------|
| Single IMU request | `./mousebot_serial_client.py -p /dev/ttyACM0 -o imu` | `./mousebot_ros2_client.py -o imu` |
| Continuous at 10Hz | `./mousebot_serial_client.py -p /dev/ttyACM0 -o imu -r 10` | `./mousebot_ros2_client.py -o imu -r 10` |
| Raw op code | `./mousebot_serial_client.py -p /dev/ttyACM0 --op-code 102` | `./mousebot_ros2_client.py --op-code 102` |
| Help | `./mousebot_serial_client.py --help` | `./mousebot_ros2_client.py --help` |

### Feature Comparison

| Feature | Serial Client | ROS2 Client |
|---------|--------------|-------------|
| Direct hardware access | ✓ | ✗ |
| Works without ROS2 | ✓ | ✗ |
| Requires serial permissions | ✓ | ✗ |
| Topic-based communication | ✗ | ✓ |
| Works in distributed ROS2 systems | ✗ | ✓ |
| Requires serial bridge | ✗ | ✓ |

## License

MIT License - Copyright (c) 2025 Ryder Robots

See [LICENSE](../LICENSE) for details.

#!/bin/bash
# Copyright (c) 2025 Ryder Robots
# Generate Python protobuf files for mousebot client

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
PROTO_DIR="$PROJECT_ROOT/proto"

echo "Generating Python protobuf files..."
echo "Proto directory: $PROTO_DIR"

# Check if protoc is installed
if ! command -v protoc &> /dev/null; then
    echo "ERROR: protoc (Protocol Buffer Compiler) not found"
    echo ""
    echo "Install on Ubuntu/Debian:"
    echo "  sudo apt-get install protobuf-compiler python3-protobuf"
    echo ""
    echo "Install on macOS:"
    echo "  brew install protobuf"
    echo ""
    exit 1
fi

# Generate Python protobuf files
cd "$PROTO_DIR"
protoc --python_out=. rr_serial.proto

if [ -f "rr_serial_pb2.py" ]; then
    echo "✓ Generated rr_serial_pb2.py successfully"
    ls -lh rr_serial_pb2.py
else
    echo "ERROR: Failed to generate protobuf files"
    exit 1
fi

echo ""
echo "Setup complete!"
echo ""
echo "Usage:"
echo "  cd $SCRIPT_DIR"
echo "  ./mousebot_serial_client.py --port /dev/ttyACM0 --operation imu"

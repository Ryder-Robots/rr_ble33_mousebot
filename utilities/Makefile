# Makefile for mousebot utilities
# Copyright (c) 2025 Ryder Robots

PYTHON := python3
PORT ?= /dev/ttyACM0
RATE ?= 10

.PHONY: help setup test imu monitor clean

help:
	@echo "Mousebot Utilities Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  setup    - Install dependencies and generate protobuf files"
	@echo "  test     - Test serial connection"
	@echo "  imu      - Request single IMU reading"
	@echo "  monitor  - Monitor IMU continuously at $(RATE)Hz"
	@echo "  clean    - Remove generated protobuf files"
	@echo ""
	@echo "Variables:"
	@echo "  PORT     - Serial port (default: $(PORT))"
	@echo "  RATE     - Monitoring rate in Hz (default: $(RATE))"
	@echo ""
	@echo "Examples:"
	@echo "  make setup"
	@echo "  make test PORT=/dev/ttyACM0"
	@echo "  make imu"
	@echo "  make monitor RATE=100"

setup:
	@echo "Installing Python dependencies..."
	$(PYTHON) -m pip install -r requirements.txt
	@echo ""
	@echo "Generating protobuf files..."
	./setup_proto.sh

test:
	@echo "Testing connection on $(PORT)..."
	./test_connection.py $(PORT)

imu:
	@echo "Requesting IMU data from $(PORT)..."
	./mousebot_serial_client.py --port $(PORT) --operation imu

monitor:
	@echo "Monitoring IMU at $(RATE)Hz on $(PORT)..."
	@echo "Press Ctrl+C to stop"
	./mousebot_serial_client.py --port $(PORT) --operation imu --rate $(RATE)

clean:
	@echo "Cleaning generated protobuf files..."
	rm -f ../proto/rr_serial_pb2.py
	rm -rf __pycache__
	@echo "Done"

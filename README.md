# RPI IGVC Emergency Stop

## DISCLAIMER!

This program is NOT intended for life-critical or industrial safety applications. This is meant for a college robotics team.

## Description

This is a lightweight Emergency Stop protocol for RPI's IGVC Emergency Stop. All code has been refactored to be more readable and structured in a set hierarchy. It uses FreeRTOS as a way to multiprocess and leave CPU overhead for important tasks such as sending Emergency Stop packets. This protocol ensures the Emergency Stop works as intended and as fast as possible.

## Hardware
- ESP32-S3 N16R8
- HC-12 Radio Modules
- I2C 128x64 OLED Screen

## Software Requirements

- VSCode PlatformIO
- Rhys Weatherley's arduinolibs Crypto library
- olikraus's u8g2 library

## Usage

This program uses FreeRTOS to multiprocess important and less important tasks: (Emergency Stop on Core 0, and Heartbeat packet and OLED on Core 1). On startup, the remote will attempt to pair with the robot using a Handshake packet. Once connected, the remote will continuously feed the robot heartbeat packets until the Emergency Stop is pressed. If the heartbeat packet feed is interrupted for any reason on the robot side, the robot will automatically trigger the Emergency Stop.

## License

MIT license

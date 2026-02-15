# Big Servo

ESP32 S3 WROOM servo controller that sweeps a servo 60 degrees and returns to start position.

## Hardware
- **Board:** ESP32 S3 WROOM
- **Pin:** GPIO 48
- **Component:** Servo motor

## Dependencies
- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) library

Install via Arduino IDE Library Manager:
1. Sketch → Include Library → Manage Libraries
2. Search "ESP32Servo"
3. Install by Kevin Harrington

## Behavior
- Starts at 0°
- Moves to 60°
- Returns to 0°
- Repeats with 500ms delays

## Configuration
Pulse width: 600-2300μs for accurate servo positioning

## Troubleshooting
**Servo not responding?** Ensure both the ESP32 and servo are connected to the power supply's ground (common ground required).

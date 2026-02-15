# HC-SR04 Ultrasonic Sensor

This sketch reads distance measurements from an HC-SR04 ultrasonic sensor and outputs the results to the serial monitor.

## Hardware Connections

| Arduino/ESP32 | HC-SR04 |
|---------------|---------|
| 5V            | VCC     |
| GPIO 7        | Trig    |
| GPIO 8        | Echo    |
| GND           | GND     |

## Setup

1. Connect the HC-SR04 sensor according to the hardware connections above
2. Upload the sketch to your board
3. Open the serial monitor and set baud rate to **115200**

## Expected Output

When working properly, you should see distance readings in centimeters like:
```
15.23
15.45
16.02
```

## Troubleshooting

### Garbled Text in Serial Monitor
If you see garbled characters like `�ju��-�sLR����f�`, your serial monitor baud rate doesn't match the sketch. Set it to **115200 baud**.

### "Timeout waiting for echo pulse start - Check sensor power!"
This means the sensor is not getting power or is not responding. Check:
- VCC is connected to 5V (or 3.3V for some sensor models)
- GND is properly connected
- Sensor has a solid power connection

### "Timeout waiting for echo pulse end - No nearby objects detected"
The sensor is powered but not detecting any objects. This is normal when:
- No objects are within range (max ~4 meters)
- Objects are too far away
- Objects are at extreme angles
- The sensor needs something solid to reflect off of

### "Out of range"
An object was detected but is beyond the 400cm maximum range.

## Notes

- The ESP32-S3 uses 3.3V logic, which may not reliably trigger some HC-SR04 sensors that expect 5V
- If you experience issues, consider using a 3.3V-compatible sensor or a level shifter
- Minimum reliable range is typically around 2cm
- Maximum reliable range is around 400cm for most HC-SR04 sensors

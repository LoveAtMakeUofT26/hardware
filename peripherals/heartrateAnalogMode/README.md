# Heart Rate Sensor - Analog Mode

This Arduino sketch reads analog heart rate data from a pulse sensor and streams it over serial communication for backend processing.

## Hardware Requirements

- ESP32 development board
- Heart rate pulse sensor
- **5V power source** (external power required - ESP32 only outputs 3.3V)

## Hardware Setup

### Power Configuration
⚠️ **Important**: The heart rate sensor requires 5V power input. Since the ESP32 only outputs 3.3V from its pins, you must connect the sensor to an external 5V power source (e.g., VIN pin connected to USB power, or external 5V supply).

### Sensor Configuration
- Set the sensor switch to **Analog Mode**

### Wiring
- **Signal Pin**: Connect sensor output to GPIO 3 (analog input)
- **VCC**: Connect to 5V power source
- **GND**: Connect to ground

## How It Works

The sketch continuously reads analog values from the heart rate sensor at a sampling rate of **50 Hz** (every 20ms) and outputs the raw data to the serial port at 115200 baud.

```
Sampling Rate: 50 Hz
Serial Baud Rate: 115200
Output Format: Raw analog values (0-4095)
```

## Usage

1. Upload the sketch to your ESP32
2. Ensure the sensor is properly powered with 5V
3. Open the Serial Monitor at **115200 baud** to view the raw data stream
4. Place your finger on the sensor

## Data Processing

This sketch streams **raw analog data** only. BPM calculation is performed on the backend side by:
- Detecting peaks and troughs in the analog signal
- Calculating time intervals between heartbeats
- Converting to beats per minute (BPM)

The raw data stream allows for flexible signal processing and filtering algorithms on the backend.

## Code Overview

```cpp
#define heartPin 3        // Analog pin for heart rate sensor
Serial.begin(115200);     // Initialize serial communication
analogRead(heartPin);     // Read analog value
delay(20);                // 20ms delay = 50 Hz sampling
```

## Backend Integration

Your backend should:
1. Read serial data at 115200 baud
2. Buffer incoming analog values
3. Apply signal processing (e.g., smoothing, noise filtering)
4. Detect peaks/troughs to identify heartbeats
5. Calculate BPM from inter-beat intervals

## Troubleshooting

- **No data / erratic readings**: Check 5V power connection
- **Constant high/low values**: Verify sensor is in Analog Mode
- **Serial communication issues**: Confirm baud rate is set to 115200

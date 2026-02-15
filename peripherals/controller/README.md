# ESP32 Sensor & Servo Controller

Main controller that integrates distance detection, heart rate monitoring, and servo control with WiFi connectivity.

## Features

- **Sensor Streaming**: Real-time WebSocket JSON stream of distance and heart rate data
- **Servo Control**: HTTP endpoints to control two servos remotely
- **WiFi Connected**: Joins your existing WiFi network for wireless operation

## Hardware Requirements

- ESP32 development board
- HC-SR04 ultrasonic distance sensor
- Heart rate pulse sensor (analog mode)
- 2x Servo motors
- **5V external power** for heart rate sensor
- USB power or external power supply for ESP32

## Pin Connections

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| **Distance Sensor** | | |
| HC-SR04 Trig | GPIO 7 | |
| HC-SR04 Echo | GPIO 8 | |
| HC-SR04 VCC | 5V | |
| HC-SR04 GND | GND | |
| **Heart Rate Sensor** | | |
| HR Signal | GPIO 3 | Analog input |
| HR VCC | 5V | **External 5V required** |
| HR GND | GND | |
| **Servos** | | |
| SlapServo Signal | GPIO 48 | Executes slap motion |
| TalkServo Signal | GPIO 47 | Positional control |
| Servo VCC | 5V | May need external power for both |
| Servo GND | GND | Common ground |

## Software Setup

### 1. Install Required Libraries

Open Arduino IDE and install these libraries via Library Manager:

- **ESP32Servo** by Kevin Harrington
- **ESPAsyncWebServer** by Me-No-Dev
- **AsyncTCP** by Me-No-Dev  
- **ArduinoJson** by Benoit Blanchon

### 2. Configure WiFi Credentials

Edit `controller.ino` and replace these lines with your network information:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### 3. Upload to ESP32

1. Select your ESP32 board in Arduino IDE
2. Choose the correct COM port
3. Upload the sketch
4. Open Serial Monitor (115200 baud)

### 4. Note the IP Address

When successfully connected, the Serial Monitor will display:

```
WiFi connected!
IP Address: 192.168.1.XXX

Endpoints:
  WebSocket: ws://192.168.1.XXX/ws
  POST http://192.168.1.XXX/slap
  POST http://192.168.1.XXX/talk?value=X
  GET  http://192.168.1.XXX/status
```

**Save this IP address** - you'll need it to connect from your laptop.

## API Reference

### WebSocket Stream (Sensor Data)

**Endpoint**: `ws://[ESP32_IP]/ws`

**Format**: JSON messages sent every 100ms

```json
{
  "distance": 15.23,
  "heartRate": 512,
  "timestamp": 12345
}
```

**Distance values**:
- Positive number: Distance in centimeters
- `-1`: Timeout waiting for sensor
- `-2`: No object detected
- `-3`: Out of range (>400cm)

**Heart Rate values**:
- Raw analog reading (0-4095)
- BPM calculation should be done on your laptop

### HTTP Endpoints

#### Slap Servo

**Endpoint**: `POST /slap`

Executes a "slap" motion: moves from 0° to 65° and back.

```bash
curl -X POST http://192.168.1.XXX/slap
```

**Response**:
```json
{"status": "slapped"}
```

#### Talk Servo

**Endpoint**: `POST /talk?value=X`

Sets the talk servo position based on a value from 0-10.

- `value=0`: Closed position (0°)
- `value=10`: Open position (30°)
- `value=5`: Halfway (15°)

```bash
curl -X POST "http://192.168.1.XXX/talk?value=7"
```

**Response**:
```json
{
  "status": "success",
  "value": 7,
  "degrees": 21
}
```

**Error Response** (missing parameter):
```json
{
  "status": "error",
  "message": "Missing 'value' parameter"
}
```

#### Status Endpoint

**Endpoint**: `GET /status`

Returns current sensor readings and system info.

```bash
curl http://192.168.1.XXX/status
```

**Response**:
```json
{
  "distance": 15.23,
  "heartRate": 512,
  "uptime": 123456,
  "clients": 1
}
```

## Usage Examples

### Receiving Sensor Data (Python)

```python
import asyncio
import websockets
import json

async def receive_sensors():
    uri = "ws://192.168.1.XXX/ws"
    async with websockets.connect(uri) as websocket:
        while True:
            message = await websocket.recv()
            data = json.loads(message)
            print(f"Distance: {data['distance']}cm, Heart: {data['heartRate']}")

asyncio.run(receive_sensors())
```

### Receiving Sensor Data (JavaScript/Node.js)

```javascript
const WebSocket = require('ws');

const ws = new WebSocket('ws://192.168.1.XXX/ws');

ws.on('message', (data) => {
  const sensors = JSON.parse(data);
  console.log(`Distance: ${sensors.distance}cm, Heart: ${sensors.heartRate}`);
});
```

### Controlling Servos (Python)

```python
import requests

ESP32_IP = "192.168.1.XXX"

# Trigger slap
response = requests.post(f"http://{ESP32_IP}/slap")
print(response.json())

# Set talk position (0-10)
response = requests.post(f"http://{ESP32_IP}/talk?value=5")
print(response.json())
```

### Browser Console Testing

Open browser to `http://192.168.1.XXX/status` and open DevTools console:

```javascript
// Connect to WebSocket
const ws = new WebSocket('ws://192.168.1.XXX/ws');
ws.onmessage = (event) => console.log(JSON.parse(event.data));

// Control servos
fetch('http://192.168.1.XXX/slap', {method: 'POST'})
  .then(r => r.json())
  .then(console.log);

fetch('http://192.168.1.XXX/talk?value=10', {method: 'POST'})
  .then(r => r.json())
  .then(console.log);
```

## Tuning

### Adjusting Talk Servo Range

If the talk servo's open position needs adjustment, modify this constant in `controller.ino`:

```cpp
const int TALK_OPEN = 30;  // Change this value (degrees)
```

Upload the sketch again after changes.

### Changing Update Rate

To adjust sensor streaming frequency, modify:

```cpp
const unsigned long SENSOR_INTERVAL = 100;  // milliseconds
```

- `100` = 10 Hz (10 updates/second)
- `50` = 20 Hz (20 updates/second)
- `20` = 50 Hz (50 updates/second) - matches heart rate sensor sampling

## Troubleshooting

### Can't Connect to WiFi

- Verify SSID and password are correct
- Check that WiFi is 2.4GHz (ESP32 doesn't support 5GHz)
- Serial Monitor will show "Failed to connect to WiFi!" after 30 seconds
- Check signal strength - move ESP32 closer to router

### WebSocket Disconnects

- Check WiFi signal strength
- Reduce `SENSOR_INTERVAL` if network is congested
- Verify firewall isn't blocking WebSocket connections

### Servos Not Moving

- Check servo power supply (may need external 5V power)
- Verify correct pin connections (GPIO 47 and 48)
- Check Serial Monitor for servo position logs
- Test servos individually with simple servo test sketch

### Distance Sensor Returns Errors

- `-1`: Check sensor power and connections
- `-2`: Normal when no object in range
- `-3`: Object too far (>4 meters)

### Heart Rate Readings Look Wrong

- Ensure sensor has 5V power (not 3.3V)
- Verify sensor switch is in "Analog Mode"
- Place finger firmly on sensor
- Remember: raw analog values need BPM processing on laptop side

### HTTP Requests Timeout

- Ping ESP32 IP to verify network connectivity
- Check firewall settings on laptop
- Ensure ESP32 and laptop are on same network
- Try accessing `/status` endpoint first for debugging

## Technical Notes

- **Baud Rate**: 115200
- **Update Rate**: 10 Hz (100ms intervals)
- **WebSocket Protocol**: Text frames with JSON
- **HTTP Server**: Asynchronous (non-blocking)
- **Maximum WebSocket Clients**: Limited by ESP32 RAM (~5-10 concurrent)
- **Servo Control**: Brief blocking calls (500ms for slap sequence)

## Power Considerations

⚠️ **Important**: 
- Heart rate sensor requires 5V
- Two servos may draw significant current
- For reliable operation, use external 5V power supply
- USB power may be insufficient when both servos are active

## License

Based on public domain examples and hackathon project code.

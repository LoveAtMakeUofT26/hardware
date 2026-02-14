# ESP32-CAM Web Server

Streams video from ESP32-CAM to a laptop, which forwards to a remote server.

## Hardware Required

- ESP32-CAM (AI-Thinker module with PSRAM)
- USB-to-serial programmer (FTDI, CP2102, etc.)

## Setup

### 1. Arduino IDE Configuration
- Install ESP32 board support: `https://dl.espressif.com/dl/package_esp32_index.json`
- Select **AI Thinker ESP32-CAM** from Tools > Board
- Select partition scheme with **at least 3MB APP space**

### 2. Install Libraries
- `esp_camera` (included with ESP32 board package)
- `WiFi` (built-in)

### 3. Configure WiFi
Edit [CameraWebServer.ino](CameraWebServer/CameraWebServer.ino):
```cpp
const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";
```

### 4. Upload
- Connect ESP32-CAM via programmer
- Set GPIO0 to GND for flash mode
- Upload sketch
- Remove GPIO0 connection and press reset

## Usage

After successful boot (LED solid on):
- Serial monitor shows: `Camera Ready! Use 'http://192.168.x.x' to connect`
- Access stream at: `http://<ESP32-IP>/stream`

**LED Indicators:**
- Solid ON = Connected and streaming
- Blinking = Connection error

## Architecture

```
ESP32-CAM → WiFi → Laptop (local server) → Remote Server
```

Tested with local server during development.

## Security Warning

⚠️ **No authentication implemented** - stream is accessible to anyone on the network. Do not expose to untrusted networks.

## Camera Configuration

- Resolution: VGA (640x480)
- Format: MJPEG
- JPEG Quality: 10 (with PSRAM)
- Frame buffer: 2 buffers in PSRAM

## Troubleshooting

- **Camera init failed**: Check camera ribbon cable connection
- **WiFi FAILED**: Verify SSID/password, check 2.4GHz network
- **Brownout detected**: Use quality 5V 2A power supply
- **Poor quality**: Adjust `jpeg_quality` (lower = better quality, higher CPU)

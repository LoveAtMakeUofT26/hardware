/*
 * ESP32 Sensor & Servo Controller
 * 
 * This sketch integrates:
 * - HC-SR04 ultrasonic distance sensor
 * - Heart rate sensor (analog mode)
 * - Two servo motors (SlapServo and TalkServo)
 * 
 * Features:
 * - Streams sensor data via WebSocket in JSON format
 * - HTTP endpoints to control servos
 * - Connects to your WiFi network
 * 
 * Date: February 14, 2026
 */

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

// ============= CONFIGURATION =============
// WiFi Credentials - CHANGE THESE TO YOUR NETWORK
const char* ssid = "Not ur iPhone";
const char* password = "noturwifi132";

// Pin Definitions
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;
const int HEART_PIN = 3;
const int SLAP_SERVO_PIN = 48;
const int TALK_SERVO_PIN = 47;

// Servo Position Constants
const int TALK_CLOSED = 70;  // Closed position
const int TALK_OPEN = 90;    // Open for talk position
const int SLAP_START = 0;
const int SLAP_PEAK = 65;

// Sensor Reading Interval
const unsigned long SENSOR_INTERVAL = 100;  // 100ms = 10 Hz

// Distance Sensor Constants
const unsigned int MAX_DIST = 23200;  // 400 cm max range

// ============= GLOBAL OBJECTS =============
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Servo slapServo;
Servo talkServo;

// Timing
unsigned long lastSensorRead = 0;

// ============= SENSOR FUNCTIONS =============

float readDistance() {
  unsigned long t1, t2;
  unsigned long pulse_width;
  float cm;

  // Send trigger pulse
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for echo pulse start with timeout
  unsigned long timeout = micros();
  while (digitalRead(ECHO_PIN) == 0) {
    if (micros() - timeout > 25000) {  // 25ms timeout
      return -1.0;  // Timeout error
    }
  }

  // Measure pulse width
  t1 = micros();
  while (digitalRead(ECHO_PIN) == 1) {
    if (micros() - t1 > 25000) {  // 25ms timeout
      return -2.0;  // No object detected
    }
  }
  t2 = micros();
  pulse_width = t2 - t1;

  // Calculate distance in centimeters
  cm = pulse_width / 58.0;

  // Check if out of range
  if (pulse_width > MAX_DIST) {
    return -3.0;  // Out of range
  }

  return cm;
}

int readHeartRate() {
  return analogRead(HEART_PIN);
}

// ============= SERVO CONTROL FUNCTIONS =============

void doSlap() {
  Serial.println("Executing slap...");
  slapServo.write(SLAP_PEAK);
  delay(500);
  slapServo.write(SLAP_START);
  delay(500);
  Serial.println("Slap complete!");
}

void setTalkPosition(int value) {
  // Clamp value to 0-10 range
  if (value < 0) value = 0;
  if (value > 10) value = 10;
  
  // Map value (0-10) to degrees (TALK_CLOSED to TALK_OPEN)
  int degrees = map(value, 0, 10, TALK_CLOSED, TALK_OPEN);
  
  talkServo.write(degrees);
  
  Serial.print("TalkServo: value=");
  Serial.print(value);
  Serial.print(", degrees=");
  Serial.println(degrees);
}

// ============= WEBSOCKET HANDLERS =============

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, 
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("WebSocket client #%u connected from %s\n", 
                  client->id(), client->remoteIP().toString().c_str());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
  }
}

// ============= SETUP =============

void setup() {
  // Initialize Serial
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n\nESP32 Sensor & Servo Controller");
  Serial.println("================================");

  // Configure distance sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // Initialize servos
  Serial.println("Initializing servos...");
  slapServo.attach(SLAP_SERVO_PIN, 600, 2300);
  talkServo.attach(TALK_SERVO_PIN, 600, 2300);
  slapServo.write(SLAP_START);
  talkServo.write(TALK_CLOSED);
  Serial.println("Servos ready!");

  // Connect to WiFi
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long wifiTimeout = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - wifiTimeout > 30000) {
      Serial.println("\nFailed to connect to WiFi!");
      Serial.println("Please check your SSID and password.");
      while (1) delay(1000);  // Halt
    }
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Configure WebSocket
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // ============= HTTP ENDPOINTS =============

  // POST /slap - Execute slap servo action
  server.on("/slap", HTTP_POST, [](AsyncWebServerRequest *request) {
    doSlap();
    request->send(200, "application/json", "{\"status\":\"slapped\"}");
  });

  // POST /talk?value=X - Set talk servo position (0-10)
  server.on("/talk", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      int value = request->getParam("value")->value().toInt();
      setTalkPosition(value);
      
      // Build response
      String response = "{\"status\":\"success\",\"value\":" + String(value) + 
                       ",\"degrees\":" + String(map(value, 0, 10, TALK_CLOSED, TALK_OPEN)) + "}";
      request->send(200, "application/json", response);
    } else {
      request->send(400, "application/json", 
                   "{\"status\":\"error\",\"message\":\"Missing 'value' parameter\"}");
    }
  });

  // GET /status - Debug endpoint for current readings
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    float distance = readDistance();
    int heartRate = readHeartRate();
    
    String response = "{\"distance\":" + String(distance, 2) + 
                     ",\"heartRate\":" + String(heartRate) + 
                     ",\"uptime\":" + String(millis()) + 
                     ",\"clients\":" + String(ws.count()) + "}";
    request->send(200, "application/json", response);
  });

  // Start server
  server.begin();
  Serial.println("HTTP server started!");
  Serial.println("\nEndpoints:");
  Serial.println("  WebSocket: ws://" + WiFi.localIP().toString() + "/ws");
  Serial.println("  POST http://" + WiFi.localIP().toString() + "/slap");
  Serial.println("  POST http://" + WiFi.localIP().toString() + "/talk?value=X");
  Serial.println("  GET  http://" + WiFi.localIP().toString() + "/status");
  Serial.println("\nStreaming sensor data...\n");
}

// ============= MAIN LOOP =============

void loop() {
  // Clean up WebSocket clients
  ws.cleanupClients();

  // Read and broadcast sensor data at specified interval
  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorRead >= SENSOR_INTERVAL) {
    lastSensorRead = currentMillis;

    // Read sensors
    float distance = readDistance();
    int heartRate = readHeartRate();

    // Build JSON message
    String json = "{\"distance\":" + String(distance, 2) + 
                  ",\"heartRate\":" + String(heartRate) + 
                  ",\"timestamp\":" + String(currentMillis) + "}";

    // Broadcast to all WebSocket clients
    if (ws.count() > 0) {
      ws.textAll(json);
    }
  }
}

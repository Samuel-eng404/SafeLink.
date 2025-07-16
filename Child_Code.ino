// Author: Team SafeLink
// Purpose: Transmits data to parent bracelet via Wi-Fi,

#include <WiFi.h>

// Wi-Fi Credentials (Child as Station)
const char* ssid = "MyESP32Network";
const char* password = "MyPassword123";
const char* serverIP = "192.168.4.1"; // Parent IP
const uint16_t serverPort = 80;
WiFiClient client;

// Alert State Flags
bool reedAlertTriggered = false;
bool rssiAlertTriggered = false;

// Pin Definitions
#define OUTPUT_PIN   22   // Reed switch OUTPUT control
#define INPUT_PIN    5    // Reed switch INPUT read
#define vibratePin   16   // Vibration motor
#define buzzerPin    21   // Buzzer
#define buttonPin    17   // SOS push button

// Variables for reed switch and button
int reedopen = 1;
int push = 0;

// RSSI Filtering (Moving Average)
const int windowSize = 5;
int32_t rssiValues[windowSize] = {0};
int rssiIndex = 0;
int32_t sum = 0;
bool filled = false;

// Battery Monitoring
const int adcPin = 35;
float maxBatteryVoltage = 4.2;
float minBatteryVoltage = 3.3;
const float calibrationFactor = 1.13;

void setup() {
  Serial.begin(115200);

  // Wi-Fi Setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  // IO Setup
  pinMode(buzzerPin, OUTPUT);
  pinMode(vibratePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(OUTPUT_PIN, OUTPUT);
  digitalWrite(OUTPUT_PIN, HIGH); // Force reed circuit HIGH
  pinMode(INPUT_PIN, INPUT);
}

// Trigger Custom Alerts: buzzer+vibration, buzzer-only, vibration-only
void customAlert(int vibBuzzerTimes, int buzzerOnlyTimes, int vibOnlyTimes) {
  for (int i = 0; i < vibBuzzerTimes; i++) {
    tone(buzzerPin, 2731);
    digitalWrite(vibratePin, HIGH);
    delay(300);
    noTone(buzzerPin);
    digitalWrite(vibratePin, LOW);
    delay(300);
  }
  for (int i = 0; i < buzzerOnlyTimes; i++) {
    tone(buzzerPin, 2731);
    delay(300);
    noTone(buzzerPin);
    delay(300);
  }
  for (int i = 0; i < vibOnlyTimes; i++) {
    digitalWrite(vibratePin, HIGH);
    delay(300);
    digitalWrite(vibratePin, LOW);
    delay(300);
  }
}

void loop() {
  // Read Reed Switch State
  reedopen = digitalRead(INPUT_PIN) == HIGH ? 0 : 1;

  // Read SOS Button State
  push = digitalRead(buttonPin) == HIGH ? 1 : 0;

  // Try to connect to parent if not connected
  if (!client.connected()) {
    Serial.println("Trying to connect to server...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connected to Server!");
    } else {
      Serial.println("Connection Failed, retrying in 2s...");
      delay(2000);
      return;
    }
  }

  // RSSI Reading and Filtering
  int32_t raw_rssi = WiFi.RSSI();
  sum -= rssiValues[rssiIndex]; // Subtract old value
  rssiValues[rssiIndex] = raw_rssi;
  sum += raw_rssi;

  rssiIndex = (rssiIndex + 1) % windowSize;
  if (rssiIndex == 0) filled = true;

  int32_t avg_rssi = filled ? sum / windowSize : sum / (rssiIndex + 1);

  // Battery Monitoring
  int adcValue = analogRead(adcPin);
  float voltage = adcValue * (3.3 / 4095.0);
  float batteryVoltage = voltage * (46 + 95) / 46;
  batteryVoltage *= calibrationFactor;
  float batteryPercentage = ((batteryVoltage - minBatteryVoltage) / (maxBatteryVoltage - minBatteryVoltage)) * 100.0;
  batteryPercentage = constrain(batteryPercentage, 0, 99);

  // Reed Switch Alert (bracelet opened)
  if (reedopen == 1 && !reedAlertTriggered) {
    customAlert(3, 0, 0); // Alert once if opened
    reedAlertTriggered = true;
  } else if (reedopen == 0) {
    reedAlertTriggered = false; // Reset alert if closed
  }

  // Send Data to Parent ESP32
  client.print(avg_rssi);
  client.print(",");
  client.print(batteryPercentage);
  client.print(",");
  client.print(reedopen == 1 ? "11" : "00"); // Tamper flag
  client.print(",");
  client.println(push == 1 ? "1" : "0"); // SOS flag

  // Debug Output
  Serial.print(avg_rssi);
  Serial.print(",");
  Serial.print(batteryPercentage);
  Serial.print(",");
  Serial.print(reedopen == 1 ? "11" : "00");
  Serial.print(",");
  Serial.println(push == 1 ? "1" : "0");

  delay(1000); // Update every second
}

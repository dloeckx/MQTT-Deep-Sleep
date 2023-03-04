#include <WiFi.h>
#include <ArduinoHA.h>
#include "secrets.h" // Wifi & MQTT credentials

RTC_DATA_ATTR int bootCount = 0;
int bootCountReceived = -1;

#define DEVICE_NAME "sleep_esp32"
WiFiClient espClient;
HADevice device(DEVICE_NAME);
HAMqtt mqtt(espClient, device, 6, 1); // QoS = 1
HASensorNumber bootCountSensor("bootCount", HASensorNumber::PrecisionP0);

void onMessage(const char* topic, const uint8_t* payload, uint16_t length) {
    Serial.printf("%05lu: New message on topic: %s: %s\n", millis(), topic, (const char *)payload);
}

// After deep sleep, the ESP32 will always re-start from setup().
void setup() {
  Serial.begin(9600);
  Serial.printf("\n\n%05lu: Welcome. bootCount = %d\n", millis(), bootCount);

  // Home Assistant
  device.setName("Sleep ESP32");
  bootCountSensor.setName("Boot Count");
  if (bootCount > 0) mqtt.setDiscoveryPrefix(NULL);

  // Wifi
  WiFi.mode(WIFI_STA); WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult(10000L) != WL_CONNECTED) { Serial.printf("%05lu: No Wifi\n", millis()); return; }
  Serial.printf("%05lu: Connected to WiFi: ", millis()); Serial.println(WiFi.localIP());

  // mqtt
  mqtt.begin(MQTT_HOST, MQTT_USER, MQTT_PASSWORD); mqtt.onMessage(onMessage);
  for (int i = 0; i < 100; i++) {  mqtt.loop(); delay(10); if (mqtt.isConnected()) break; }
  if (!mqtt.isConnected()) { Serial.printf("%05lu: No mqtt\n", millis()); return; }
  Serial.printf("%05lu: Connected to mqtt\n", millis());
  delay(100);

  // Subscribe
  if (!mqtt.subscribe("aha/" DEVICE_NAME "/bootCount/stat_t")) { Serial.printf("%05lu: Failed to subscribe to bootCount. Error %i\n", millis(), mqtt.lastError()); return; };
  Serial.printf("%05lu: Subscribed to bootCount sensor\n", millis());

  // Publish
  if (!bootCountSensor.setValue(bootCount)) { Serial.printf("%05lu: Failed to publish bootCount sensor. Error %i\n", millis(), mqtt.lastError()); return;};
  Serial.printf("%05lu: Published bootCount sensor value: %d\n", millis(), bootCount);
  bootCount++;
}

// loop() code can also be in setup, but we come here by return if connections don't work.
void loop() { 
  Serial.printf("%05lu: Goodnight!\n", millis());
  delay(500); // Need some delay here to avoid watchdog timeout when starting deep sleep.
  esp_sleep_enable_timer_wakeup(1000000L);
  esp_deep_sleep_start();
}
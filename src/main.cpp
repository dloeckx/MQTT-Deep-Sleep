#include <WiFi.h>
#include <ArduinoHA.h>
#include "secrets.h" // Wifi & MQTT credentials

RTC_DATA_ATTR int bootCount = 0;
int bootCountReceived = -1;

#define DEVICE_NAME "sleep_esp32"
WiFiClient espClient;
HADevice device(DEVICE_NAME);
HAMqtt mqtt(espClient, device);
HASensorNumber bootCountSensor("bootCount", HASensorNumber::PrecisionP0);

void onMessage(const char* topic, const uint8_t* payload, uint16_t length) {
    Serial.printf("New message on topic: %s: %s\n", topic, (const char *)payload);
}

// After deep sleep, the ESP32 will always re-start from setup().
void setup() {
  Serial.begin(9600);
  Serial.printf("\n\nWelcome. bootCount =  %d\n", bootCount);

  // Home Assistant
  device.setName("Sleep ESP32");
  bootCountSensor.setName("Boot Count");
  if (bootCount > 0) mqtt.setDiscoveryPrefix(NULL);

  // Wifi
  WiFi.mode(WIFI_STA); WiFi.begin(SSID, PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) { Serial.printf("No Wifi\n"); return; }
  Serial.print("Connected to WiFi: "); Serial.println(WiFi.localIP());

  // mqtt
  mqtt.begin(MQTT_HOST, MQTT_USER, MQTT_PASSWORD); mqtt.onMessage(onMessage);
  for (int i = 0; i < 100; i++) {  mqtt.loop(); delay(10); if (mqtt.isConnected()) break; }
  if (!mqtt.isConnected()) { Serial.printf("No mqtt\n"); return; }
  Serial.printf("Connected to mqtt\n");

  // Publish and subscribe
  bootCountSensor.setValue(bootCount);
  mqtt.subscribe("aha/" DEVICE_NAME "/bootCount/stat_t");
  for (int i = 0; i < 200; i++) { mqtt.loop(); delay(10); }

  bootCount++;
}

// loop() code can also be in setup, but we come here by return if connections don't work.
void loop() { 
  Serial.printf("Goodnight!\n");
  esp_sleep_enable_timer_wakeup(5000000L);
  esp_deep_sleep_start();
}
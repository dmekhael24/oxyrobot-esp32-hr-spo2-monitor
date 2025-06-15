 /*******************************************************************************
 *  ESP32 – MAX30102 Heart-Rate & SpO2 monitor with Telegram alert
 *  Hardware:
 *    • ESP32 DevKit v1 (or similar)
 *    • DFRobot Gravity MAX30102 Heart-Rate & Oximeter Sensor v2.0
 *  Connections (default I2C):
 *    ESP32 3V3  ->  VCC   |  ESP32 GND  ->  GND
 *    ESP32 GPIO21 -> SDA  |  ESP32 GPIO22 -> SCL
 ******************************************************************************/

#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DFRobot_BloodOxygen_S.h"

/* --------------------------------------------------------------------------
   USER SETTINGS
   -------------------------------------------------------------------------- */
const char* WIFI_SSID     = "C311";
const char* WIFI_PASSWORD = "KCHTC2H5OH";

/* Telegram credentials  */
const char* TG_BOT_TOKEN  = "7867129252:AAGifceoByQW4zDy2N9PrnISSPTi12mnxhQ";
const char* TG_CHAT_ID    = "1422418016";

/* Alert thresholds */
const uint8_t SPO2_LOW_LIMIT   = 97;    // %
const uint8_t HEART_LOW_LIMIT  = 70;    // bpm
const uint8_t HEART_HIGH_LIMIT = 90;   // bpm

/* Sensor update interval (MAX30102 refreshes every 4 s) */
const uint32_t MEASUREMENT_PERIOD_MS = 4000;
/* -------------------------------------------------------------------------- */

/* Sensor object (I2C address 0x57) */
#define I2C_ADDRESS 0x57
DFRobot_BloodOxygen_S_I2C oximeter(&Wire, I2C_ADDRESS);

/* --------------------------------------------------------------------------
   Telegram helper
   -------------------------------------------------------------------------- */
void sendAlert(uint8_t spo2, uint8_t bpm)
{
  if (WiFi.status() != WL_CONNECTED) return;        // skip if offline

  HTTPClient http;
  String url = String("https://api.telegram.org/bot") + TG_BOT_TOKEN + "/sendMessage";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  /* If you hit SSL errors on very old ESP32 cores, uncomment: */
  // http.setInsecure();

  /* Build JSON payload */
  String message = String("⚠️❤️‍🔥 HR/SpO2 alert!\nSpO2: ") + spo2 +
                   "%\nHeart rate: " + bpm + " bpm";
  String payload = String("{\"chat_id\":\"") + TG_CHAT_ID +
                   "\",\"text\":\"" + message + "\"}";

  int code = http.POST(payload);
  Serial.printf("[HTTP] Telegram POST returned %d\n", code);
  http.end();
}

/* -------------------------------------------------------------------------- */
void setup()
{
  Serial.begin(115200);
  delay(1000);                         // let Serial settle

  /* -------- Wi-Fi ---------- */
  WiFi.mode(WIFI_STA);
  Serial.printf("Connecting to %s ", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - t0) < 15000) {
    delay(200);
    Serial.print('.');
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWi-Fi connected, IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWi-Fi NOT connected – continuing offline");
  }

  /* -------- Sensor --------- */
  if (!oximeter.begin()) {
    Serial.println("MAX30102 init FAIL – check wiring.");
    while (true) delay(1000);
  }
  Serial.println("MAX30102 init OK – start measuring...");
  oximeter.sensorStartCollect();
}

/* -------------------------------------------------------------------------- */
void loop()
{
  /* Read sensor (library updates every 4 s) */
  oximeter.getHeartbeatSPO2();
  uint8_t spo2 = oximeter._sHeartbeatSPO2.SPO2;
  uint8_t bpm  = oximeter._sHeartbeatSPO2.Heartbeat;

   /* Local debug output */
  Serial.printf("SpO2: %u %% | Heart rate: %u bpm\n", spo2, bpm);

  // Reject faulty readings (255 = invalid)
  bool valid = (spo2 > 50 && spo2 < 101) && (bpm > 30 && bpm < 200);

  if (!valid) {
    Serial.println("⚠️ Invalid reading – skipping alert.");
    delay(MEASUREMENT_PERIOD_MS);
    return;
  }

  bool trigger = (spo2 < SPO2_LOW_LIMIT) ||
                 (bpm  < HEART_LOW_LIMIT) ||
                 (bpm  > HEART_HIGH_LIMIT);

  if (trigger) {
    Serial.println("Threshold crossed – sending Telegram alert");
    sendAlert(spo2, bpm);
  }

  delay(MEASUREMENT_PERIOD_MS);
}

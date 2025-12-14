/*************************************************
 * ESP32 Air Quality Monitor
 * MQTT JSON + Blynk + TLS
 *************************************************/

/* ================= BLYNK CONFIG ================= */
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6CfqHZdZl"
#define BLYNK_TEMPLATE_NAME "ESP32 Air Quality"
#define BLYNK_AUTH_TOKEN "YbQE7xjm6FISaTz8wx0oNcScMqD_AKGt"

char blynkAuth[] = BLYNK_AUTH_TOKEN;

/* ================= LIBRARIES ==================== */
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

/* ================= WIFI ========================= */
const char* ssid = "67dtp302";
const char* password = "lusaco123";

/* ================= MQTT ========================= */
const char* mqtt_server   = "56ccb23b917d48aa837086e35825e960.s1.eu.hivemq.cloud";
const int   mqtt_port     = 8883;
const char* mqtt_username = "air-quality";
const char* mqtt_password = "AirQuality123";

/* ================= PIN ========================== */
#define DHTPIN       4
#define DHTTYPE      DHT11
#define MQ135_PIN    34
#define BUZZER_PIN   27
#define LED_GREEN    16
#define LED_RED      23

/* ================= OBJECT ======================= */
WiFiClientSecure espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

/* ================= STATE ======================== */
bool buzzerState = false;
bool ledRedState = false;
unsigned long lastMsg = 0;
unsigned long ledBlinkMillis = 0;
bool ledBlinkState = false;

/* ================= WIFI ========================= */
void setupWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

/* ================= MQTT CALLBACK ================= */
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (uint8_t i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("MQTT [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  if (String(topic) == "esp32/control") {
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, message);

    if (!err) {
      if (doc.containsKey("buzzer")) {
        buzzerState = doc["buzzer"];
      }
      if (doc.containsKey("led")) {
        ledRedState = doc["led"];
      }
    }
  }
}

/* ================= MQTT RECONNECT ================= */
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    String clientId = "ESP32-Air-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("esp32/control");
    } else {
      Serial.println("failed, retry 5s");
      delay(5000);
    }
  }
}

/* ================= BLYNK ========================= */
BLYNK_WRITE(V3) {   // Buzzer
  buzzerState = param.asInt();
}

BLYNK_WRITE(V4) {   // LED đỏ
  ledRedState = param.asInt();
}

/* ================= SETUP ========================= */
void setup() {
  Serial.begin(115200);

  // LCD - SDA = 32, SLC = 33 
  Wire.begin(32, 33);
  lcd.init();
  lcd.backlight();

  dht.begin();
  analogReadResolution(12);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  setupWiFi();

  Blynk.config(blynkAuth);
  Blynk.connect();

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);

  lcd.setCursor(0, 0);
  lcd.print("ESP32 READY");
  delay(1500);
  lcd.clear();
}

/* ================= LOOP ========================== */
void loop() {
  Blynk.run();

  if (!client.connected()) reconnectMQTT();
  client.loop();

  unsigned long now = millis();

  /* ===== LED ===== */
  if (ledRedState) {
    digitalWrite(LED_GREEN, LOW);
    if (now - ledBlinkMillis > 500) {
      ledBlinkMillis = now;
      ledBlinkState = !ledBlinkState;
      digitalWrite(LED_RED, ledBlinkState);
    }
  } else {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
  }

  /* ===== BUZZER ===== */
  if (buzzerState) {
    tone(BUZZER_PIN, 400);   // kêu liên tục ở tần số 400Hz
    delay(100);              // giữ 100ms
    noTone(BUZZER_PIN);      // tắt còi
    delay(100);              // nghỉ 100ms
  }
  else noTone(BUZZER_PIN);

  /* ===== SENSOR ===== */
  if (now - lastMsg > 2000) {
    lastMsg = now;

    float t = dht.readTemperature();
    float h = dht.readHumidity();
    float co2 = map(analogRead(MQ135_PIN), 0, 4095, 400, 2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(t);
    lcd.print(" H:");
    lcd.print(h);

    lcd.setCursor(0, 1);
    lcd.print("CO2:");
    lcd.print(co2);
    lcd.print("ppm");

    StaticJsonDocument<256> doc;
    doc["temperature"] = t;
    doc["humidity"] = h;
    doc["co2_ppm"] = co2;

    char buffer[256];
    serializeJson(doc, buffer);
    client.publish("airquality/data", buffer);

    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, h);
    Blynk.virtualWrite(V2, co2);    

    // LED + Buzzer theo ngưỡng
    if (co2 <= 1400) {
      noTone(BUZZER_PIN);
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);
    } else {
      tone(BUZZER_PIN, 400);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);

      delay(500);
      noTone(BUZZER_PIN);
    }
  }
}

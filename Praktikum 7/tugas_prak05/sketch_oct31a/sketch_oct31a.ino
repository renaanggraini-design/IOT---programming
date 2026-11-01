#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// --- KONFIGURASI WIFI ---
const char* ssid = "mypacarr";         
const char* password = "12345678";    

// --- KONFIGURASI MQTT BROKER ---
// Sesuaikan dengan alamat IP laptop/komputer server Docker Mosquitto Anda
const char* mqttServer = "10.80.201.220"; 
const int mqtt_port = 1883;

// Kredensial Autentikasi MQTT sesuai tugas praktikum
const char* username = "admin";
const char* password_mqtt = "rahasia123";

// --- KONFIGURASI SENSOR & PIN ---
#define DHTPIN D4       // Pin data DHT terhubung ke D4 (GPIO2)
#define DHTTYPE DHT22   // Jenis sensor (ubah ke DHT11 jika menggunakan DHT11)
DHT dht(DHTPIN, DHTTYPE);

const int ledPin = D2;  // Pin LED indikator aktuator

WiFiClient espClient;
PubSubClient client(espClient);

// Topik MQTT
const char* topicSubCmd = "kampus/labiot/kendali";
const char* topicPubTelemetri = "kampus/labiot/telemetri";

unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi terhubung!");
  Serial.print("Alamat IP ESP8266: ");
  Serial.println(WiFi.localIP());
}

// Fungsi callback saat menerima pesan kendali dari broker / Node-RED
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println("----------------------------------------");
  Serial.print("[INSTRUKSI MASUK] Topik: ");
  Serial.println(topic);
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("[PAYLOAD]               : ");
  Serial.println(message);

  // Aksi kontrol LED berdasarkan payload perintah
  if (message.indexOf("0") >= 0 || message == "OFF") {
    digitalWrite(ledPin, LOW);
    Serial.println("--> [AKTUATOR] Status LED diubah: PADAM (OFF)");
  } else if (message.indexOf("1") >= 0 || message == "ON") {
    digitalWrite(ledPin, HIGH);
    Serial.println("--> [AKTUATOR] Status LED diubah: MENYALA (ON)");
  }
  Serial.println("----------------------------------------");
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("[MQTT] Menghubungkan ke Broker Lokal (");
    Serial.print(mqttServer);
    Serial.print(")...\n");
    
    // Membuat Client ID unik untuk koneksi
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Menghubungkan ke broker dengan menyertakan Username dan Password
    if (client.connect(clientId.c_str(), username, password_mqtt)) {
      Serial.println("[MQTT] Terhubung dengan Autentikasi Sah! (rc=0)");
      
      // Berlangganan kembali ke topik kendali setelah berhasil terhubung
      client.subscribe(topicSubCmd);
      Serial.printf("[MQTT] Berlangganan ke topik: %s\n", topicSubCmd);
    } else {
      Serial.printf("[MQTT] Gagal (rc=%d). Coba lagi dalam 4 detik...\n", client.state());
      delay(4000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  dht.begin();
  setup_wifi();
  
  client.setServer(mqttServer, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Mengirim data telemetri sensor setiap 5 detik
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature(); // Celcius

    // Cek jika pembacaan sensor gagal
    if (isnan(h) || isnan(t)) {
      Serial.println("Gagal membaca dari sensor DHT!");
      return;
    }

    int statusLed = digitalRead(ledPin);

    // Format payload JSON telemetri sesuai standar sistem industri
    String payloadJson = "{\"node_id\":\"ESP8266_Local_Node01\",\"suhu\":" + String(t, 2) + ",\"kelembapan\":" + String(h, 2) + ",\"status_led\":" + String(statusLed) + "}";
    
    client.publish(topicPubTelemetri, payloadJson.c_str());
    Serial.println("[TELEMETRI TERKIRIM] " + payloadJson);
  }
}
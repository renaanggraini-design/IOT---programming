#include "DHT.h"

// Definisi Pin
#define DHTPIN D5          // Pin Data DHT ke D5
#define DHTTYPE DHT11      // Ubah ke DHT22 jika memakai sensor DHT22 putih
const int ldrPin    = A0;  // Pin AO modul LDR ke pin A0 ESP8266
const int sensorPin = D1;  // Pin trigger tombol/sensor halangan
const int relayPin  = D2;  // Pin IN modul relay

DHT dht(DHTPIN, DHTTYPE);

// Variabel status & pewaktu
bool relayState = false;
int lastSensorState = HIGH;
unsigned long previousMillis = 0;
const long interval = 2000; // Interval pembacaan sensor setiap 2 detik

void setup() {
  Serial.begin(115200);
  delay(300);

  // Inisialisasi Sensor DHT
  dht.begin();

  // Konfigurasi pin
  pinMode(sensorPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // Relay awal MATI (Active LOW)

  Serial.println("\n--- SISTEM MONITORING & KONTROL AKTIF ---");
}

void loop() {
  // 1. KONTROL SAKELAR TOGGLE RELAY (Respon Seketika)
  int currentSensorState = digitalRead(sensorPin);

  // Deteksi transisi pemicu (Active LOW: HIGH -> LOW)
  if (currentSensorState == LOW && lastSensorState == HIGH) {
    relayState = !relayState;
    digitalWrite(relayPin, relayState ? LOW : HIGH);

    Serial.print("\n>>> STATUS RELAY BERUBAH: ");
    Serial.println(relayState ? "AKTIF (ON)" : "MATI (OFF)");
    
    delay(250); // Debounce
  }
  lastSensorState = currentSensorState;

  // 2. PEMBACAAN SUHU, KELEMBAPAN, DAN CAHAYA (Setiap 2 Detik)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Baca data dari DHT
    float kelembapan = dht.readHumidity();
    float suhu = dht.readTemperature();

    // Baca intensitas cahaya dari LDR (Rentang 0 - 1023)
    int nilaiLDR = analogRead(ldrPin);

    Serial.println("\n--- DATA SENSOR TERKINI ---");
    if (isnan(kelembapan) || isnan(suhu)) {
      Serial.println("[DHT Error] Gagal membaca DHT! Periksa kabel pin D5.");
    } else {
      Serial.print("Suhu Lingkungan : ");
      Serial.print(suhu);
      Serial.println(" °C");
      Serial.print("Kelembapan Udara: ");
      Serial.print(kelembapan);
      Serial.println(" %");
    }

    Serial.print("Intensitas LDR  : ");
    Serial.println(nilaiLDR);
  }
}
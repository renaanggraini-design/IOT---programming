#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC Address Node 2
uint8_t receiver1[] = {
  0xBC, 0xDD, 0xC2, 0xE3, 0xBD, 0x82
};

// MAC Address Node 3
uint8_t receiver2[] = {
  0x34, 0x94, 0x54, 0x7D, 0x85, 0x93
};

// MAC Address Node 4
uint8_t receiver3[] = {
  0x84, 0xF3, 0xEB, 0x92, 0xA9, 0xBC
};

typedef struct struct_pesan {
  int perintahId;
  int nilaiParameter;
} struct_pesan;

struct_pesan paketKirim;

unsigned long previousMillis = 0;
const long interval = 2000;

// Menampilkan status pengiriman
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {

  char macStr[18];

  snprintf(
    macStr,
    sizeof(macStr),
    "%02x:%02x:%02x:%02x:%02x:%02x",
    mac_addr[0],
    mac_addr[1],
    mac_addr[2],
    mac_addr[3],
    mac_addr[4],
    mac_addr[5]
  );

  Serial.print("Kirim paket ke: ");
  Serial.print(macStr);
  Serial.print(" | Status: ");

  if (sendStatus == 0) {
    Serial.println("Berhasil Diterima");
  } else {
    Serial.println("Gagal (Tidak Terjangkau)");
  }
}

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Inisialisasi ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Gagal menginisialisasi ESP-NOW!");
    return;
  }

  // Menjadikan Node 1 sebagai Controller
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Mengaktifkan callback pengiriman
  esp_now_register_send_cb(OnDataSent);

  // Mendaftarkan Node 2
  esp_now_add_peer(
    receiver1,
    ESP_NOW_ROLE_SLAVE,
    1,
    NULL,
    0
  );

  // Mendaftarkan Node 3
  esp_now_add_peer(
    receiver2,
    ESP_NOW_ROLE_SLAVE,
    1,
    NULL,
    0
  );

  // Mendaftarkan Node 4
  esp_now_add_peer(
    receiver3,
    ESP_NOW_ROLE_SLAVE,
    1,
    NULL,
    0
  );

  Serial.println("Controller ESP-NOW Siap!");
}

void loop() {

  unsigned long currentMillis = millis();

  // Kirim data setiap 2 detik
  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    paketKirim.perintahId = 101;

    paketKirim.nilaiParameter =
      random(10, 100);

    // Kirim ke semua receiver yang sudah terdaftar
    esp_now_send(
      0,
      (uint8_t *)&paketKirim,
      sizeof(paketKirim)
    );
  }
}
#include <dummy.h>

#include <dummy.h>

// Konfigurasi pin sesuai gambar skema Praktikum 2
const int pinLED    = D1; // Pin D1 terhubung ke LED
const int pinTombol = D2; // Pin D3 terhubung ke Push Button (Pull-Down)

// Variabel pelacak status (state tracking)
bool statusLED = false;          // Menyimpan kondisi LED (false = OFF/mati, true = ON/nyala)
int statusTombolSebelumnya = LOW; // Menyimpan status pembacaan tombol sebelumnya

void setup() {
  pinMode(pinLED, OUTPUT);
  pinMode(pinTombol, INPUT);

  // Saat sistem pertama kali dinyalakan, LED diatur dalam keadaan MATI (OFF)
  digitalWrite(pinLED, LOW);
}

void loop() {
  // Membaca input digital dari tombol (HIGH saat ditekan pada Rangkaian Pull-Down)
  int statusTombolSaatIni = digitalRead(pinTombol);

  // Deteksi Perubahan State (Edge Detection):
  // Memastikan logika hanya berjalan saat tombol BARU SAJA ditekan (transisi LOW ke HIGH)
  if (statusTombolSaatIni == HIGH && statusTombolSebelumnya == LOW) {
    
    // Fitur Debouncing:
    // Delay singkat 200ms untuk mengabaikan getaran/pantulan mekanis fisik tombol
    delay(200);

    // Toggle State:
    // Mengubah nilai variabel status (jika OFF menjadi ON, jika ON menjadi OFF)
    statusLED = !statusLED;

    // Memperbarui kondisi fisik LED sesuai status terbaru
    if (statusLED == true) {
      digitalWrite(pinLED, HIGH); // LED menyala dan tetap menyala
    } else {
      digitalWrite(pinLED, LOW);  // LED mati dan tetap mati
    }
  }

  // Menyimpan kondisi tombol saat ini untuk dibandingkan pada siklus loop berikutnya
  statusTombolSebelumnya = statusTombolSaatIni;
}
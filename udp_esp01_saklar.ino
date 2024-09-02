#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

//const char* ssid = "daffa4g";         // Ganti dengan nama SSID WiFi kamu
//const char* password = "bitagure123"; // Ganti dengan password WiFi kamu
const char* ssid = "PJU TIMUR";         // Ganti dengan nama SSID WiFi kamu
const char* password = "prastrans05"; // Ganti dengan password WiFi kamu

WiFiUDP udp;
unsigned int localUdpPort = 9999;       // Port UDP yang digunakan
unsigned int remotePortnya = 2222;       // Port UDP yang digunakan

const int irSensorPin = 2;  // Pin sensor IR (GPIO2 pada ESP-01)
const int ledPin = 3;       // Pin untuk LED atau relay (GPIO3 pada ESP-01)

//IPAddress broadcastIp(255, 255, 255, 255); // Alamat IP broadcast global
IPAddress broadcastIp(192,168,1,16); // Alamat IP statis atau ip lokal target

bool lampState = false;  // Flag untuk status lampu
unsigned long lastDetectionTime = 0; // Waktu deteksi terakhir
unsigned long debounceDelay = 1000;  // Debouncing delay dalam milidetik

void setup() {
  pinMode(irSensorPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi.");
  udp.begin(localUdpPort);
  Serial.printf("UDP server started at port %d\n", localUdpPort);
}

void loop() {
  int irState = digitalRead(irSensorPin); // Membaca status sensor IR
  unsigned long currentTime = millis();

  // Cek jika sensor mendeteksi objek dan melewati waktu debounce
  if (irState == LOW && (currentTime - lastDetectionTime > debounceDelay)) {
    lastDetectionTime = currentTime; // Perbarui waktu deteksi terakhir

    if (!lampState) {
      // Deteksi pertama - Nyalakan lampu dan kirim command "on"
      digitalWrite(ledPin, LOW);
      lampState = true;
      Serial.println("Object detected! Turning on the light.");

      // Mengirimkan sinyal UDP "on"
      const char* udpMessage = "on";
      udp.beginPacket(broadcastIp, remotePortnya);
      udp.write(udpMessage);
      udp.endPacket();
    } else {
      // Deteksi kedua - Matikan lampu dan kirim command "off"
      digitalWrite(ledPin, HIGH);
      lampState = false;
      Serial.println("Object detected again! Turning off the light.");

      // Mengirimkan sinyal UDP "off"
      const char* udpMessage = "off";
      udp.beginPacket(broadcastIp, remotePortnya);
      udp.write(udpMessage);
      udp.endPacket();
    }
  }

  // Tidak ada penundaan tambahan, loop terus berjalan
}

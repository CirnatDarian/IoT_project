#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>  // Biblioteca pentru NTP

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int relee[] = {16, 15};  // Pinurile pentru relee
#define SS_PIN 5
#define RST_PIN 17
MFRC522 rfid(SS_PIN, RST_PIN);

const char* ssid = "ASUS";
const char* password = "parasuta";

const char* mqtt_server = "192.168.1.14";  // Adresa IP a Raspberry Pi
WiFiClient espClient;
PubSubClient client(espClient);

// Definirea obiectului NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);  // Server NTP si offset-ul pentru UTC

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectare la WiFi...");
  }
  Serial.println("Conectat la WiFi");

  // Sincronizare ora cu NTP
  timeClient.begin();
  timeClient.update();

  client.setServer(mqtt_server, 1883);
  if (!client.connect("ESP32Client", "mqtt_user", "jmecher69")) {
    Serial.println("Conexiune MQTT eșuată!");
  } else {
    Serial.println("Conexiune la broker realizată!");
    client.subscribe("esp32/command");
  }
  client.setCallback(mqttCallback);

  dht.begin();

  for (int i = 0; i < 2; i++) {
    pinMode(relee[i], OUTPUT);
    digitalWrite(relee[i], LOW);
  }

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("Sistemul a fost inițializat cu succes.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Generare date aleatorii pentru temperatură și umiditate
  float t = random(20, 30);
  float h = random(40, 60);

  // Publicarea datelor de temperatură și umiditate
  String tempData = "{\"temperatura\": " + String(t) + ", \"umiditate\": " + String(h) + "}";
  Serial.println("Trimiterea datelor către broker: " + tempData);
  client.publish("esp32/data", tempData.c_str());

  // Actualizează ora și data
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();  // Obține timestamp-ul

  // Convertește timestamp-ul într-un format de dată și oră
  int year = 1970 + (epochTime / 31557600); // Calculul anului (31557600 secunde pe an)
  int month = ((epochTime / 2592000) % 12) + 1; // Calculul lunii
  int day = (epochTime / 86400) % 31 + 1;  // Calculul zilei
  int hour = (epochTime / 3600) % 24;  // Ora
  int minute = (epochTime / 60) % 60;  // Minutul
  int second = epochTime % 60;  // Secunda

  String currentTime = String(hour) + ":" + String(minute) + ":" + String(second);  // Ora curentă
  String currentDate = String(day) + "/" + String(month) + "/" + String(year);  // Data curentă

  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      String rfidID = "";
      for (byte i = 0; i < rfid.uid.size; i++) {
        rfidID += String(rfid.uid.uidByte[i], HEX);
      }

      Serial.println("Card RFID detectat. ID: " + rfidID);

      if (rfidID == "603325a3") {
        Serial.println("Card autorizat. Activare relee.");
        digitalWrite(relee[0], HIGH);
        digitalWrite(relee[1], HIGH);
        delay(2000);
        digitalWrite(relee[0], LOW);
        digitalWrite(relee[1], LOW);

        // Publicare comanda cu ora și data
        String commandData = "{\"command\": \"open_relay\", \"timestamp\": \"" + currentDate + " " + currentTime + "\"}";
        Serial.println("Trimiterea comenzii de deschidere a releelor: " + commandData);
        client.publish("esp32/command", commandData.c_str());
      } else {
        Serial.println("Acces nepermis!");
      }
      delay(1000);
    }
  }
  delay(1000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Încercare reconectare la broker MQTT...");
    if (client.connect("ESP32Client", "mqtt_user", "jmecher69")) {
      Serial.println("Reconectare realizată!");
      client.subscribe("esp32/command");
    } else {
      Serial.print("Reconectare eșuată, aștept 5 secunde pentru nouă încercare...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.println("Mesaj primit pe topicul " + String(topic) + ": " + message);

  if (message == "open_relay") {
    Serial.println("Comandă primită: deschiderea releelor.");
    digitalWrite(relee[0], HIGH);
    digitalWrite(relee[1], HIGH);
    delay(2000);
    digitalWrite(relee[0], LOW);
    digitalWrite(relee[1], LOW);
  }
}

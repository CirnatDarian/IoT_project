#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <MFRC522.h>
#include <DHT.h>
#include <WiFiUdp.h>
#include <NTPClient.h>  // biblioteca pentru NTP

#define DHTPIN 14
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int relee[] = {16, 15};  // pinurile pentru relee
#define SS_PIN 5
#define RST_PIN 17
MFRC522 rfid(SS_PIN, RST_PIN);

const char* ssid = "xxx";
const char* password = "xxx";

const char* mqtt_server = "192.168.1.14";  // adresa IP a Raspberry Pi
WiFiClient espClient;
PubSubClient client(espClient);

// definirea obiectului NTPClient
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);  // server NTP si offset-ul pentru UTC

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectare la WiFi...");
  }
  Serial.println("Conectat la WiFi");

  // sincronizare ora cu NTP
  timeClient.begin();
  timeClient.update();

  client.setServer(mqtt_server, 1883);
  if (!client.connect("ESP32Client", "mqtt_user", "jmecher69")) {
    Serial.println("Conexiune MQTT esuata!");
  } else {
    Serial.println("Conexiune la broker realizata!");
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
  Serial.println("Sistemul a fost initializat cu succes.");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // generare date aleatorii pentru temperatura si umiditate
  float t = random(20, 30);
  float h = random(40, 60);

  // publicarea datelor de temperatura si umiditate
  String tempData = "{\"temperatura\": " + String(t) + ", \"umiditate\": " + String(h) + "}";
  Serial.println("Trimiterea datelor catre broker: " + tempData);
  client.publish("esp32/data", tempData.c_str());

  // actualizeaza ora si data
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();  // obtine timestamp-ul

  // converteste timestamp-ul intr-un format de data si ora
  int year = 1970 + (epochTime / 31557600); // calculul anului
  int month = ((epochTime / 2592000) % 12) + 1; // calculul lunii
  int day = (epochTime / 86400) % 31 + 1;  // calculul zilei
  int hour = (epochTime / 3600) % 24;  // ora
  int minute = (epochTime / 60) % 60;  // minutul
  int second = epochTime % 60;  // secunda

  String currentTime = String(hour) + ":" + String(minute) + ":" + String(second);  // ora curenta
  String currentDate = String(day) + "/" + String(month) + "/" + String(year);  // data curenta

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

        // publicare comanda cu ora si data
        String commandData = "{\"command\": \"open_re_

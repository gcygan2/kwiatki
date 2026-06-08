#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

#define LED_PIN 5
#define LED_COUNT 8
#define GRZYBEK 25

#define EEPROM_SIZE 512
#define START_ADDR 0

String ssid;
String password;

int tik = 0;
volatile int pokaz = 20;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

WebServer server(80);
bool apMode = false;

// Strona HTML z formularzem
String formPage() {
  return String(
    "<html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'></head><body><div style='background-color: white;max-width: 400px;margin: auto;'>"
    "<form method='POST' action='/save'>"
    "SSID: <input name='ssid'><br><br>"
    "Klucz: <input name='key'><br><br>"
    "<input type='submit' value='Zapisz'>"
    "</form>"
    "</body></html>"
  );
}

void handleRoot() {
  server.send(200, "text/html", formPage());
}

void handleSave() {
  Serial.println("Args: " + String(server.args()));

  if (server.args() > 0) {
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Brak danych");
  }
  
  if (server.hasArg("ssid") && server.hasArg("key")) {
    EEPROM.begin(EEPROM_SIZE);
    writeStringToEEPROM(START_ADDR, server.arg("ssid")+'\\'+server.arg("key"));
    EEPROM.commit();
  }
}

void setupAP() {
  WiFi.mode(WIFI_AP);

  WiFi.softAPConfig(
    IPAddress(192,168,31,1),
    IPAddress(192,168,31,1),
    IPAddress(255,255,255,0)
  );

  WiFi.softAP("KWIATKI", "12345678");

  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
}
  
void handleInterrupt ()
{
  //Serial.println("Przerwanie");
  portENTER_CRITICAL_ISR(&mux);
  if (pokaz == 19 || pokaz > 190) {
    pokaz = 2000;
  } else {
    pokaz = 20;
  }
  portEXIT_CRITICAL_ISR(&mux);
}

void writeStringToEEPROM(int addr, const String &data) {
  int len = data.length();
  EEPROM.write(addr, len);
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr + 1 + i, data[i]);
  }
}

String readStringFromEEPROM(int addr) {
  int len = EEPROM.read(addr);
  if (len <= 0 || len > 250) return "";

  String result = "";
  for (int i = 0; i < len; i++) {
    result += char(EEPROM.read(addr + 1 + i));
  }
  return result;
}

void setup() {
  pinMode(GRZYBEK, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GRZYBEK), handleInterrupt, FALLING);
  
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  // Sprawdzenie przycisku podczas startu
  if (digitalRead(GRZYBEK) == LOW) {
    apMode = true;
    Serial.println("Start w trybie AP");
    setupAP();
  } else {
    apMode = false;
    Serial.println("Normalny start (bez AP)");
  
    String stored = readStringFromEEPROM(START_ADDR);
  
    int sep = stored.indexOf('\\');
    if (sep != -1) { 
      ssid = stored.substring(0, sep);
      password = stored.substring(sep + 1);
    }
    Serial.println(ssid);
    Serial.println(password);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(300);
    }
  }
}

void loop() {
  if (apMode) {
    server.handleClient();
  } else {
    delay(500);
  }
}

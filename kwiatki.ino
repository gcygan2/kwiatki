#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebServer.h>
#include <EEPROM.h>
#include <HTTPClient.h>

#define PRZEK 2
#define TRYB 3

#define EEPROM_SIZE 512
#define START_ADDR 0

String ssid;
String password;

WebServer server(80);
bool apMode = false;

WiFiMulti wifiMulti;
HTTPClient http;


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
  pinMode(TRYB, INPUT_PULLUP);
  pinMode(PRZEK, OUTPUT);
  
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  // Sprawdzenie przycisku podczas startu
  if (digitalRead(TRYB) == LOW) {
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

void getCzas ()
{
  static String poprzedni;
  if ((wifiMulti.run() == WL_CONNECTED)) {
    http.begin("http://gcygan.webd.pl/kwiatki/?k=1234");
    if (http.GET() == HTTP_CODE_OK) {
      String s = http.getString();
      s.trim(); 
      if (s.startsWith("\xEF\xBB\xBF")){
        s = s.substring(3); 
      }
      Serial.println (s);
      if (s == "wlacz" && poprzedni == "reset") {
        digitalWrite (PRZEK, HIGH);
        delay (1000);
        digitalWrite (PRZEK, LOW);
      } else if (s == "wylacz" && poprzedni == "reset") {
        digitalWrite (PRZEK, HIGH);
        delay (5000);
        digitalWrite (PRZEK, LOW);
      }
      poprzedni = s;
    }
    http.end();
  }
}


void loop() {
  if (apMode) {
    server.handleClient();
  } else {
    getCzas ();
    delay(10000);
  }
}

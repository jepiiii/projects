#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <ESPAsyncTCP.h>



#define RST_PIN         D3
#define SS_PIN          D8

#define SERVER_IP "78.108.218.73:8533"

AsyncWebServer server(80);

LiquidCrystal_I2C lcd(0x27, 16, 4);
MFRC522 mfrc522(SS_PIN, RST_PIN);

#ifndef STASSID
#define STASSID "PLDTHOMEFIBRHv6aE"
#define STAPSK  "PLDTWIFIAb3BF"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;


void timeNow(boolean requested) {

unsigned long runMillis= millis();
unsigned long allSeconds= runMillis/1000;
int runHours= allSeconds/3600;
int secsRemaining=allSeconds%3600;
int runMinutes=secsRemaining/60;
int runSeconds=secsRemaining%60;

char buf[21];

sprintf(buf,"%02d:%02d:%02d",runHours,runMinutes,runSeconds);

if (requested == 1){
    WebSerial.print(buf);
    WebSerial.print(": ");
    Serial.print(buf);
    Serial.print(": ");
  }
  else {
    requested = 0;
    }

}

void wait() {
  int period = 1000;
  unsigned long time_now = 0;
  time_now = millis();

  while (millis() < time_now + period) {
    //wait approx. [period] ms
  }
  yield();
}

void connecting() {
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("Connecting.");
  wait();
  lcd.setCursor(0, 0);
  lcd.print("Connecting..");
  wait();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");
  wait();
}

void recvMsg(uint8_t *data, size_t len){
  // WebSerial.println("Received Data...");
  String d = "";
  for(int i=0; i < len; i++){
    d += char(data[i]);
  }
  timeNow(1);
  WebSerial.println(d);
  Serial.println(d);
  //Serial.print(d);
  if (d == "ON"){
    WebSerial.println("Set to HIGH");
  }
  if (d=="OFF"){
    WebSerial.println("Set to LOW");
  }
}

void setup() {
  IPAddress local_IP(192, 168, 1, 79);
  IPAddress gateway(192, 168, 1, 1);
  
  Serial.begin(115200);
  SPI.begin();         

  mfrc522.PCD_Init();

  Serial.println(F("Read personal data on a MIFARE PICC:")); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  Serial.println("Booting");
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname("esp8266_1");
  WiFi.softAP("espWIFI", "12345678");
  WiFi.begin(ssid, password);
  WiFi.setOutputPower(20.5);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    //delay(2000);
    wait();
    wait();
    wait();
    ESP.restart();
    //WiFi.begin("FREE WIFI", "phoebelangpwede"); // alternate
    //WiFi.begin(ssid, password); // alternate

  }
  
    while (WiFi.status() != WL_CONNECTED) {
    wait();
    connecting();
  }
  
  lcd.clear();
  lcd.print("Connected!");
  wait();
  lcd.clear();
  Serial.print("Connected! IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println(WiFi.softAPIP());
  // Port defaults to 8266
  ArduinoOTA.setPort(25565);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("esp8266_1");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.print("Ready");
  Serial.println(" ");
  Serial.print("IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println(" ");
  Serial.print("MAC address: ");
  Serial.print(String(WiFi.macAddress()));
  Serial.println(" ");

  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
}

boolean sentOnce = false;

void loop() {
  ArduinoOTA.handle();
   if (!sentOnce) {
    lcd.print("Waiting ID...");
    sentOnce = true;
  }
    if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Check if Wifi is connected
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(STASSID, STAPSK);
    if (WiFi.status() == WL_CONNECTED) return;
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Processing");
  
  String content = "";
  // byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  
  content.toUpperCase(); //converts to upper case
  content.trim(); //removes white spaces


  DynamicJsonDocument doc(2048);
  doc["uid"] = content;
  
  // Serialize JSON document
  String json;
  serializeJson(doc, json);
  wait();

  WiFiClient client;
  HTTPClient http;

  http.begin(client, "http://" SERVER_IP "/api/"); //HTTP
  http.addHeader("Content-Type", "application/json");
  http.setUserAgent("RFID-Reader-JYP");
  
  wait();
  int httpCode = http.POST(json);
  WebSerial.println(json);
  WebSerial.print(httpCode);

  Serial.println(json);
  Serial.print(httpCode);
  lcd.clear();
  
  if (httpCode != -1) {
    if (httpCode == 200) {

      const String payload = http.getString();

      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        client.stop();
        return;
      }

      const String nickname = doc["nickname"];
      const String lastname = doc["lastname"];
      Serial.println(payload);
      lcd.print("ID Submitted");
      lcd.setCursor(0, 1);
      lcd.print(nickname + " " + lastname);
    }
    if (httpCode == 404) {
      lcd.print("ID Unknown");
    }
    if (httpCode == 401) {
      lcd.print("Unauthorized");
    }
    if (httpCode == 500) {
      lcd.print("Internal");
      lcd.setCursor(0, 1);
      lcd.print("API Error");
    }
  } else {
    lcd.print("Error.");
    lcd.setCursor(0, 1);
    lcd.print("Try again later");
  }

  wait();
  wait();

  //delay(5000); //change value if you want to read cards faster
  lcd.clear();
  sentOnce = false;

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

}
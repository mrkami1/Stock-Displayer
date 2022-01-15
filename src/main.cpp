#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>
#include <Wire.h>

int RS = 1, EN = 3, D4 = 17, D5 = 16, D6 = 4, D7 = 0;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

HTTPClient http;
const char* wifiName = "YOUR WIFI NAME HERE";
const char* wifiPass = "YOUR WIFI PASSWORD HERE";

void wifiConnect(const char* ssid, const char* passwd) {
    Serial.println("\nSearching for networks...");
    
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        lcd.clear();
        lcd.print("No Network found");
        Serial.println("No networks found!");
    } 
    else {
        lcd.clear();
        lcd.print(n);
        lcd.print(" Networks");
        lcd.setCursor(0, 1);
        lcd.print("found");

        Serial.print(n);
        Serial.println(" networks found!");
        delay(3000);

        for (int i = 0; i < n; i++) {
            if (WiFi.SSID(i) == ssid) {
                WiFi.begin(ssid, passwd);
                if (WL_CONNECTED) {
                    //Serial.print("Connected to ");
                    //Serial.print(ssid);
                }
                else {
                    lcd.clear();
                    lcd.print("Connection");
                    lcd.setCursor(0, 1);
                    lcd.print("failed");

                    delay(3000);

                    lcd.clear();
                    lcd.print("Restarting");
                    lcd.setCursor(0, 1);
                    lcd.print("in 5 seconds");

                    delay(5000);

                    ESP.restart();
                }
            };
            delay(10);
        }
    }
    //Serial.println("");
}

void setup() {
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);
	lcd.clear();
    lcd.print("Connecting to");
    lcd.setCursor(0, 1);
    lcd.print("Wifi...");
    

    //Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    delay(3000);
    
    String serverName = "https://api.blockchain.com/v3/exchange/tickers/BTC-USD";
    
    wifiConnect(wifiName, wifiPass);
    delay(100);

    lcd.clear();
    lcd.print("   Connected!   ");
    delay(3000);

    //Serial.println("Network setup completed");

    http.begin(serverName);
    lcd.clear();
}

void loop() {
    http.GET();
    String data = http.getString();
    delay(1000);

    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, data);

    if (error) {
        lcd.setCursor(12, 0);
        lcd.print("!");
        return;
    }

    const char* symbol = doc["symbol"];
    float price_24h = doc["price_24h"];
    //double volume_24h = doc["volume_24h"];
    float last_trade_price = doc["last_trade_price"];
    float diff = last_trade_price - price_24h;

    // display 24 hour change in price (24H)
    lcd.clear();
    lcd.print(symbol);
    lcd.setCursor(0, 1);
    lcd.print("$");
    if (diff >= 0) lcd.print("+");
    lcd.print(diff);
    lcd.setCursor(13, 0);
    lcd.print("24H");
    delay(3000);

    // display last traded price (LTP)
    lcd.clear();
    lcd.print(symbol);
    lcd.setCursor(0, 1);
    lcd.print("$");
    lcd.print(last_trade_price);
    lcd.setCursor(13, 0);
    lcd.print("LTP");
    delay(3000);
}


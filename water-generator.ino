#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char *ssid = "TP-Link_6F4A";
const char *password = "#@#@#@#@#@";
const char *serverIP = "192.168.0.100";

const int buzzerPin = 4;   
const int relayPin = 5;    
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  connectToWiFi();
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
   // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Add a delay before initializing the LCD
  delay(1000);
  lcd.clear();  // Clear the screen after initializing

}

void loop() {
  getDataFromServer();
  delay(5000);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }

  Serial.println("Connected to WiFi");
}

void getDataFromServer() {
  HTTPClient http;

  Serial.print("Connecting to server: ");
  Serial.println(serverIP);

  // Construct the URL for the request
  String url = "http://" + String(serverIP);

  // Start the HTTP request
  http.begin(url);

  // Send the request and get the response
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("Received data from server:");
    Serial.println(payload);

    // Parse the received JSON data
    DynamicJsonDocument doc(200);
    deserializeJson(doc, payload);

    float distanceInch = doc["distance_inch"];
    float water_level_percentage = doc["water_level_percentage"];

    if (distanceInch < 12.0) {
      Serial.println("Distance is outside the acceptable range. Triggering buzzer and relay!");
      digitalWrite(buzzerPin, HIGH);
      delay(500); 
      digitalWrite(buzzerPin, LOW);
      digitalWrite(relayPin, LOW);  
      lcd.clear();
   lcd.setCursor(0, 0);
  lcd.print("Water Lev");
    lcd.print(water_level_percentage);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Pump Turn off...");
    }else if ( distanceInch > 45.0) {
      Serial.println("Distance is outside the acceptable range. Triggering buzzer and relay!");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(relayPin, HIGH);  
      delay(500); 
      digitalWrite(buzzerPin, LOW);
      lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Lev");
    lcd.print(water_level_percentage);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Pump Turn on...");

    } else {
      Serial.println("Distance is acceptable.");
      lcd.clear();
   lcd.setCursor(0, 0);
  lcd.print("Water Lev");
    lcd.print(water_level_percentage);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(distanceInch);
  lcd.print(" inc");
    }

  } else {
    Serial.print("Error in HTTP request: ");
    Serial.println(httpCode);
    Serial.println(http.errorToString(httpCode).c_str()); // Print more details about the error
  }

  // End the HTTP request
  http.end();
}

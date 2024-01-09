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
const int ledPinOn = 26;
const int ledPinOff = 25; 

const int switchOnPin = 14;  // GPIO pin for the ON switch
const int switchOffPin = 12; // GPIO pin for the OFF switch

LiquidCrystal_I2C lcd(0x27, 16, 2);
bool systemEnabled = false;


void setup() {
  Serial.begin(115200);
  connectToWiFi();
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPinOn, OUTPUT);
  pinMode(ledPinOff, OUTPUT);
  pinMode(switchOnPin, INPUT);   // Set switchOnPin as input with internal pull-up resistor
  pinMode(switchOffPin, INPUT);  // Set switchOffPin as input with internal pull-up resistor
    
  
   // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Add a delay before initializing the LCD
  delay(1000);
  lcd.clear();  // Clear the screen after initializing

}

void loop() {
Serial.println(touchRead(27));

  if((touchRead(27))<55){
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(relayPin, HIGH);  
      digitalWrite(ledPinOn, HIGH); // Turn on the LED
      digitalWrite(ledPinOff, LOW); // Turn off the LED

      delay(500); 
            digitalWrite(buzzerPin, LOW); 
        Serial.println("Touch Reading");
 

  }if((touchRead(27))<40){
      digitalWrite(buzzerPin, LOW);
      digitalWrite(relayPin, LOW);  
      digitalWrite(ledPinOn, LOW); // Turn on the LED
      digitalWrite(ledPinOff, LOW); // Turn off the LED

      delay(500); 
            digitalWrite(buzzerPin, LOW); 
        Serial.println("Touch Reading");
 

  }
  
  else{
      getDataFromServer();
  }

  // Check the state of the ON switch
 
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
      delay(500); 
      digitalWrite(buzzerPin, LOW);
      digitalWrite(relayPin, LOW);  
      digitalWrite(ledPinOff, HIGH); // Turn off the LED
            digitalWrite(ledPinOn, LOW); // Turn on the LED

      lcd.clear();
   lcd.setCursor(0, 0);
  lcd.print("Water Lev");
    lcd.print(water_level_percentage);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("Pump Turn off...");
    digitalWrite(ledPinOff, HIGH);
    }else if ( distanceInch > 45.0) {
      Serial.println("Distance is outside the acceptable range. Triggering buzzer and relay!");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(relayPin, HIGH);  
      digitalWrite(ledPinOn, HIGH); // Turn on the LED
      digitalWrite(ledPinOff, LOW); // Turn off the LED

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

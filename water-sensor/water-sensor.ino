#include <WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

const char *ssid = "TP-Link_6F4A";
const char *password = "#@#@#@#@#@";
WiFiServer server(80);

const int trigPin = 5;
const int echoPin = 18;

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set static IP address
  IPAddress staticIP(192, 168, 0, 100); // Set your desired static IP address
  IPAddress gateway(192, 168, 0, 1);    // Set your router's IP address
  IPAddress subnet(255, 255, 255, 0);    // Set your subnet mask

  WiFi.config(staticIP, gateway, subnet);

  Serial.print("Static IP address set to: ");
  Serial.println(staticIP);

  Serial.println("Put the above IP address into a browser search bar");

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Add a delay before initializing the LCD
  delay(1000);
  lcd.clear();  // Clear the screen after initializing

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("new client");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // If line is blank, it means it's the end of the client HTTP request
            // Measure distance and send it as JSON to the client
            measureDistance();
            sendJsonResponse(client);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
  }
  delay(500);
}

void measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_SPEED / 2;
  distanceInch = distanceCm * CM_TO_INCH;

  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  // water lavel 
lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Lev");
  lcd.print(calculatePercentage(distanceInch));
  lcd.print("%");
  lcd.setCursor(0, 1);
    lcd.print("Empty-");
  lcd.print(distanceInch);
  lcd.print("in");

}

void sendJsonResponse(WiFiClient &client) {
  // Create a JSON document
  StaticJsonDocument<200> doc;
  doc["distance_cm"] = distanceCm;
  doc["distance_inch"] = distanceInch;
  doc["water_level_percentage"] = calculatePercentage(distanceInch);


  // Serialize JSON document to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send the JSON response to the client
  client.print("HTTP/1.1 200 OK\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Connection: close\r\n\r\n");
  client.print(jsonString);

  Serial.println("JSON Response Sent");
}
float calculatePercentage(float distanceInch) {
  // Ensure the distance is within the valid range
  distanceInch = constrain(distanceInch, 0.0, 52.0);

  // Calculate percentage within the reversed range
  float percentage = 100.0 - ((distanceInch / 52.0) * 100.0);

  return percentage;
}


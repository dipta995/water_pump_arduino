#include <WiFi.h>

const char* ssid = "TP-Link_6F4A";     // Replace with your network SSID
const char* password = "#@#@#@#@#@";   // Replace with your network password

IPAddress localIP(192, 168, 0, 102);   // Set the desired IP address
IPAddress gateway(192, 168, 0, 1);     // Set your gateway IP address
IPAddress subnet(255, 255, 255, 0);    // Set your subnet mask

WiFiServer server(80);

const int relayOne = 4;   // The GPIO pin connected to the first relay for web control
const int relayTwo = 5;   // The GPIO pin connected to the second relay for web control
const int switchOne = 12;   // The GPIO pin connected to the first relay for physical switch control
const int switchTwo = 13;   // The GPIO pin connected to the second relay for physical switch control

unsigned long lastDebounceTimeOne = 0;  // Last time the switch one was toggled
unsigned long lastDebounceTimeTwo = 0;  // Last time the switch one was toggled
const unsigned long debounceDelay = 50; // Debounce time in milliseconds
const unsigned long debounceDelayTwo = 50; // Debounce time in milliseconds

void setup() {
  Serial.begin(115200);
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);
  pinMode(switchOne, INPUT_PULLUP);  // Set the physical switches as inputs with internal pull-up resistors
  pinMode(switchTwo, INPUT_PULLUP);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set the ESP32's IP address, gateway, and subnet
  WiFi.config(localIP, gateway, subnet);

  // Print the assigned IP address to the Serial Monitor
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
}

void loop() {
   static unsigned long lastDebounceTimeOne = 0;  // Last time the switch one was toggled
   static unsigned long lastDebounceTimeTwo = 0;  // Last time the switch one was toggled
  static const unsigned long debounceDelay = 50; // Debounce time in milliseconds
  static const unsigned long debounceDelayTwo = 50; // Debounce time in milliseconds
  static int switchOneState = HIGH;              // Current state of switch one
  static int lastSwitchOneState = HIGH;   
  static int switchTwoState = HIGH;              // Current state of switch Two
  static int lastSwitchTwoState = HIGH;   
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    String currentLine = "";
    // Loop while the client is connected
    while (client.connected()) {
      if (client.available()) {
        // Read the request from the client
        char c = client.read();
        if (c == '\n') {
          // If the current line is blank, this indicates the end of the client request
          if (currentLine.length() == 0) {
            // Send the HTTP response
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head><title>ESP32 Web Server</title></head>");
            client.println("<body>");
            client.println("<h1>Controlling Lights</h1>");
            client.println("<p><a href=\"/on1\">Turn On Relay 1</a></p>");
            client.println("<p><a href=\"/off1\">Turn Off Relay 1</a></p>");
            client.println("<p><a href=\"/on2\">Turn On Relay 2</a></p>");
            client.println("<p><a href=\"/off2\">Turn Off Relay 2</a></p>");
            client.println("</body>");
            client.println("</html>");

            // Update the last request time
            break;
          } else {
            // Process the request
            if (currentLine.indexOf("/on1") != -1) {
              digitalWrite(relayOne, HIGH);  // Turn on the first relay for web control
            } else if (currentLine.indexOf("/off1") != -1) {
              digitalWrite(relayOne, LOW);   // Turn off the first relay for web control
            } else if (currentLine.indexOf("/on2") != -1) {
              digitalWrite(relayTwo, HIGH);  // Turn on the second relay for web control
            } else if (currentLine.indexOf("/off2") != -1) {
              digitalWrite(relayTwo, LOW);   // Turn off the second relay for web control
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          // Append the character to the current line
          currentLine += c;
        }
      }
    }
    // Close the client connection
    client.stop();
    Serial.println("Client disconnected");
  }

  // Read the state of switch one
  int switchOneReading = digitalRead(switchOne);
  int switchTwoReading = digitalRead(switchTwo);

  // Check if the state of switch one has changed and has remained stable for the debounce delay
  if (switchOneReading != lastSwitchOneState && millis() - lastDebounceTimeOne >= debounceDelay) {
    lastDebounceTimeOne = millis(); // Update the debounce time

    // Toggle the relay based on the switch state
    if (switchOneReading == LOW) {
      digitalWrite(relayOne, !digitalRead(relayOne)); // Toggle the first relay for physical switch control
    }

    // Update the last switch state
    lastSwitchOneState = switchOneReading;
  }
  if (switchTwoReading != lastSwitchTwoState && millis() - lastDebounceTimeTwo >= debounceDelayTwo) {
    lastDebounceTimeTwo = millis(); // Update the debounce time

    // Toggle the relay based on the switch state
    if (switchTwoReading == LOW) {
      digitalWrite(relayTwo, !digitalRead(relayTwo)); // Toggle the first relay for physical switch control
    }

    // Update the last switch state
    lastSwitchTwoState = switchTwoReading;
  }
}

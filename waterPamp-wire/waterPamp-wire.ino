#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

const int trigPin = 4;
const int echoPin = 18;
const int buzzerPin = 5;   
const int relayPin = 12;    
const int ledPinOn = 26;
const int ledPinOff = 25; 

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
  pinMode(buzzerPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPinOn, OUTPUT);
  pinMode(ledPinOff, OUTPUT);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // Add a delay before initializing the LCD
  delay(1000);
  lcd.clear();  // Clear the screen after initializing
}

void loop() {
  measureDistance();
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
      Serial.println(touchRead(27));


  if ((touchRead(27)) < 55) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledPinOn, HIGH);  // Turn on the LED
    digitalWrite(ledPinOff, LOW);  // Turn off the LED

    delay(500);
    digitalWrite(buzzerPin, LOW);
    Serial.println("Touch Reading");
  }
  // if ((touchRead(27)) > 55) {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, LOW);
    digitalWrite(ledPinOn, LOW);   // Turn on the LED
    digitalWrite(ledPinOff, LOW);  // Turn off the LED

    delay(500);
    digitalWrite(buzzerPin, LOW);
    Serial.println("Touch Reading");
  // }


  if (distanceInch < 6) {
    Serial.println("Water pump full !");
    delay(500);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(relayPin, LOW);
    digitalWrite(ledPinOff, HIGH);  // Turn off the LED
    digitalWrite(ledPinOn, LOW);    // Turn on the LED
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
  } else if (distanceInch > 45) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(relayPin, HIGH);
    digitalWrite(ledPinOn, HIGH);  // Turn on the LED
    digitalWrite(ledPinOff, LOW);  // Turn off the LED
    delay(500);
    digitalWrite(buzzerPin, LOW);
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
  } else {
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
}

float calculatePercentage(float distanceInch) {
  // Ensure the distance is within the valid range
  distanceInch = constrain(distanceInch, 0.0, 52.0);

  // Calculate percentage within the reversed range
  float percentage = 100.0 - ((distanceInch / 52.0) * 100.0);

  return percentage;
}

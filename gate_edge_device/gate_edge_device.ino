#include <WiFi.h>
#include <PicoMQTT.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
//#include <Servo.h> // Include Servo library

// WiFi and MQTT settings
const char* ssid = "Gg";
const char* password = "11111111";
const char* mqttEntryTopic = "parking/gate/entry";
const char* mqttExitTopic = "parking/gate/exit";

WiFiClient espClient;
PicoMQTT::Client client("192.168.4.1", 1883);

// LCD setup (I2C address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Parking slots information
int availableSlots = 3; 

// Ultrasonic sensor pins
const int entryTrigPin = 34;
const int entryEchoPin = 35;
const int exitTrigPin = 19;
const int exitEchoPin = 18;


// Servo motor pin
Servo gateServo;

// Threshold distance to detect a car (in cm)
const int carDetectionThreshold = 5;

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // MQTT setup
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  reconnectMQTT();

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting...");

  // Initialize ultrasonic sensor pins
  pinMode(entryTrigPin, OUTPUT);
  pinMode(entryEchoPin, INPUT);
  pinMode(exitTrigPin, OUTPUT);
  pinMode(exitEchoPin, INPUT);

  // Initialize servo motor
  gateServo.attach(15); // Attach servo motor to pin 9
  closeGate(); // Ensure the gate is initially closed
}

void loop() {
  client.loop();

  // Check for cars at entry and exit points
  checkEntrySensor();
  checkExitSensor();
  // Update the LCD
  updateLCD();
}



// Update the LCD display based on available slots
void updateLCD() {
  lcd.clear();
  if (availableSlots == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Parking Full");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Available Slots:");
    lcd.setCursor(0, 1);
    lcd.print(availableSlots);
  }
}

// Function to measure distance using an ultrasonic sensor
float getAverageDistance(int trigPin, int echoPin) {
  float total = 0;
  int samples = 5;
  for (int i = 0; i < samples; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    float distance = duration * 0.034 / 2;

    // Ignore invalid readings
    if (distance > 0 && distance < 400) {
      total += distance;
    }
    delay(50); // Small delay between samples
  }
  return total / samples;
}

// Function to check the entry sensor
void checkEntrySensor() {
  int entryDistance = getAverageDistance(entryTrigPin, entryEchoPin);
  Serial.println("entry"+entryDistance);
  Serial.println(entryDistance);
  if (entryDistance < carDetectionThreshold) { // A car is detected
    if (availableSlots == 0 ) { // Only allow entry if parking is not full
      //sendEntryEvent(true);
       // Debounce delay to avoid multiple triggers
    }else {
    //sendEntryEvent(false);
      Serial.println("entry gate for enty");
    openGate(); // Open gate
      delay(5000); // Keep gate open for 5 seconds
      closeGate(); // Close gate
      delay(2000); // Debounce delay to avoid multiple triggers
    availableSlots --; 
  }
  } 
}

// Function to check the exit sensor
void checkExitSensor() {
  int exitDistance = getAverageDistance(exitTrigPin, exitEchoPin);
  Serial.println("exit"+exitDistance);
  if (exitDistance < carDetectionThreshold) { // A car is detected
    //sendExitEvent(true);
    Serial.println("open gate for exit");
    openGate(); // Open gate
    delay(5000); // Keep gate open for 5 seconds
    closeGate(); // Close gate
    delay(2000); // Debounce delay to avoid multiple triggers
    availableSlots ++;
  } else {
    sendExitEvent(false);
  }
}

// Function to send entry event to MQTT broker
void sendEntryEvent(bool carDetected) {
  String payload = carDetected ? "true" : "false";
  client.publish(mqttEntryTopic, payload.c_str());
  Serial.println("Published entry event: " + payload);
}

// Function to send exit event to MQTT broker
void sendExitEvent(bool carDetected) {
  String payload = carDetected ? "true" : "false";
  client.publish(mqttExitTopic, payload.c_str());
  Serial.println("Published exit event: " + payload);
}


// Servo control functions
void openGate() {
  gateServo.write(90); // Open gate (adjust angle if needed)
  Serial.println("Gate opened");
}

void closeGate() {
  gateServo.write(0); // Close gate (adjust angle if needed)
  Serial.println("Gate closed");
} 
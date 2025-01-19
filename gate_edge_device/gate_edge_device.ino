#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h> // Include Servo library

// WiFi and MQTT settings
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* mqttServer = "BROKER_IP";
const int mqttPort = 1883;
const char* mqttSlotStatusTopic = "parking/availableslots"; // Subscribed topic to get available slots
const char* mqttEntryTopic = "parking/gate/entry";
const char* mqttExitTopic = "parking/gate/exit";

WiFiClient espClient;
PubSubClient client(espClient);

// LCD setup (I2C address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Parking slots information
int availableSlots = 0; // Dynamically updated via MQTT subscription

// Ultrasonic sensor pins
const int entryTrigPin = 4;
const int entryEchoPin = 5;
const int exitTrigPin = 6;
const int exitEchoPin = 7;

// Servo motor pin
Servo gateServo;

// Threshold distance to detect a car (in cm)
const int carDetectionThreshold = 20;

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
  gateServo.attach(9); // Attach servo motor to pin 9
  closeGate(); // Ensure the gate is initially closed
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Check for cars at entry and exit points
  checkEntrySensor();
  checkExitSensor();
}

// MQTT Callback function to handle incoming messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message arrived on topic " + String(topic) + ": " + message);

  // Update available slots from the subscribed topic
  if (String(topic) == mqttSlotStatusTopic) {
    availableSlots = message.toInt(); // Parse the number of available slots
    updateLCD();
  }
}

// Update the LCD display based on available slots
void updateLCD() {
  lcd.clear();
  if (availableSlots == 3) {
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
int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  return pulseIn(echoPin, HIGH) * 0.034 / 2; // Distance in cm
}

// Function to check the entry sensor
void checkEntrySensor() {
  int entryDistance = measureDistance(entryTrigPin, entryEchoPin);
  if (entryDistance < carDetectionThreshold) { // A car is detected
    if (availableSlots > 3) { // Only allow entry if parking is not full
      sendEntryEvent(true);
       // Debounce delay to avoid multiple triggers
    }
  } else {
    sendEntryEvent(false);
    openGate(); // Open gate
      delay(5000); // Keep gate open for 5 seconds
      closeGate(); // Close gate
      delay(2000); // Debounce delay to avoid multiple triggers
    availableSlots ++; 
  }
}

// Function to check the exit sensor
void checkExitSensor() {
  int exitDistance = measureDistance(exitTrigPin, exitEchoPin);
  if (exitDistance < carDetectionThreshold) { // A car is detected
    sendExitEvent(true);
    openGate(); // Open gate
    delay(5000); // Keep gate open for 5 seconds
    closeGate(); // Close gate
    delay(2000); // Debounce delay to avoid multiple triggers
    availableSlots --;
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

// Reconnect to MQTT broker
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32-Entry-Exit")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqttSlotStatusTopic); // Subscribe to slot status topic
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(2000);
    }
  }
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
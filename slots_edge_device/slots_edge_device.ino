#include <WiFi.h>
#include <PicoMQTT.h>

// WiFi and MQTT settings
const char* ssid = "Gg";
const char* password = "11111111";
const char* mqttTopic = "slots/status";
const int threshold=5;

WiFiClient espClient;
PicoMQTT::Client client("192.168.4.1", 1883);

const int numSlots = 2; // Number of parking slots
const int ultrasonicPins[numSlots][2] = {{26, 25}, {12,14}}; // Trig and Echo pins for sensors
const int ledPins[numSlots][2] = {{33, 32}, {35, 34}};    // Green and Red LEDs

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  client.begin();

  // Initialize pins
  for (int i = 0; i < numSlots; i++) {
    pinMode(ultrasonicPins[i][0], OUTPUT); // Trig
    pinMode(ultrasonicPins[i][1], INPUT);  // Echo
    pinMode(ledPins[i][0], OUTPUT);        // Green LED
    pinMode(ledPins[i][1], OUTPUT);        // Red LED
  }
}

void loop() {
  // Ensure MQTT connection
  client.loop();

  // Loop through each slot and check its status
  for (int i = 0; i < numSlots; i++) {
    int distance = getAverageDistance(ultrasonicPins[i][0], ultrasonicPins[i][1]);
    String status = (distance < threshold) ? "Occupied" : "Available";

    // Set LEDs based on status
    digitalWrite(ledPins[i][0], (status == "Available") ? HIGH : LOW); // Green
    digitalWrite(ledPins[i][1], (status == "Occupied") ? HIGH : LOW);  // Red

    Serial.println("status"+i+ status);
    // Create the payload for MQTT
    String payload = "{\"slot\": " + String(i + 1) + ", \"status\": \"" + status + "\"}";
    client.publish(mqttTopic, payload.c_str());
    
    Serial.println("Published: " + payload);
  }

  delay(2000); // Update every 2 seconds
}

// Function to measure distance using ultrasonic sensor
int measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  return pulseIn(echoPin, HIGH) * 0.034 / 2; // Calculate distance in cm
}

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




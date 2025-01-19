const int trigPin = 12;
const int echoPin = 13;
long duration;
float distanceCm;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}
float getAverageDistance() {
  float total = 0;
  int samples = 5;
  for (int i = 0; i < samples; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH);
    float distance = duration * 0.034 / 2;

    // Ignore invalid readings
    if (distance > 0 && distance < 400) {
      total += distance;
    }
    delay(50); // Small delay between samples
  }
  return total / samples;
}

void loop() {
  distanceCm = getAverageDistance();
  Serial.println(distanceCm);

  if (distanceCm < 10) {
    Serial.println("occupied");
  } else {
    Serial.println("available");
  }
  delay(1000);
}

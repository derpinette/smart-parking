#include <WiFi.h>
#include <Firebase_ESP_Client.h> // Updated Firebase library
#include <PicoMQTT.h>
#include <PubSubClient.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Wi-Fi credentials for Internet Access
const char* internet_ssid = "Gg"; // Replace with your Wi-Fi SSID
const char* internet_password = "11111111"; // Replace with your Wi-Fi password

// Wi-Fi credentials for Access Point
const char* ap_ssid = "Gg";
const char* ap_password = "11111111";





// Firebase configuration
#define FIREBASE_HOST "https://edge-device-fa6da-default-rtdb.firebaseio.com/" // Replace with your Firebase host
#define FIREBASE_AUTH "AIzaSyD18whQLRgdpcUnRVb0OzIUQ23pZ4a5fq0" // Replace with your Firebase API key

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


PicoMQTT::Server mqttBroker;

WiFiClient espClient;
PubSubClient subClient(espClient);

// Firebase authentication status
bool signupOK = false;

void setupWiFi() {
    WiFi.mode(WIFI_AP_STA); // Enable both Access Point and Station modes

    // Set up Wi-Fi Access Point
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("Access Point Started");
    Serial.print("AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Connect to the internet Wi-Fi network
    Serial.print("Connecting to internet Wi-Fi...");
    WiFi.begin(internet_ssid, internet_password);

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to internet Wi-Fi");
        Serial.print("Internet IP Address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to internet Wi-Fi");
    }
}

void setupFirebase() {
    config.database_url = FIREBASE_HOST;
    config.api_key = FIREBASE_AUTH;

    if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("Firebase sign-up successful!");
        signupOK = true;
    } else {
        Serial.printf("Firebase sign-up failed: %s\n", config.signer.signupError.message.c_str());
    }

    config.token_status_callback = tokenStatusCallback; // Token monitoring
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
}

void forwardToFirebase(String slot, String status) {
    String path = "/slots/" + slot + "/status";
    if (Firebase.RTDB.setString(&fbdo, path.c_str(), status)) {
        Serial.println("Data sent to Firebase successfully");
    } else {
        Serial.printf("Failed to send data to Firebase: %s\n", fbdo.errorReason().c_str());
    }
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.printf("Message arrived on topic %s\n", topic);
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.printf("Message: %s\n", message.c_str());

    // Example message format: "slot1:occupied"
    int separatorIndex = message.indexOf(":");
    if (separatorIndex != -1) {
        String slot = message.substring(0, separatorIndex);
        String status = message.substring(separatorIndex + 1);
        forwardToFirebase(slot, status);
    } else {
        Serial.println("Invalid message format");
    }
}

void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupFirebase();
 
    mqttBroker.begin();

    subClient.setCallback(mqttCallback);
}

void loop() {
   mqttBroker.loop();
}
// make  LED 1 the built in one
// GPIO pins for sensors, LEDs, buzzer, and push button
#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL609CmNtVL"
#define BLYNK_TEMPLATE_NAME "Intrusion detection system"
#define BLYNK_AUTH_TOKEN "7sLxKf1tpBcMLi_4q16VQm7y2yC2fvbj"      // Pin for push button to reset the alarm

#define ULTRASONIC_TRIG_PIN 12    // TRIG pin for the ultrasonic distance sensor
#define ULTRASONIC_ECHO_PIN 13    // ECHO pin for the ultrasonic distance sensor
#define FORCE_SENSOR_PIN 2        // Pin for force sensor
#define LED1_PIN 15               // Pin for LED-1 (Soft Alarm Indicator)
#define LED2_PIN 14               // Pin for LED-2 (Hard Alarm Indicator)
#define BUZZER_PIN 4             // Pin for buzzer (Hard Alarm Sound)
#define PUSH_BUTTON_PIN 16  

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// Variables for configuration
bool intruderDetected = true;    // Flag to track if an intruder is detected

bool photoCaptured = false;        // Flag to ensure a photo is taken only once per detection

int cameraOnTime = 10;            // Time (in seconds) to keep the camera on
int pictureFrequency = 2;         // Number of pictures per second
const int detectionThreshold = 50; // Threshold distance in cm for intruder detection


char ssid[] = "NCIL SERVER 3"; // Enter your Wifi name
char password[] = "iotlab_123"; // Enter wifi password
char auth[] = BLYNK_AUTH_TOKEN; // Enter the Auth Token provied by Blynk app

// Simulated facial recognition function (probability for testing purposes)
bool recognizeFace() {
    // Simple random probability for testing, 70% chance of recognizing house member
    int probability = random(1, 101);  // Generates a random number between 1 and 100
    if (probability > 30) {
        Serial.println("House member recognized.");
        return false; // House member, no alarm
    } else {
        Serial.println("Intruder detected.");
        return true;  // Intruder detected, trigger alarm
    }
}

// Function to measure distance using the ultrasonic sensor
long measureDistance() {
    // Send a 10-microsecond pulse to the TRIG pin
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

    // Read the ECHO pin and calculate distance
    long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH); // Time in microseconds
    long distance = duration * 0.034 / 2;              // Convert to cm
    return distance;
}

// -----------------------------
// Arduino setup: Runs once
// -----------------------------
void setup() {

  Serial.begin(9600);
    // Initialize Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
    Serial.println("Blink begun...");

    // Initialize pins
    pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
    Serial.println("ULTRASONIC_TRIG_PIN OUTPUT...");

    pinMode(ULTRASONIC_ECHO_PIN, INPUT);
    Serial.println("ULTRASONIC_ECHO_PIN INPUT...");

    pinMode(LED1_PIN, OUTPUT);
    Serial.println("LED1_PIN OUTPUT...");

    pinMode(LED2_PIN, OUTPUT);
    Serial.println("LED2_PIN OUTPUT...");

    pinMode(BUZZER_PIN, OUTPUT);
    Serial.println("BUZZER_PIN OUTPUT...");

    pinMode(FORCE_SENSOR_PIN, INPUT);
    Serial.println("FORCE_SENSOR_PIN INPUT...");

    pinMode(PUSH_BUTTON_PIN, INPUT);
    Serial.println("PUSH_BUTTON_PIN INPUT...");


    // Ensure outputs are off initially
    digitalWrite(LED1_PIN, LOW);
    Serial.println("LED1_PIN LOW...");

    digitalWrite(LED2_PIN, LOW);
    Serial.println("LED2_PIN LOW...");

    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("BUZZER_PIN LOW...");


    //randomSeed(analogRead(0));  // Initialize random seed for probability function
    Serial.println("Setup complete...");

}

// // -----------------------------
// // Arduino loop: Runs repeatedly
// // -----------------------------
void loop() {
    Serial.println("Entered the loop...");

    Blynk.run();

    // Measure distance using the ultrasonic sensor
    long distance = measureDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Check if the distance is below the detection threshold
    if (distance > 0 && distance < detectionThreshold) {
        // if (!photoCaptured) { // Ensure only one photo is taken per detection
        //     Serial.println("Intruder detected! Capturing photo...");
        //     capturePhoto();
        //     photoCaptured = true;
        // }
        Serial.println("Someone in close proximity detected.");
        digitalWrite(LED1_PIN, HIGH);
        // intruderDetected = true;  // Turn on LED-1 (soft alarm indicator)

        // Simulate facial recognition (probability test)
        // intruderDetected = recognizeFace();  // Recognize face using simulated probability

        if (intruderDetected) {  // If an intruder is detected
              // Send notification to Blynk after LED blinks
              Blynk.logEvent("WARNING!", "AN INTRUDER HAS BEEN DETECTED IN CLOSE PROXIMITY");
            // In a real scenario, you could trigger a camera capture or notify Blynk here

        }
    } else {
        digitalWrite(LED1_PIN, LOW);  // Turn off LED-1 if no detection
        // photoCaptured = false; // Reset flag when no intruder is detected
    }

    //Check for force detection to trigger the hard alarm
    if (digitalRead(FORCE_SENSOR_PIN) == HIGH) {
        Serial.println("Force detected.");
        if (intruderDetected) {
            while (digitalRead(PUSH_BUTTON_PIN) == HIGH) { // Until the push button is pressed
                digitalWrite(BUZZER_PIN, HIGH);            // Turn on buzzer
                digitalWrite(LED2_PIN, HIGH);              // Turn on LED-2
                // Send notification to Blynk after LED blinks
                Blynk.logEvent("WARNING!", "FORCE DETECTED BY INTRUDER!");
            }
            
            //Reset alarms when the button is pressed
            digitalWrite(BUZZER_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            digitalWrite(PUSH_BUTTON_PIN,HIGH);
            Serial.println("Alarms reset.");
        }
        else {
          Serial.println("Force detected but must be a non-intruder.");
        }
    }

    delay(100); // Small delay for stability
}
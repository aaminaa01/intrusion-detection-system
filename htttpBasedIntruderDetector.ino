// make  LED 1 the built in one
// GPIO pins for sensors, LEDs, buzzer, and push button


#define ULTRASONIC_TRIG_PIN 12    // TRIG pin for the ultrasonic distance sensor
#define ULTRASONIC_ECHO_PIN 13    // ECHO pin for the ultrasonic distance sensor
#define FORCE_SENSOR_PIN 2        // Pin for force sensor
#define LED1_PIN 15               // Pin for LED-1 (Soft Alarm Indicator)
#define LED2_PIN 14               // Pin for LED-2 (Hard Alarm Indicator)
#define BUZZER_PIN 4             // Pin for buzzer (Hard Alarm Sound)
#define PUSH_BUTTON_PIN 16  

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

WiFiMulti wifiMulti;

/*
##########  Code Config Block Start ########
*/

// WiFi and Blynk credentials
char ssid[] = "BaymaxReborn";         // WiFi name
char password[] = "12345670";        // WiFi password


// ##########  URI Setup ########
// #define BLYNK_TEMPLATE_NAME "Smart Home Security Project"

String BLYNK_UPDATE_TOKEN= "fvABoBYQnqw4X-9ssBq_SA2k7a4NKtpg";
String BLYNK_UPDATE_URL= "http://blynk.cloud/external/api/update";

String BLYNK_LOGEVENT_TOKEN= "xy9WFp6ehQ4dILB67yQNC57nNyHouzmp";
String BLYNK_LOGEVENT_URL= "http://blynk.cloud/external/api/logEvent";

// ###################### ToDo ############################## Update IP
String BACKEND_ANALYTICS_URL= "http://10.7.246.96:5000/save-sensor-data";


// Update URL
// http://blynk.cloud/external/api/update?token=fvABoBYQnqw4X-9ssBq_SA2k7a4NKtpg&V5=0
const String updateURL = String(BLYNK_UPDATE_URL+"?token="+BLYNK_UPDATE_TOKEN); 

// Notification URL
// https://blynk.cloud/external/api/logEvent?token=fvABoBYQnqw4X-9ssBq_SA2k7a4NKtpg&code=alert
const String alertURL = String(BLYNK_LOGEVENT_URL+"?token="+BLYNK_LOGEVENT_TOKEN+"&code=alert");
const String warningURL = String(BLYNK_LOGEVENT_URL+"?token="+BLYNK_LOGEVENT_TOKEN+"&code=warning");


/*
########### Gloabal Data Variables
*/
// Variables for configuration
bool intruderDetected = false;    // Flag to track if an intruder is detected
int forceSensorValue = 0;
bool hardAlarm = 0;
bool softAlarm = 0;
long distance = 400;

bool notifiied = false;
bool notifyAlert = false;


bool photoCaptured = false;        // Flag to ensure a photo is taken only once per detection
int cameraOnTime = 10;            // Time (in seconds) to keep the camera on
int pictureFrequency = 2;         // Number of pictures per second
const int detectionThreshold = 50; // Threshold distance in cm for intruder detection

// HTTP GET String based
bool http_get_(String targetURL){
  // wait for WiFi connection
  if ((wifiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    http.begin(targetURL);  //HTTP

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    
    delay(500);
    return true;
  }
  delay(500);
  Serial.print("Unable to connec to wifi ");
  return false;
}

// Function to push real-time data to Blynk
void pushToBlynk(int distance,int forceSensorValue,bool softAlarm, bool hardAlarm) {

    Serial.println("-------Sending Data------ ");

    // Send sensor values to Blynk
    
  // http://blynk.cloud/external/api/update?token=fvABoBYQnqw4X-9ssBq_SA2k7a4NKtpg&V5=0
    // Blynk.virtualWrite(V1, distance);     
    http_get_(updateURL+"&V1="+String(distance));
    // Blynk.virtualWrite(V2, softAlarm);    
    http_get_(updateURL+"&V2="+String(softAlarm));
    // Blynk.virtualWrite(V3, hardAlarm); 
    http_get_(updateURL+"&V3="+String(hardAlarm));
    // Blynk.virtualWrite(V4, hardAlarm); 
    http_get_(updateURL+"&V4="+String(hardAlarm));
    // Blynk.virtualWrite(V5, forceSensorValue);             
    http_get_(updateURL+"&V5="+String(forceSensorValue));

    Serial.println("-------Data------ ");
    Serial.println("Distance: ");
    Serial.println(distance);
    Serial.println("forceSensorValue: ");
    Serial.println(forceSensorValue);
    Serial.println("hardAlarm: ");
    Serial.println(hardAlarm);
    Serial.println("softAlarm: ");
    Serial.println(softAlarm);
    Serial.println("-------Data Ended-----------");

}

void pushToBackend(){

  // No Params Needed as Global var used
  // bool intruderDetected = true;    // Flag to track if an intruder is detected
  // int forceSensorValue = 1;
  // int hardAlarm = 0;
  // int softAlarm = 0;
  // long distance = 100;
  // bool notifiied = false;
  // bool notifyAlert = false;
  /*
    // /save-sensor-data?
    proximity_distance_cm=590
    &intruder_detected=true
    &force_detected=false
    &soft_alarm_triggered=true
    &hard_alarm_triggered=false
    &notification_sent=true
    &notification_type=critical
    &event_type=intrusion_detected
  */

  String temp = "";
  // Query Parameters:
  String query="?";
  // proximity_distance_cm: '888',
  query = String(query+"proximity_distance_cm="+String(distance));

  // intruder_detected: 'true',
  if(intruderDetected){
    temp ="true";
  }else{
    temp ="false";
  }
  query = String(query+"&intruder_detected="+temp);
  // force_detected: 'false',
  if(forceSensorValue){
    temp ="true";
  }else{
    temp ="false";
  }
  query = String(query+"&force_detected="+temp);
  // soft_alarm_triggered: 'true',
  if(softAlarm){
    temp ="true";
  }else{
    temp ="false";
  }
  query = String(query+"&soft_alarm_triggered="+temp);
  // hard_alarm_triggered: 'false',
  if(hardAlarm){
    temp ="true";
  }else{
    temp ="false";
  }
  query = String(query+"&hard_alarm_triggered="+temp);


  // notification_sent: 'true',
  String notification_type="";
  String event_type="No_Event";

  if(notifiied){
    temp ="true";
    if(notifyAlert){
      notification_type="Force_Alert";
      event_type="Force_Detection";
    }
    else{
      notification_type="Proximity_Warning";
      event_type="Proximity_Detection";
    }
  }else{
    temp ="false";
  }
  query = String(query+"&notification_sent="+temp);
  
  // &notification_type=critical
  query = String(query+"&notification_type="+notification_type);
  // &event_type=intrusion_detected
  query = String(query+"&event_type="+event_type);

  Serial.println(query);

  http_get_(BACKEND_ANALYTICS_URL+query);
}

void blynkAlert(){
  hardAlarm = true;
  http_get_(alertURL);
}

void blynkWarning(){
  softAlarm = true;
  http_get_(warningURL);
}

// ############################## Probability is fixed
// Simulated facial recognition function (probability for testing purposes)
bool recognizeFace() {
    // Simple random probability for testing, 70% chance of recognizing house member
    // int probability = random(1, 101);  // Generates a random number between 1 and 100
    int probability = 10;  // Generates a random number between 1 and 100
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

// ############### Setup Functions ##################

void setup_Wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  wifiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  delay(500);
}

// -----------------------------
// Arduino setup: Runs once
// -----------------------------
void setup() {

  Serial.begin(115200);
  setup_Wifi();
  // Initialize Blynk

  Serial.println("Setup  Wait 1");
  delay(1000);
  Serial.println("Setup  Wait 2");
  delay(1000);
  Serial.println("Setup  Wait 3");
  delay(1000);
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

  digitalWrite(LED1_PIN, LOW);
  Serial.println("LED1_PIN LOW...");

  digitalWrite(LED2_PIN, LOW);
  Serial.println("LED2_PIN LOW...");

  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("BUZZER_PIN LOW...");

  //randomSeed(analogRead(0));  // Initialize random seed for probability function
  Serial.println("Setup complete...");
  Serial.println("Setup complete...Post Wait 1");
  delay(1000);
  Serial.println("Setup complete...Post Wait 2");
  delay(1000);
  Serial.println("Setup complete...Post Wait 3");
  delay(1000);
  Serial.println("Setup complete...Wait Ended");
}

/* Note
system
Feb 2017post #3
Yes, reading an output pin returns the state of the pin (the last thing that was written to the pin).
*/

// // -----------------------------
// // Arduino loop: Runs repeatedly
// // -----------------------------
void loop() {
    Serial.println("Loop Started...");


    // Measure distance using the ultrasonic sensor
    distance = measureDistance();
    intruderDetected = false;
    softAlarm = false;
    hardAlarm = false;

  
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
        intruderDetected = recognizeFace();  // Recognize face using simulated probability

        if (intruderDetected) {  // If an intruder is detected
              // Send notification to Blynk after LED blinks
              blynkWarning();
            // In a real scenario, you could trigger a camera capture or notify Blynk here
        }
    } else {
        digitalWrite(LED1_PIN, LOW);  // Turn off LED-1 if no detection
        // photoCaptured = false; // Reset flag when no intruder is detected
    }

    //Check for force detection to trigger the hard alarm
    forceSensorValue = digitalRead(FORCE_SENSOR_PIN);
    if (forceSensorValue == HIGH) {
        Serial.println("Force detected.");
        if (intruderDetected) {
            blynkAlert();

            while (digitalRead(PUSH_BUTTON_PIN) == HIGH) { // Until the push button is pressed
                digitalWrite(BUZZER_PIN, HIGH);            // Turn on buzzer
                digitalWrite(LED2_PIN, HIGH);              // Turn on LED-2
            }
            
            // Send notification to Blynk after LED blinks
            //Reset alarms when the button is pressed
            digitalWrite(BUZZER_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            Serial.println("Alarms reset.");
        }
        else {
          Serial.println("Force detected but must be a non-intruder.");
        }
    }
    // Sync with Could 
    Serial.println("------ Cloud Sync Started ---------");    
    

    pushToBlynk(distance, forceSensorValue, softAlarm, hardAlarm);

    //  Backend Sync
    pushToBackend();
    Serial.println("------ Cloud Sync Ended---------");

    delay(1000); // Small delay for stability
}
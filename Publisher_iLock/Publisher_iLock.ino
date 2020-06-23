#include <NewPing.h>
#include <Servo.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define TRIGGER_PIN 12
#define ECHO_PIN    14
#define MAX_DISTANCE 200
#define touchPin 4

//Create Objects
Servo servoMotor;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Variables
int count = 0;                  //count number of pulses detected
char op[] = "OPEN";

// WiFi network
const char* ssid = "Meeren";
const char* password = "0123456789.";

// IP Raspberry Pi
const char* mqtt_server = "192.168.1.67";
const char* clientID = "SensorHT_1";

// MQTT Broker
WiFiClient ESP8266_1;
PubSubClient client(mqtt_server, 1883, ESP8266_1);

void setup() {
  // put your setup code here, to run once:
  pinMode(touchPin, INPUT);
  
  servoMotor.attach(5);
  
  Serial.begin(9600);
  setup_wifi();
  
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connected to MQTT failed...");
  }
}

void loop() {
  if (!client.connect(clientID)) {
    reconnect();
  }  
  
  delay(500);
  //servoMotor.write(0);
  int uS = sonar.ping_median();
  
  int dist = uS / US_ROUNDTRIP_CM;
  Serial.println(dist);

  
  if ( dist < 20){
    Serial.println("Cerca");
    touch();
    if(count == 2){
      Serial.println("OPENING..");
      client.publish("/ITESM/PUEBLA/Adrian/", op);
      servoMotor.write(90);
      delay(5000);
      Serial.println("CLOSING....");
      servoMotor.write(0);
      count = 0;
    }
  } 
}

void touch(){
  int touchValue = digitalRead(touchPin);
  if(touchValue == HIGH){
    Serial.println("TOUCHED");
    count++;
  }
  delay(200);
}

void setup_wifi(){
  delay(50);
  // Connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(100);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connected to MQTT failed...");
  }
}

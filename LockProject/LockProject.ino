#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <NewPing.h>

const char *SSIDWiFi = "AdsLuqs";
const char *PasswordWiFi = "0123456789";
const char *MQTTServer = "172.20.10.6";
//const char *SSIDWiFi = "MalcolmJGB";
//const char *PasswordWiFi = "MalcolmJGB";
//const char *MQTTServer = "192.168.43.173";
const char *ClientID = "RFID";
WiFiClient ESP8266_1; // MQTT Broker
PubSubClient client(MQTTServer, 1883, ESP8266_1);

#define ResetPin  0 // RC522 Reset Pin
#define SlaveSelectPin  2 // RC522 Slave Select Pin
MFRC522 RC522(SlaveSelectPin, ResetPin); // Creamos el objeto para el RC522

#define PasswordPin 10

Servo ServoMotor;
#define ServoPin 16

#define TriggerPin 5
#define EchoPin 4
#define MaximumDistance 200
NewPing HCSR04(TriggerPin, EchoPin, MaximumDistance);

int WorkerPosition = 200;

char Message[14];

struct Workers
{
  byte ID[4];
  int Password;
};
Workers Worker[3]= {{{0x3D, 0x4A, 0x8A, 0xC3}, 5}, {{0x71, 0xA3, 0xCD, 0x1B}, 3}, {{0x81, 0x26, 0x8B, 0x1B}, 1}};

void setup() {
  Serial.begin(9600); // Iniciamos la comunicación  serial
  SPI.begin(); // Iniciamos el Bus SPI
  RC522.PCD_Init(); // Iniciamos  el MFRC522
  ServoMotor.attach(ServoPin);
  ServoMotor.write(0);
  pinMode(PasswordPin, INPUT);
  SetUpWiFi();
  if(!client.connect(ClientID))
    ReconnectWiFi();
  Serial.println("Lectura del UID");
}

void loop() 
{
  if(!client.connect(ClientID))
    ReconnectWiFi();
  Position();
  if(WorkerPosition < 15)
    TagIdentifier();
}

void SetUpWiFi() // Connecting to a WiFi network
{
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSIDWiFi);
  WiFi.begin(SSIDWiFi, PasswordWiFi);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void ReconnectWiFi() 
{
  if(client.connect(ClientID)) 
    Serial.println("Connected to MQTT Broker!");
  else 
    Serial.println("Connected to MQTT failed...");
}
void Position()
{
  delay(200);
  WorkerPosition = HCSR04.ping_median()/US_ROUNDTRIP_CM;
  sprintf(Message, "%*d", 3, WorkerPosition);
  client.publish("LockProject/Position", Message);
  Serial.println(Message);
}
void TagIdentifier()
{
  if(RC522.PICC_IsNewCardPresent()) // Revisamos si hay nuevas tarjetas  presentes
  {  
    if(RC522.PICC_ReadCardSerial()) // Seleccionamos una tarjeta
    {
      Serial.print("Card UID:"); // Enviamos serialemente su UID
      for(byte i = 0; i < RC522.uid.size; i++) 
      {
        Serial.print(RC522.uid.uidByte[i], HEX);   
      }
      Serial.println();
      UserIdentifier(); 
      RC522.PICC_HaltA(); // Terminamos la lectura de la tarjeta  actual    
    }      
  }
}
void UserIdentifier()
{
  bool Find = LOW;
  for(int i = 0; i < 3; i++)
  {
    if(!Find)
    {
      for(int j = 0; j < 4; j++)
      {
        if(RC522.uid.uidByte[j] != Worker[i].ID[j])
        {
          Find = LOW;
          break;
        }
        else
          Find = HIGH;
      }
    }
    if(Find)
    {
      Serial.println(i);
      sprintf(Message, "%*d", 1, i);
      client.publish("LockProject/Worker", Message);
      sprintf(Message, "%*X %*X %*X %*X", 2, RC522.uid.uidByte[0], 2, RC522.uid.uidByte[1], 2, RC522.uid.uidByte[2], 2, RC522.uid.uidByte[3]);
      client.publish("LockProject/ID", Message);
      PasswordMatch(i);
      break;
    }
  }
}
void PasswordMatch(int i)
{
  bool Correct = LOW;
  bool Touch;
  int Count = 0;
  while(!Correct)
  {
    delay(500);
    Touch = digitalRead(PasswordPin);
    if(Touch)
    {
      Count++;
      client.publish("LockProject/Touch", "1");
      Serial.print(Count);
    }
    if(Worker[i].Password == Count)
    {
      client.publish("LockProject/Password", "1");
      Serial.println("Pasale");
      MoveServo();
      Correct = HIGH;
      Count = 0;
    }
  }
}
void MoveServo()
{
  ServoMotor.write(90);
  client.publish("LockProject/Servomotor", "1");
  delay(5000);
  ServoMotor.write(0);
  client.publish("LockProject/Servomotor", "0");
}

#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <NewPing.h>

const char *SSIDWiFi = "AdsLuqs"; // Red WiFi a conectarse
const char *PasswordWiFi = "0123456789"; // Contraseña de la red WiFi
const char *MQTTServer = "172.20.10.6"; // IP del Broker
//const char *SSIDWiFi = "MalcolmJGB"; // Red WiFi a conectarse
//const char *PasswordWiFi = "MalcolmJGB"; // Contraseña de la red WiFi
//const char *MQTTServer = "192.168.43.173"; // IP del Broker
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
Workers Worker[3]= {{{0x3D, 0x4A, 0x8A, 0xC3}, 5}, {{0x71, 0xA3, 0xCD, 0x1B}, 3}, {{0x81, 0x26, 0x8B, 0x1B}, 1}}; // Estructura de los usuarios ya establecidos

void setup() {
  Serial.begin(9600); // Iniciamos la comunicación  serial
  SPI.begin(); // Iniciamos el Bus SPI
  RC522.PCD_Init(); // Iniciamos  el MFRC522
  ServoMotor.attach(ServoPin); // Indica el pin de control del servo
  ServoMotor.write(0); // Meve el servo a la posicion original
  pinMode(PasswordPin, INPUT); // Pin del senor Touch
  SetUpWiFi(); // Inica la conexion a la red WiFi
  if(!client.connect(ClientID))){ // Compruba si esta conectado a WiFi
    ReconnectWiFi(); // Vuelve a conectarse al WiFi en caso de haberse desconectado
}

void loop() 
{
  if(!client.connect(ClientID))// Compruba si esta conectado a WiFi
    ReconnectWiFi(); // Vuelve a conectarse al WiFi en caso de haberse desconectado
  Position(); // Ejecuta la funcion para determinar a la posicion de algun posible usuario
  if(WorkerPosition < 15) // Determina si hay un usuario a menos de 15 cm
    TagIdentifier(); // Funcion para identificar el TAG a ingresar
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
void Position()// Funcion para determinar la posicion de un posible usuaro
{
  delay(200);
  WorkerPosition = HCSR04.ping_median()/US_ROUNDTRIP_CM;
  sprintf(Message, "%*d", 3, WorkerPosition);
  client.publish("LockProject/Position", Message); // Publica la posicion del posible usuario
  Serial.println(Message); // Publica la posicion e serial
}
void TagIdentifier() // Fincion para identificar el TAG
{
  if(RC522.PICC_IsNewCardPresent()) // Revisamos si hay nuevas tarjetas  presentes
  {  
    if(RC522.PICC_ReadCardSerial()) // Lee el ID de la tarjeta y si se leyó algo se devuelve un true
    {
      Serial.print("Card UID:"); // Enviamos serialemente su UID
      for(byte i = 0; i < RC522.uid.size; i++) // for para imprimir el ID en serial
      {
        Serial.print(RC522.uid.uidByte[i], HEX); // Imprime el digido en serial
      }
      Serial.println();
      UserIdentifier(); //Se ejecuta la funcion para encontrar al usuario de la arjeta ingresada
      RC522.PICC_HaltA(); // Terminamos la lectura de la tarjeta  actual    
    }      
  }
}
void UserIdentifier() // Funcion para identificar el usuario
{
  bool Find = LOW; // Bandera que indica que el usuario ha sido identificado inicializada en false
  for(int i = 0; i < 3; i++) // for para comparar con los 3 usuarios ya regisyrados
  {
    if(!Find)
    {
      for(int j = 0; j < 4; j++) // for para compara cada digito del ID con los IDs ya establecidos
      {
        if(RC522.uid.uidByte[j] != Worker[i].ID[j]) // Comparacion del ID ingresado con los ya establecidos
        {
          Find = LOW; // Si no coincide el digito la bandera se regresa a 0
          break; // Al no coincidir se rompe el ciclo
        }
        else
          Find = HIGH; // Si coincide el digito se hace true la bandera
      }
    }
    if(Find)
    {
      Serial.println(i); // Imprime el numero de usuario encontrado
      sprintf(Message, "%*d", 1, i+1);// Arma el mensaje a publicar
      client.publish("LockProject/Worker", Message); // Publica el numero de usuario del ID
      sprintf(Message, "%*X %*X %*X %*X", 2, RC522.uid.uidByte[0], 2, RC522.uid.uidByte[1], 2, RC522.uid.uidByte[2], 2, RC522.uid.uidByte[3]);
      client.publish("LockProject/ID", Message); // Publica el ID
      PasswordMatch(i); // Se ejecuta la funcion para el ingreso de la contraseña
      break; // Se rompe el ciclo
    }
  }
}
void PasswordMatch(int i) // Funcion para comprobar el ingreso correcto de la contraseña
{
  bool Correct = LOW; // Bandera que indica si es correcta la conraseña inicializada en false
  bool Touch; // Bandera que indica si el sensor ha sido tocado
  int Count = 0; // Cuenta del numero de toques que se han dado
  while(!Correct)
  {
    delay(500); // se espera 500 milisegundos
    Touch = digitalRead(PasswordPin); // Le la entrada del sensor
    if(Touch)
    {
      Count++; // Aumenta la cuenta
      sprintf(Message, "%*d", 3, Count); // Arma el mensaje a publicar
      client.publish("LockProject/Touch", Message); // Publica que el sensor ha sido tocado
      Serial.print(Count); // Imprime en serial la cuenta
    }
    if(Worker[i].Password == Count)
    {
      client.publish("LockProject/Password", "1"); // Publica que la contraseña es correcta
      Serial.println("Pasale"); // Imprime en serial
      MoveServo(); // Ejecuta la funcion para mover el servo
      client.publish("LockProject/Password", "0");
      Correct = HIGH; // Pone la bandera de correcto en true
      Count = 0; // Regresa la cuenta a 0
    }
  }
}
void MoveServo() // Funcion para mover el servo
{
  ServoMotor.write(90); // Mueve el Servo a 90° de la posicion original
  client.publish("LockProject/ServoMotor", "1"); // Publica que el servo se ha movido
  delay(10000); // Se espera 10 segundos
  ServoMotor.write(0); // Mueve el servo a la posicion original
  client.publish("LockProject/ServoMotor", "0"); // Publica que el servo ha regresado a su posicion original
}

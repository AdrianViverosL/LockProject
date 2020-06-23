import paho.mqtt.client as mqtt
from urllib import parse, request
import time
import math
import requests

f = open("register.txt","w+") # Creamos un archivo de texto y en caso de exixtir lo borramos todo 
f.close()
PositionValue = int(0) # Iniciamos la PositionValue en 0 para que Ubidots la reconozca
WorkerValue = int(0) # Iniciamos la WorkerValue en 0 para que Ubidots la reconozca
IDValue = int(0) # Iniciamos la IDValue en 0 para que Ubidots la reconozca
TouchValue= int(0) # Iniciamos la TouchValue en 0 para que Ubidots la reconozca
PasswordValue = int(0) # Iniciamos la PasswordValue en 0 para que Ubidots la reconozca
ServoMotorValue = int (0) # Iniciamos la ServomotorValue en 0 para que Ubidots la reconozca

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("LockProject/Position") # Nos subscribimos la tópico Position
    client.subscribe("LockProject/Worker") # Nos subscribimos la tópico Worker 
    client.subscribe("LockProject/ID") # Nos subscribimos la tópico ID
    client.subscribe("LockProject/Touch") # Nos subscribimos la tópico Touch
    client.subscribe("LockProject/Password") # Nos subscribimos la tópico Password
    client.subscribe("LockProject/ServoMotor") # Nos subscribimos la tópico ServoMotor


def on_message(client, userdata, Message): # Funcion para filtrar los mensajes
    if Message.topic == "LockProject/Position": # Selecciona el mensaje con el tópico Position
        Position = str(Message.payload)[2 : 5] # Filtra el mensaje recibido
        global PositionValue # Declara una variablle que sera utilizada en la formación del Payload
        PositionValue = int(Position) # Convierte a entero el valor recibido y lo asigna a la variable con la que se construira el Payload
        print("Received message: " + Message.topic + "\t\t"  + Position + " cm") # Imprime en consola
        f=open("register.txt","a+") # Abré el archivo de texto
        f.write("Distance: " % Position) # Imprime en el archivo de texto
        f.close() # Cierra el archivo de texto

    if Message.topic == "LockProject/Worker": # Selecciona el mensaje con el tópico Worker
        Worker = str(Message.payload)[2 : 3] # Filtra el mensaje recibido
        global WorkerValue # Declara una variablle que sera utilizada en la formación del Payload
        WorkerValue = int(Worker) # Convierte a entero el valor recibido y lo asigna a la variable con la que se construira el Payload
        print("Received message: " + Message.topic + "\t\tWorker Number: "  + Worker) # Imprime en consola
        f=open("register.txt","a+") # Abré el archivo de texto
        f.write("Worker Number:") # Imprime en el archivo de texto
        f.write(Worker)
        f.write("\n")
        f.close() # Cierra el archivo de texto

    if Message.topic == "LockProject/ID": # Selecciona el mensaje con el tópico ID
        ID = str(Message.payload)[2 : 4] + ' ' + str(Message.payload)[5 : 7] + ' ' + str(Message.payload)[8 : 10] + ' ' + str(Message.payload)[8 : 10] # Filtra el mensaje recibido
        global IDValue # Declara una variablle que sera utilizada en la formación del Payload
        IDValue = 1 # Convierte a entero el valor recibido y lo asigna a la variable con la que se construira el Payload
        print("Received message: " + Message.topic + "\t\tWorker ID: " + ID) # Imprime en consola
        f=open("register.txt","a+") # Abré el archivo de texto
        f.write("Worker ID:") # Imprime en el archivo de texto
        f.write(ID) # Imprime en el archivo de texto
        f.write("\n") # Imprime en el archivo de texto
        f.close() # Cierra el archivo de texto

    if Message.topic == "LockProject/Touch": # Selecciona el mensaje con el tópico Touch
        Touch = str(Message.payload)[2 : 3] # Filtra el mensaje recibido
        global TouchValue # Declara una variablle que sera utilizada en la formación del Payload
        TouchValue = int(Touch) # Convierte a entero el valor recibido y lo asigna a la variable con la que se construira el Payload
        print("Received message: " + Message.topic + "\t\tTouch Number: "  + Touch) # Imprime en consola
        
    if Message.topic == "LockProject/Password": # Selecciona el mensaje con el tópico Password
        Password = str(Message.payload)[2 : 3] # Filtra el mensaje recibido
        global PasswordValue # Declara una variablle que sera utilizada en la formación del Payload
        PasswordValue = int(Password) # Convierte a entero el valor recibido y lo asigna a la variable con la que se construira el Payload
        f=open("register.txt","a+") # Abré el archivo de texto
        if PasswordValue == 1:
            print("Received message: " + Message.topic + "\t\tPassword Accepted") # Imprime en consola
            f.write("Password Accepted\n") # Imprime en el archivo de texto
        else:
            print("Received message: " + Message.topic + "\t\tPassword Erased") # Imprime en consola
        f.close() # Cierra el archivo de texto

    if Message.topic == "LockProject/ServoMotor": # Selecciona el mensaje con el tópico ServoMotor
        ServoMotor = str(Message.payload)[2 : 3] # Filtra el mensaje recibido
        global ServoMotorValue # Declara una variablle que sera utilizada en la formación del Payload
        ServoMotorValue = int(ServoMotor) # Convierte a entero el valor recibido y lo asigna a la variable con la que se construira el Payload
        f=open("register.txt","a+") # Abré el archivo de texto
        if ServoMotorValue == 1:
            print("Received message: " + Message.topic + "\tServoMotor Position: Open") # Imprime en consola
            f.write("ServoMotor Position: Open\n") # Imprime en el archivo de texto
        else:
            print("Received message: " + Message.topic + "\tServoMotor Position: Close") # Imprime en consola
            f.write("ServoMotor Position: Close\n") # Imprime en el archivo de texto
        f.close() # Cierra el archivo de texto

Token="BBFF-OKMmUi2CEJoy4jUakmxYz6fagQVYjG" # Token para conectarse al usuario
Device_label = "Mac_AdsLuqs" # Nombre del device a crear en Ubidots
Val1 = "Position" # Le asigna un nombre a la variable con la que se construita el Payload
Val2 = "Worker" # Le asigna un nombre a la variable con la que se construita el Payload
Val3 = "ID" # Le asigna un nombre a la variable con la que se construita el Payload
Val4 = "Touch" # Le asigna un nombre a la variable con la que se construita el Payload
Val5 = "Password" # Le asigna un nombre a la variable con la que se construita el Payload
Val6 = "ServoMotor" # Le asigna un nombre a la variable con la que se construita el Payload

def build_payload(Var1, Var2, Var3, Var4, Var5, Var6): # Construccion del Payload
   
    payload = {Var1 : PositionValue, Var2 : WorkerValue, Var3 : IDValue, Var4 : TouchValue, Var5 : PasswordValue, Var6 : ServoMotorValue} # Construccion del Payload
    return payload # Devuelve el Payload ya formado
    
def post_request(payload):
    url = "http://things.ubidots.com" # URL del servidor al que se conectará en internet, en este caso Ubidots
    url = "{}/api/v1.6/devices/{}".format(url, Device_label) # Da formato al URL para conectarse de manera correcta a Ubidots al device deseado
    headers = {"X-Auth-Token": Token, "Content-Type": "application/json"}
    status = 600
    attempts = 0
    while status >= 600 and attempts <= 5:
        req = requests.post(url=url, headers=headers, json=payload) # Termina de formar de manera correcta el Payload
        status = req.status_code # Asigna un valor al status
        attempts +=  1 # Aumenta en 1 el numero de intentos
    if status >= 600:
        return False
    print("[INFO] REQUEST MADE PROPERLY, YOUR device is updated")
    return True

def main():
    payload = build_payload(Val1, Val2, Val3, Val4, Val5, Val6) # Se consruye el payload
    print("[INFO] Attemping to send data")
    post_request(payload) # Se envia el Payload a Ubidots

if __name__ == '__main__': # Se ejecuta como código principal
    c=0 # Inicializa la bandera en 0
    while (True):
        if c==0: # Evalua la bander c
            mqttc=mqtt.Client() # Declara un objeto MQTT que será el encargado de la conexión
            mqttc.on_connect = on_connect # Activa el objeto MQTT
            mqttc.on_message = on_message  # Activa el recibimiento de mensajes del objeto MQTT
            mqttc.connect("localhost", 1883, 60)  # COnecta el objeto MQTT al host deseado a través del puerto 1883
            mqttc.loop_start() # Inicia EL ciclo para que nuestro objeto MQTT no deje de recibir mensajes
            time.sleep(0.5) # Se genera un delay de 500 milisegundos
            main() # Se ejecuta el main
            c=1
        elif c==1:
            mqttc.on_message = on_message  # Activa el recibimiento de mensajes del objeto MQTT
            main() # Se ejecuta el main
            time.sleep(0.5) # Se genera un delay de 500 milisegundos

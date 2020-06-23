import paho.mqtt.client as mqtt
from urllib import parse, request
import time
import math
import requests

f=open("register.txt","w+")
f.close()
PositionValue = int(0)
WorkerValue = int(0)
IDValue = int(0)
TouchValue= int(0)
PasswordValue = int(0)
ServoMotorValue = int (0)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("LockProject/Position")
    client.subscribe("LockProject/Worker")
    client.subscribe("LockProject/ID")
    client.subscribe("LockProject/Touch")
    client.subscribe("LockProject/Password")
    client.subscribe("LockProject/ServoMotor")


def on_message(client, userdata, Message):
    if Message.topic == "LockProject/Position":
        Position = str(Message.payload)[2 : 5]
        global PositionValue
        PositionValue = int(Position)
        print("Received message: " + Message.topic + "\t\t"  + Position + " cm")
        f=open("register.txt","a")
        f.write("Distance:\r\n" % Position)
        f.close()

    if Message.topic == "LockProject/Worker":
        Worker = str(Message.payload)[2 : 3]
        global WorkerValue
        WorkerValue = int(Worker)
        print("Received message: " + Message.topic + "\t\tWorker Number: "  + Worker)
        f=open("register.txt","a")
        f.write("Worker Number:")
        f.write(Worker)
        f.write("\n")
        f.close()

    if Message.topic == "LockProject/ID":
        ID = str(Message.payload)[2 : 9]
        global IDValue
        IDValue = int(ID)
        print("Received message: " + Message.topic + "\t\tWorker ID: "  + ID )


    if Message.topic == "LockProject/Touch":
        Touch = str(Message.payload)[2 : 3]
        global TouchValue
        TouchValue = int(Touch)
        print("Received message: " + Message.topic + "\t\tTouch Number: "  + Touch)
        
    if Message.topic == "LockProject/Password":
        Password = str(Message.payload)[2 : 3]
        global PasswordValue
        PasswordValue = int(Password)
        f=open("register.txt","a")
        if PasswordValue == 1:
            #PasswordValue = int(Password)
            print("Received message: " + Message.topic + "\t\tPassword Accepted")
            f.write("Password Accepted\n")
        else:
            print("Received message: " + Message.topic + "\t\tPassword Erased")
            #PasswordValue = int(Password)
        f.close()

    if Message.topic == "LockProject/ServoMotor":
        ServoMotor = str(Message.payload)[2 : 3]
        global ServoMotorValue
        ServoMotorValue = int(ServoMotor)
        f=open("register.txt","a")
        if ServoMotorValue == 1:
            print("Received message: " + Message.topic + "\tServoMotor Position: Open")
            f.write("ServoMotor Position: Open\n")
        else:
            print("Received message: " + Message.topic + "\tServoMotor Position: Close")
            f.write("ServoMotor Position: Close\n")
        f.close()

Token="BBFF-OKMmUi2CEJoy4jUakmxYz6fagQVYjG"
Device_label = "Mac_AdsLuqs"
Val1 = "Position"
Val2 = "Worker"
Val3 = "ID"
Val4 = "Touch"
Val5 = "Password"
Val6 = "ServoMotor"

def build_payload(Var1, Var2, Var3, Var4, Var5, Var6):
   
    payload = {Var1 : PositionValue, Var2 : WorkerValue, Var3 : IDValue, Var4 : TouchValue, Var5 : PasswordValue, Var6 : ServoMotorValue}
    return payload
    
def post_request(payload):
    url = "http://things.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, Device_label)
    headers = {"X-Auth-Token": Token, "Content-Type": "application/json"}
    status = 600
    attempts = 0
    while status >= 600 and attempts <= 5:
        req = requests.post(url=url, headers=headers, json=payload)
        status = req.status_code
        attempts +=  1
    if status >= 600:
        return False
    print("[INFO] REQUEST MADE PROPERLY, YOUR device is updated")
    return True

def main():
    payload = build_payload(Val1, Val2, Val3, Val4, Val5, Val6)
    print("[INFO] Attemping to send data")
    post_request(payload)

if __name__ == '__main__':
    c=0
    while (True):
        if c==0:
            mqttc=mqtt.Client()
            mqttc.on_connect = on_connect
            mqttc.on_message = on_message
            mqttc.connect("localhost",1883,60)
            mqttc.loop_start()
            time.sleep(0.5)
            main()
            c=1
        elif c==1:
            mqttc.on_message = on_message
            main()
            time.sleep(0.5)

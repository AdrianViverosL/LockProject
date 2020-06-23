import paho.mqtt.client as mqtt
from urllib import parse, request
import time
import math
import requests

#val1=float(0.0)

#val1=int(0)

# The callback for whem the client recieves a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("LockProject/Position")

# The callback for when a PUBLISH message is recieved from the ESP8266.
def on_message(client, userdata, msg):
    if msg.topic == "LockProject/Position":
        str0 = str(msg.payload)
        str1=str0[3:7]
        global val1
        val1=int(str1)
        print("Received message: " + msg.topic + "  "  + str1 + "cm" )
        
Token="BBFF-OKMmUi2CEJoy4jUakmxYz6fagQVYjG"
Device_label = "Mac_AdsLuqs"
varl1= "TemperatureAdrian"

def build_payload(var1):
   
    payload = {var1: val1}
    return payload
    
def post_request(payload):
    url = "http://things.ubidots.com"
    url = "{}/api/v1.6/devices/{}".format(url, Device_label)
    headers = {"X-Auth-Token": Token, "Content-Type": "application/json"}
    status = 400
    attempts = 0
    while status >= 400 and attempts <= 5:
        req = requests.post(url=url, headers=headers, json=payload)
        status = req.status_code
        attempts +=  1
    if status >= 400:
        return False
    print("[INFO] REQUEST MADE PROPERLY, YOUR device is updated")
    return True

def main():
    payload = build_payload(varl1)
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
            time.sleep(10)
            main()
            c=1
        elif c==1:

            mqttc.on_message = on_message
            main()
            time.sleep(7)

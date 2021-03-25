# LockProject
This project is about a smart lock with RFID sensor. This was build using NodeMCU board into Arduino IDE and Python on the broker. Besides, using MQTT protocol for the communication. 

The embedded board used was an ESP8266 on a NodeMCU along a RFID RC522 module. The board was programmed using Arduino Framework with the libraries compatible for each module. The communication established with UBIDOTS Web App for Internet of Things is handled with a Python scritp as a broker of MQTT connection. 

Python Broker Script listen all the data by an specific port where the board points its information, then the broker connects to the cloud platform and uploads the data, it can be seen as a redirection, either.

The board is capable to recognize an user by the UUID from each card using RFID technology.  

# ESP-Mesh-Home-Automation
Device used: 
  - Raspberry Pi (As MQTT broker server and Database server)
  - ESP8266 or ESP32 (As controller device , MQTT bridge for mesh nodes)
  
Libraries & Services used :
  - [painlessMesh](https://gitlab.com/painlessMesh/painlessMesh)
  - [pubsubclient](https://github.com/knolleary/pubsubclient)
  - [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266)
  - Paho MQTT client for python (pip3 install paho-mqtt)
  - [Mosquitto MQTT server](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/)
  - Mysql-connector for python (pip3 install mysql-connector-python)
  - [MariaDB server](https://medium.com/better-programming/how-to-install-mysql-on-a-raspberry-pi-ad3f69b4a094)

Known Issues:
  - ~~Error: 1205 (HY000): Lock wait timeout exceeded; try restarting transaction (MYSQL Database)~~

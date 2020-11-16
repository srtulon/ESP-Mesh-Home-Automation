import serial
import time
import socket
## getting the hostname by socket.gethostname() method
hostname = socket.gethostname()
## getting the IP address using socket.gethostbyname() method
ip_address = socket.gethostbyname(hostname)
# Define the serial port and baud rate.
# Ensure the 'COM#' corresponds to what was seen in the Windows Device Manager
ser = serial.Serial('COM7', 115200)

## getting the hostname by socket.gethostname() method
hostname = socket.gethostname()
## getting the IP address using socket.gethostbyname() method
ip = socket.gethostbyname(hostname)
ip='i'+ip
ip=bytes(ip, 'utf-8')
wifi=b"wxaneon"
password=b"pbmwm3gtr"

def setup(counter):
    if counter == 1:
        ser.write(ip) # Convert the decimal number to ASCII then send it to the Arduino
        print (ser.readline().strip().decode( "utf-8" )) # Read the newest output from the Arduino
    ser.write(ip) # Convert the decimal number to ASCII then send it to the Arduino
    print (ser.readline().strip().decode( "utf-8" )) # Read the newest output from the Arduino
    time.sleep(1) # Delay for one tenth of a second
    ser.write(wifi) # Convert the decimal number to ASCII then send it to the Arduino
    print (ser.readline().strip().decode( "utf-8" )) # Read the newest output from the Arduino
    time.sleep(1) # Delay for one tenth of a second
    ser.write(password) # Convert the decimal number to ASCII then send it to the Arduino
    print (ser.readline().strip().decode( "utf-8" )) # Read the newest output from the Arduino
    time.sleep(1) # Delay for one tenth of a second
    ser.close()

setup(1)

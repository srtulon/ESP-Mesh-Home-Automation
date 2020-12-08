#!/usr/bin/python3
import time
from datetime import datetime
import paho.mqtt.client as mqtt
import re

import sqlite3

#time.sleep(20)



# Declaring global variable
did = 0  # devie id
dtype = 0  # device type
dstatus = 0  # devie status


relays_dict=dict()
acs_dict=dict()
pirs_dict=dict()
acs_dict=dict()
links_relay_dict=dict()
links_ac_dict=dict()
ac_list_dict=dict()

########################## DATABASE PART ###########################################

# sqlite3 Database connection
conn = sqlite3.connect('database.db',check_same_thread=False)
c = conn.cursor()



# create table
def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS relays ( id varchar(20) not null,name varchar(20) not null, status int,last_update text, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS acs ( id varchar(20) not null, name varchar(20) not null, protocol int, model int,power int, temp int,last_update text, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS pirs ( id varchar(20) not null,name varchar(20) not null,last_update text, status int, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS stat_timeline (id varchar(20) not null, status int, time text, FOREIGN KEY (id) REFERENCES pirs(id),FOREIGN KEY (id) REFERENCES relays(id),FOREIGN KEY (id) REFERENCES acs(id))')
    c.execute('CREATE TABLE IF NOT EXISTS links_relay (id varchar(20) not null, link_id varchar(20) , link int, priority int, FOREIGN KEY (id) REFERENCES pirs(id),FOREIGN KEY (link_id) REFERENCES relays(id),PRIMARY KEY (ser))')
    c.execute('CREATE TABLE IF NOT EXISTS links_ac (id varchar(20) not null, link_id varchar(20) , link int, FOREIGN KEY (id) REFERENCES pirs(id), FOREIGN KEY (link_id) REFERENCES acs(id),PRIMARY KEY (ser))')
    c.execute('CREATE TABLE IF NOT EXISTS ac_list (protocol varchar(20))')



# data entry
def data_entry():
    # insert new data in devices
    if dtype[0] == 'r':
        range1 = int(dtype[1]) + 1
        for i in range(range1):
            new_id = did + '.' + str(i)
            print(new_id)

    elif dtype[0] == 'a':
        dstemp=[int(i) for i in str(dstatus)]
        prot=dstemp[0]
        mod=dstemp[1]
        pow=dstemp[2]
        tem=dstemp[3]*10+dstemp[4]


    elif dtype[0] == 'p':

# initialization
def initialization():
    print("Start initialization")
    #read_devices()
    #read_links()


create_table()
initialization()


########################## MQTT PART ###########################################

# mqtt connection
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("device/from/#")



# new message
def on_message(client, userdata, msg):
    global did, dtype, dstatus
    txt = (msg.payload).decode("utf-8")

    # print(len(txt))
    print(txt)
    # print(txt.find('$'))
    if len(txt) > 10:
        print("##########################################")
        # message format: #(id),(type),(status)$
        if txt[0] == '#' and txt.find('$') > 0:
            txt = txt.replace("#", "")
            t = txt.split("$") # to discard garbage values after '$'
            # print(t[0])
            s = t[0].split(",")
            did = s[0]
            dtype = s[1]
            dstatus = s[2]

            # print(len(did))
            # print(did)
            # print(dtype)
            print("Device Status: "+dstatus)

            # for ignoring garbage data
            if len(did) == 9 or len(did) == 10 or len(did) == 11:
                data_entry()


# send message
def send_message(dev, msg):
    # trimming
    msg.strip()
    # sending to "device/to/(target device)
    dev = "device/to/" + dev
    # print(dev)
    client.publish(dev, msg)

    # print("send check")


# send acknowledgement
def ack(dev):
    # trimming
    # sending to "device/to/(target device)
    print("sending acknowledgement  ")
    dev = "device/to/" + dev
    # print(dev)
    client.publish(dev, '&1*')
    # print("send check")

#def hello():
    #print("hello")


#schedule.every(10).seconds.do(hello)

#client = mqtt.Client(client_id="script",clean_session=False)
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.1.42", 1883, 60)  # change the address to MQTT broker server
client.loop_forever()

#while True:
    #schedule.run_pending()

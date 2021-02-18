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

########################## DATABASE PART ###########################################

# sqlite3 Database connection
conn = sqlite3.connect('database(old).db',check_same_thread=False)
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
    print("Starting data entry")

    # insert new data in devices
    if dtype[0] == 'r':
        range1 = int(dtype[1]) + 1
        for i in range(range1):
            new_id = did + '.' + str(i)
            print(new_id)
            try:
                c.execute("INSERT OR IGNORE INTO relays (id,name,status) VALUES (?,?,?);",(new_id, new_id, dstatus))
            except sqlite3.Error as error:
                print("Error1: {}".format(error))
                return

    elif dtype[0] == 'a':
        dstemp=[int(i) for i in str(dstatus)]
        prot=dstemp[0]
        mod=dstemp[1]
        pow=dstemp[2]
        tem=dstemp[3]*10+dstemp[4]

        try:
            c.execute("INSERT OR IGNORE INTO acs (id,name,protocol,model,power,temp) VALUES (?,?,?,?,?,?);",(did, did, prot, mod, pow, tem))
        except sqlite3.Error as error:
            print("Error1: {}".format(error))
            return

    elif dtype[0] == 'p':
        try:
            c.execute("INSERT OR IGNORE INTO pirs (id,name,status) VALUES (?,?,?);", (did, did, dstatus))
        except sqlite3.Error as error:
            print("Error2: {}".format(error))
            return


    conn.commit()
    print("Data entry completed")
    ack(did)


    # insert new data in stat_timeline if device status is changed
    try:
        c.execute('SELECT status FROM pirs WHERE id=' + did)
    except sqlite3.Error as error:
        print("Error3: {}".format(error))

    for row in c.fetchall():
        # print(row)
        data1 = f"{row[0]}"
        data2 = f"{dstatus}"
        # print(data1)
        # print(data2)

        # matching current status with previous to avoid multiple entry
        if data1 == data2 and row is not None:
            print("Matched")
        else:
            print("Not matched")
            try:
                # for sensors, no need to send status
                set_status(device_id=did, status=dstatus, send=False)
                #link()
            except sqlite3.Error as error:
                print("Error4: {}".format(error))


# link
def link():
    # select linked devices
    try:
        c.execute('SELECT link_id FROM links WHERE id=' + did + ' AND link= 1;')
        data = c.fetchall()
        # print(data)
    except sqlite3.Error as error:
        print("Error5: {}".format(error))
    for row in data:
        print(row[0])
        # update status change in stat_timeline and devices
        # if any sensor is high then the device status is set to high
        if (dstatus=='1'):
            # update status change in stat_timeline and devices
            set_status(device_id=row[0], status='1',send=True)

        elif (dstatus=='0'):
            try:
                c.execute('SELECT d.* FROM devices d INNER JOIN links l ON d.id = l.id WHERE l.link_id=+'+row[0]+' AND  d.status=1;')
                data = c.fetchall()

                if len(data) == 0:
                    # update status change in stat_timeline and devices
                    set_status(device_id=row[0], status='0',send=True)
                else:
                    # update status change in stat_timeline and devices
                    set_status(device_id=row[0], status='1',send=True)
            except sqlite3.Error as error:
                print("Error6: {}".format(error))

def set_status(device_id,status,send):
    try:
        c.execute('INSERT INTO stat_timeline (id,status,time) VALUES (?,?,?);', (device_id, status, datetime.now()))
    except sqlite3.Error as error:
        print("Error7: {}".format(error))
    try:
        c.execute('UPDATE devices SET status = ' + status + ' WHERE id =' + f"{device_id};")
    except sqlite3.Error as error:
        print("Error8: {}".format(error))
    if send:
        # send linked device status via MQTT [Format : @(relay number)(status)%)]
        temp = device_id.split('.')
        ori_id = temp[0]
        relay_num = temp[1]
        send_message(ori_id, '@' + relay_num + status + '%')

conn.commit()


# read database
def read_db():
    c.execute('SELECT * FROM pirs')
    for row in c.fetchall():
        print(row)
    c.execute('SELECT * FROM acs')
    for row in c.fetchall():
        print(row)
    c.execute('SELECT * FROM relays')
    for row in c.fetchall():
        print(row)

def read_devices():
    c.execute('SELECT * FROM devices')
    for row in c.fetchall():
        #print(row)
        #devices_dict[row[0]]=row[1]
        key=row[0]
        devices_dict[key] = []
        devices_dict[key].append(row[1])
        devices_dict[key].append(row[2])
        devices_dict[key].append(row[3])
        print(str(devices_dict[key][1])+" "+str(devices_dict[key][0])+" "+str(devices_dict[key][2]))

def read_links():
    c.execute('SELECT * FROM links WHERE link=1')
    for row in c.fetchall():
        #print(row)
        #devices_dict[row[0]]=row[1]
        key=row[0]
        if key not in links_dict:
            links_dict[key] = []
            links_dict[key].append(row[1])
        else:
            links_dict[key].append(row[1])
    for values in links_dict[key]:
            print(values)

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

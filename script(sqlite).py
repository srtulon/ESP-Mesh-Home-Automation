#!/usr/bin/python3
import time
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
conn = sqlite3.connect('database.db',check_same_thread=False)
c = conn.cursor()



# create table
def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS devices ( id varchar(20) not null, type varchar(20) not null,name varchar(20) not null, status int, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS stat_timeline (id varchar(20) not null, status int, TimeStamp TIMESTAMP, FOREIGN KEY (id) REFERENCES devices(id))')
    c.execute('CREATE TABLE IF NOT EXISTS links (ser int AUTO_INCREMENT,id varchar(20) not null, link_id varchar(20) , link int, FOREIGN KEY (id) REFERENCES devices(id),PRIMARY KEY (ser))')
    #c.execute('CREATE TABLE IF NOT EXISTS ircode (id varchar(20) not null, link_id varchar(20) , link int, FOREIGN KEY (id) REFERENCES devices(id))')


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
                c.execute("INSERT OR IGNORE INTO devices (id,type,name,status) VALUES (?,?,?,?);",(new_id, dtype, new_id, dstatus))
            except sqlite3.Error as error:
                print("Error1: {}".format(error))
    else:
        try:
            c.execute("INSERT OR IGNORE INTO devices (id,type,name,status) VALUES (?,?,?,?);", (did, dtype, did, dstatus))
        except sqlite3.Error as error:
            print("Error2: {}".format(error))

    conn.commit()
    print("Data entry completed")
    ack(did)


    # insert new data in stat_timeline if device status is changed
    try:
        c.execute('SELECT status FROM devices WHERE id=' + did)
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
                link()
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
        c.execute('INSERT INTO stat_timeline (id,status) VALUES (?,?);', (device_id, status))
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
    c.execute('SELECT * FROM devices')
    for row in c.fetchall():
        print(row)



# initialization
def initialization():
    print("Start initialization")
    c.execute("INSERT INTO stat_timeline (id,status) SELECT id, status FROM devices;")


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
            print(dstatus)

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

client = mqtt.Client(client_id="script",clean_session=False)
client.on_connect = on_connect
client.on_message = on_message
client.connect('192.168.0.102', 1883, 60)  # change the address to MQTT broker server
client.loop_forever()

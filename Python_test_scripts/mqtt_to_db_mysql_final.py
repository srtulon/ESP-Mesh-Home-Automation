#!/usr/bin/python3
import time
import paho.mqtt.client as mqtt
import re

import mysql.connector as mariadb

time.sleep(20)

# Declaring global variable
did = 0  # devie id
dtype = 0  # device type
dstatus = 0  # devie status

########################## DATABASE PART ###########################################

# Database connection
conn = mariadb.connect(host='192.168.1.55', database='test', password='abc123', user='root')
c = conn.cursor()

# For creating create db
# Below line  is hide your warning
c.execute("SET sql_notes = 0; ")
# create db here....
c.execute("create database IF NOT EXISTS test")


# create table
def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS devices ( id varchar(20) not null, type varchar(20) not null,name varchar(20) not null, status int, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS stat_timeline (id varchar(20) not null, status int, TimeStamp TIMESTAMP, FOREIGN KEY (id) REFERENCES devices(id))')
    c.execute('CREATE TABLE IF NOT EXISTS links (ser int AUTO_INCREMENT,id varchar(20) not null, link_id varchar(20) , link int, FOREIGN KEY (id) REFERENCES devices(id),PRIMARY KEY (ser))')


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
                c.execute("INSERT IGNORE devices (id,type,name,status) VALUES (%s,%s,%s,%s);",(new_id, dtype, new_id, dstatus))
            except mariadb.Error as error:
                print("Error: {}".format(error))
    else:
        try:
            c.execute("INSERT IGNORE devices (id,type,name,status) VALUES (%s,%s,%s,%s);", (did, dtype, did, dstatus))
        except mariadb.Error as error:
            print("Error: {}".format(error))

    conn.commit()
    print("Data entry completed")


    # insert new data in stat_timeline if device status is changed
    try:
        c.execute('SELECT status FROM devices WHERE id=' + did)
    except mariadb.Error as error:
        print("Error: {}".format(error))

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
            except mariadb.Error as error:
                print("Error: {}".format(error))


# link
def link():
    # select linked devices
    try:
        c.execute('SELECT link_id FROM links WHERE id=' + did + ' AND link= 1;')
        data = c.fetchall()
        # print(data)
    except mariadb.Error as error:
        print("Error: {}".format(error))
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
            except mariadb.Error as error:
                print("Error: {}".format(error))

def set_status(device_id,status,send):
    try:
        c.execute('INSERT INTO stat_timeline (id,status) VALUES (%s,%s);', (device_id, status))
    except mariadb.Error as error:
        print("Error: {}".format(error))
    try:
        c.execute('UPDATE devices SET status = ' + status + ' WHERE id =' + f"{device_id}")
    except mariadb.Error as error:
        print("Error: {}".format(error))
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
    # print(txt)
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


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.1.55", 1883, 60)  # change the address to MQTT broker server
client.loop_forever()

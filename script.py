import paho.mqtt.client as mqtt
import re


import mysql.connector as mariadb

#Declaring global variable
did = 0        #devie id
dtype = 0      #device type
dstatus = 0    #devie status


########################## DATABASE PART ###########################################

# Database connection
conn = mariadb.connect(host='localhost', database='test', password='abc123', user='root')
c = conn.cursor()

# For creating create db
# Below line  is hide your warning
c.execute("SET sql_notes = 0; ")
# create db here....
c.execute("create database IF NOT EXISTS test")

# create table
def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS devices ( id bigint not null, type varchar(20) not null,name varchar(20) not null, status int, unique (id))')
    c.execute('CREATE TABLE IF NOT EXISTS stat_timeline (id bigint not null, status int, TimeStamp TIMESTAMP)')
    c.execute('CREATE TABLE IF NOT EXISTS links (id bigint not null, link_id bigint , link int)')


#data entry
def data_entry():
    print("Starting data entry")

    #insert new data in devices
    try:
        c.execute("INSERT IGNORE devices (id,type,name,status) VALUES (%s,%s,%s,%s);", (did, dtype, did, dstatus))
        conn.commit()
        print("Data entry completed")

    except mariadb.Error as error:
        print("Error: {}".format(error))

    # insert new data in stat_timeline if device status is changed
    try:
        c.execute('SELECT status FROM stat_timeline WHERE id=' + did + ' ORDER BY `timestamp`  DESC limit 1')
    except mariadb.Error as error:
        print("Error: {}".format(error))

    for row in c.fetchall():
        # print(row)
        data1 = f"{row[0]}"
        data2 = f"{dstatus}"
        # print(data1)
        # print(data2)

        if data1 == data2 and row is not None:
            print("Matched")
        else:
            print("Not matched")
            try:
                c.execute('INSERT INTO stat_timeline (id,status) VALUES (%s,%s);', (did, dstatus))
                c.execute('UPDATE devices SET status = ' + dstatus + ' WHERE id =' + did)
                link()
                send_update()
            except mariadb.Error as error:
                print("Error: {}".format(error))


# link
def link():
    # select linked devices
    try:
        c.execute('SELECT link_id FROM links WHERE id=' + did + ' AND link= 1;')
        # print("check")
        data = c.fetchall()
        # print(data)
    except mariadb.Error as error:
        print("Error: {}".format(error))
    for row in data:
        #update status change in stat_timeline and devices
        try:
            print(row[0])
            # update status change in stat_timeline and devices
            c.execute('INSERT INTO stat_timeline (id,status) VALUES (%s,%s);', (row[0], dstatus))
            c.execute('UPDATE devices SET status = ' + dstatus + ' WHERE id =' + f"{row[0]}")
            #send linked device status via MQTT [Format : @(status)%)]
            send_message(f"{row[0]}", '@' + dstatus + '%')
        except mariadb.Error as error:
            print("Error: {}".format(error))




conn.commit()

# read database
def read_db():
    c.execute('SELECT * FROM devices')
    # data=c.fetchall()
    # print(data)
    for row in c.fetchall():
        print(row)
        # print(row[1])

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
    print("##########################################")
    # print(len(txt))
    # print(txt)
    # print(txt.find('$'))
    if len(txt) > 10:
        # message format: #(id),(type),(status)$
        if txt[0] == '#' and txt.find('$') > 0:
            txt = txt.replace("#", "")
            # txt=txt.replace("$","")
            # print(msg.topic+" "+txt)
            t = txt.split("$")
            # print(t[0])
            s = t[0].split(",")
            did = s[0]
            dtype = s[1]
            dstatus = s[2]
            # print(len(did))
            # print(did)
            # print(dtype)
            print(dstatus)
            # data_entry(did,dtype,did,dstatus)

            # for ignoring garbage data
            if (len(did) == 9 or len(did) == 10):
                data_entry()

# send message
def send_message(dev, msg):
    # trimming
    msg.strip()
    msg.rstrip()
    msg.lstrip()

    #sending to "device/to/(target device)
    dev = "device/to/" + dev
    # print(dev)
    client.publish(dev, msg)
    # print("send check")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("MQTT.com", 1883, 60)   #change the address to MQTT broker server
client.loop_forever()

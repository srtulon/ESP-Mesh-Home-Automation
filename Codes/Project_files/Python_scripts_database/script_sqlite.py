#!/usr/bin/python3
import time
from datetime import datetime
import paho.mqtt.client as mqtt
import re
import os

import sqlite3

#time.sleep(20)



# Declaring global variable
did = 0  # devie id
dtype = 0  # device type
dmsg = 0  # devie message


relays_dict=dict()
pirs_dict=dict()
acs_dict=dict()
relays_links_dict=dict()
acs_links_dict=dict()
ac_list_dict=dict()

########################## DATABASE PART ###########################################

# sqlite3 Database connection
database=os.path.dirname(__file__)+'/database.db'
conn = sqlite3.connect(database,check_same_thread=False)
c = conn.cursor()



# create table
def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS relays ( id varchar(20) not null,name varchar(20) not null, status int,last_update text, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS acs ( id varchar(20) not null, name varchar(20) not null, protocol int, model int,power int, temp int,last_update text, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS pirs ( id varchar(20) not null,name varchar(20) not null, status int, last_update text,PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS stat_timeline (id varchar(20) not null, status int, time text)')
    c.execute('CREATE TABLE IF NOT EXISTS relays_links (id varchar(20) not null, link_id varchar(20) , link int,priority int)')
    c.execute('CREATE TABLE IF NOT EXISTS acs_links (id varchar(20) not null, link_id varchar(20) , link int, protocol int, model int,temp int)')
    c.execute('CREATE TABLE IF NOT EXISTS ac_list (protocol varchar(20),PRIMARY KEY (protocol))')
    c.execute('CREATE TABLE IF NOT EXISTS remote_list (protocol varchar(20),PRIMARY KEY (protocol))')



def ac_name_database():
    f = open("ac_protocol_names.txt", "r")
    contents = f.readlines()
    count=0
    for x in contents:
         x.rstrip()
         count=count+1
         x.strip()
         #print(x)
         try:
             c.execute("INSERT OR IGNORE INTO ac_list (protocol) VALUES (?);",(x,))
         except sqlite3.Error as error:
            print("Error: {}".format(error))
            return
    conn.commit()



def read_database():
    #read database tables and make a dictionary copy

    c.execute('SELECT * FROM relays')
    for row in c.fetchall():
        key=row[0] #read id
        relays_dict[key] = row[2] #read status

    print(relays_dict)

    c.execute('SELECT * FROM acs')
    for row in c.fetchall():
        key=row[0] #read id
        acs_dict[key] = []
        acs_dict[key].append(row[2]) #read protocol
        acs_dict[key].append(row[3]) #read model
        acs_dict[key].append(row[4]) #read power state
        acs_dict[key].append(row[5]) #read temperature
    print(acs_dict)

    c.execute('SELECT * FROM pirs')
    for row in c.fetchall():
        key=row[0] #read id
        pirs_dict[key]=row[2] #read status
    print(pirs_dict)

    c.execute('SELECT * FROM relays_links WHERE link=1')
    for row in c.fetchall():
        key=row[0] #read id
        if key not in relays_links_dict:
            relays_links_dict[key] = []
            relays_links_dict[key].append([row[1],row[3]]) #read link_id and priority #########################################
            #relays_links_dict[key].append(row[3]) #read link_id and priority #########################################
        else:
            relays_links_dict[key].append([row[1],row[3]]) #read link_id and priority ##########################################
            #relays_links_dict[key].append(row[3]) #read link_id and priority #########################################
    print(relays_links_dict)

    c.execute('SELECT * FROM acs_links WHERE link=1')
    for row in c.fetchall():
        key=row[0] #read id
        if key not in acs_links_dict:
            acs_links_dict[key] = []
            acs_links_dict[key].append(row[1],row[3],row[4],row[5]) #read link_id and commands #########################################
        else:
            acs_links_dict[key].append(row[1],row[3],row[4],row[5]) #read link_id and commands ##########################################
    print(acs_links_dict)


# initialization
def initialization():
    print("Start initialization")
    print("Current directory: "+os.path.dirname(__file__))
    #ac_name_database()
    read_database()

# data entry
def data_entry():
    # insert new data in devices
    if dtype[0] == 'r':
        range1 = int(dtype[1]) + 1
        for i in range(range1):
            new_id = did + '.' + str(i)
            print(new_id)

            key=new_id
            if key not in relays_dict:
                print("Adding new relay to list")
                relays_dict[key] = dmsg

                try:
                    c.execute("INSERT OR IGNORE INTO relays (id,name,status) VALUES (?,?,?);",(new_id, new_id, dmsg))
                except sqlite3.Error as error:
                    print("Error1: {}".format(error))
                    return

            else:
                data1 = str(relays_dict[key])
                data2 = str(dmsg)
                # print(data1)
                # print(data2)

                # matching current status with previous to avoid multiple entry
                if data1 == data2:
                    print("Relay Value Matched. Data1:"+str(data1)+" Data2:"+str(data2))
                else:
                    print("Relay Value Not matched. Data1:"+str(data1)+" Data2:"+str(data2))

                    # for incoming status update, no need to send status
                    set_status(device_id=did, status=dmsg, type=dtype[0],send=False)


    elif dtype[0] == 'a':
        key=did
        prot=dmsg[0:2]
        mod=dmsg[2]
        pow=dmsg[3]
        tem=dmsg[4:6]
        if key not in acs_dict:
            print("Adding new ac to list")


            acs_dict[key][0] = prot
            acs_dict[key][1] = mod
            acs_dict[key][2] = pow
            acs_dict[key][4] = tem



            try:
                c.execute("INSERT OR IGNORE INTO acs (id,name,protocol,model,power,temp) VALUES (?,?,?,?,?,?);",(did, did, prot, mod, pow, tem))
            except sqlite3.Error as error:
                print("Error1: {}".format(error))
                return

        else:
            print("AC Value Not matched")

            # for incoming status update, no need to send status
            set_status(device_id=did, status=dmsg, type=dtype[0],send=False)


    elif dtype[0] == 'p':
        key=did
        if key not in pirs_dict:
            print("Adding new pir to list")
            pirs_dict[key] = dmsg


            try:
                c.execute("INSERT OR IGNORE INTO pirs (id,name,status) VALUES (?,?,?);", (did, did, dmsg))
            except sqlite3.Error as error:
                print("Error2: {}".format(error))
                return
        else:
            data1 = pirs_dict[key]
            data2 = dmsg
            print(int(data1))
            print(int(data2))

    conn.commit()
    print("Data entry completed")
    ack(did)


# link
def link():

    if did in relays_links_dict:
        # select linked devices
        for l in relays_links_dict[did]:
            print(str(l))
            # update status change in stat_timeline and devices
            # if any sensor is high then the device status is set to high
            if (dmsg=='1'):
                # update status change in stat_timel,ine and devices
                set_status(device_id=l[0], status='1',type='r',send=True) #############################

            elif (dmsg=='0'):
                if check(did):
                    # update status change in stat_timeline and devices
                    set_status(device_id=l[0], status='1',type='r',send=True) ############################
                else:
                    # update status change in stat_timeline and devices
                    set_status(device_id=l[0], status='0',type='r',send=True) #############################

    if did in acs_links_dict:
        for l in acs_links_dict[did]:
            print(l)
            # update status change in stat_timeline and devices
            # if any sensor is high then the device status is set to high
            if (dmsg=='1'):
                # update status change in stat_timeline and devices
                set_status(device_id=l[0], status=l[1]+l[2]+'1'+l[3],type='a',send=True) #############################

            elif (dmsg=='0'):
                if check(l):
                    # update status change in stat_timeline and devices
                    set_status(device_id=l[0], status=l[1]+l[2]+'1'+l[3],type='a',send=True) ############################
                else:
                    # update status change in stat_timeline and devices
                    set_status(device_id=l[0], status=l[1]+l[2]+'0'+l[3],type='a',send=True) #############################



def check(id):
    for i in [k for k,v in relays_links_dict.items() if id in v]:
        #print(i)
        #print(devices_dict[i])
        if pirs_dict[i]==1:
            return True
    return False

def set_status(device_id,status,type,send):
    #try:
        #c.execute('INSERT INTO stat_timeline (id,status,time) VALUES (?,?,?);', (device_id, status, datetime.now()))
    #except sqlite3.Error as error:
        #print("Error: {}".format(error))

    if (type == 'r'):
        relays_dict[device_id]=status
        try:
            c.execute('UPDATE relays SET status = ' + status + ' WHERE id =' + f"{device_id};")
        except sqlite3.Error as error:
            print("Error: {}".format(error))
        if send:
            # send linked relays status via MQTT [Format : @(relay number)(status)%)]
            temp = str(device_id).split('.')
            print(device_id)
            print(temp)
            ori_id = temp[0]
            relay_num = temp[1]
            send_message(ori_id, '@' + relay_num + status + '%')

    elif (type == 'a'):
        acs_dict[device_id]=status
        try:
            c.execute('UPDATE acs SET status = ' + status + ' WHERE id =' + f"{device_id};")
        except sqlite3.Error as error:
            print("Error: {}".format(error))

        if send:
            # send linked ac status via MQTT [Format : @(status)%)]
            send_message(device_id, '@' + status + '%')

    elif (type == 'p'):
        pirs_dict[device_id]=status
        try:
            c.execute('UPDATE pirs SET status = ' + status + ' WHERE id =' + f"{device_id};")
        except sqlite3.Error as error:
            print("Error: {}".format(error))


create_table()
initialization()


########################## MQTT PART ###########################################

# mqtt connection
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("device/from/#")



# new message
def on_message(client, userdata, msg):
    global did, dtype, dmsg
    txt = (msg.payload).decode("utf-8")
    print(txt)
    # print(len(txt))
    # print(txt.find('$'))
    if len(txt) > 10:
        print("##########################################")
        localtime = time.asctime( time.localtime(time.time()) )
        print(localtime)
        # message format: #(id),(type),(status)$
        if txt[0] == '#' and txt.find('$') > 0:
            txt = txt.replace("#", "")
            t = txt.split("$") # to discard garbage values after '$'
            # print(t[0])
            s = t[0].split(",")
            did = s[0]
            dtype = s[1]
            dmsg = s[2]

            # print(len(did))
            # print(did)
            # print(dtype)
            print("Device Status: "+dmsg)

            data_entry()


        else:
            p=txt.split("\n")
            p=p[0]
            if p.find('Protocol')>=0:
                protocol=p.split(":")[1]
                print(protocol.strip())
                try:
                    c.execute("INSERT OR IGNORE INTO remote_list (protocol) VALUES (?);",(protocol.strip(),))
                except sqlite3.Error as error:
                   print("Error: {}".format(error))
                   return
            conn.commit()





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



#client = mqtt.Client(client_id="script",clean_session=False)
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect("192.168.0.102", 1883, 60)  # change the address to MQTT broker server
client.loop_forever()

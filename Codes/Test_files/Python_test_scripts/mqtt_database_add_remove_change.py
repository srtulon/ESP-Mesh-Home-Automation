import paho.mqtt.client as mqtt
import sqlite3

# sqlite3 Database connection
conn = sqlite3.connect('database.db',check_same_thread=False)
c = conn.cursor()

relays_dict=dict()
acs_dict=dict()
pirs_dict=dict()
acs_dict=dict()
relays_links_dict=dict()
acs_links_dict=dict()
ac_list_dict=dict()

# create table
def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS relays ( id varchar(20) not null,name varchar(20) not null, status int,last_update text, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS acs ( id varchar(20) not null, name varchar(20) not null, protocol int, model int,power int, temp int,last_update text, PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS pirs ( id varchar(20) not null,name varchar(20) not null, status int, last_update text,PRIMARY KEY (id))')
    c.execute('CREATE TABLE IF NOT EXISTS stat_timeline (id varchar(20) not null, status int, time text)')
    c.execute('CREATE TABLE IF NOT EXISTS relays_links (id varchar(20) not null, link_id varchar(20) , link int,priority int)')
    c.execute('CREATE TABLE IF NOT EXISTS acs_links (id varchar(20) not null, link_id varchar(20) , link int, command int)') 
    c.execute('CREATE TABLE IF NOT EXISTS ac_list (protocol varchar(20),PRIMARY KEY (protocol))')
    c.execute('CREATE TABLE IF NOT EXISTS remote_list (protocol varchar(20),PRIMARY KEY (protocol))')


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("device/from/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    txt=(msg.payload).decode("utf-8")

    if txt[0] == '^' and txt.find('!') > 0:
            txt = txt.replace("^", "")
            t = txt.split("!")
            s = t[0].split(",")
            dtype=s[0]
            did = s[1]
            lid = s[2]
            link = s[3]
            msg1=s[4]
            print(dtype)
            print(did)
            print(lid)
            print(link)
            print(msg1)


            if dtype[0] == 'rl':
                key=did
                if key not in relays_links_dict:
                    print("Adding new relay_links to list")
                    relays_links_dict[key] = []
                    relays_links_dict[key].append([row[1],row[3]])

                    try:
                        c.execute("INSERT OR IGNORE INTO relays_links (id,link_id,link,priority) VALUES (?,?,?,?);", (did, did, dmsg))
                    except sqlite3.Error as error:
                        print("Error2: {}".format(error))
                        return
                else:
                    data1 = pirs_dict[key]
                    data2 = dmsg
                    print(int(data1))
                    print(int(data2))
    
                elif dtype[0] == 'al':
                    pass
            






            

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.0.102", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
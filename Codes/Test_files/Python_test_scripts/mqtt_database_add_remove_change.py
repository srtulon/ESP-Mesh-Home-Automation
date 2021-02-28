import paho.mqtt.client as mqtt
import sqlite3
import os

# sqlite3 Database connection
database=os.path.dirname(__file__)+'/database.db'
conn = sqlite3.connect(database,check_same_thread=False)
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
            #Message format ^dtype,com,did,lid,link,msg1!
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


            if dtype == 'rla':
                key=did
                if key not in relays_links_dict:
                    print("Adding new relay_links to list")
                    relays_links_dict[key] = []

                if any(lid == x[0] for x in relays_links_dict[key]):
                    print("Link already present")
                else:
                    relays_links_dict[key].append([lid,msg1])

                try:
                    c.execute("INSERT INTO relays_links (id,link_id,link,priority) VALUES (?,?,?,?);", (did, lid, 1,msg1))
                except sqlite3.Error as error:
                    print("Error: {}".format(error))
                    return
                
                

            elif dtype == 'rlu':
                print("Updating relay_links")
                key=did  
                for l in relays_links_dict[key]:
                    if lid in l:
                        l[1]=msg1
                        break
                try:
                    c.execute("UPDATE relays_links SET priority = ? WHERE link_id=? AND id=?;", (msg1,lid,did))
                except sqlite3.Error as error:
                    print("Error: {}".format(error))
                    return

            elif dtype == 'ala':
                pass
            
            elif dtype == 'rlr':
                key=did
                if key not in relays_links_dict:
                    print('Id not found')
                else:
                    print("Deleting relay_links from list")
                    check=False
                    for l in relays_links_dict[key]:
                        if lid in l:                            
                            relays_links_dict[key].remove(l)
                            print("Deleted")
                            check=True
                            break

                    if not check:
                        print('Link not found')
                        
                    try:
                        c.execute("DELETE FROM relays_links WHERE link_id=? AND id=?;", (lid,did))
                    except sqlite3.Error as error:
                        print("Error: {}".format(error))
                        return

            
            
            elif dtype == 'alr':
                    pass
            
            conn.commit()

            print(relays_links_dict)
            #print(acs_links_dict)







            

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("192.168.0.102", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
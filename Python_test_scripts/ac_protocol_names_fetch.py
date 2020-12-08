import re
import os
import sqlite3

conn = sqlite3.connect('database.db',check_same_thread=False)
c = conn.cursor()

c.execute('CREATE TABLE IF NOT EXISTS ac_list (protocol varchar(20),PRIMARY KEY (protocol))')

f = open("ac_protocol_names.txt", "r")
contents = f.readlines()


def arduino():
    count=0
    for x in contents:
         x.rstrip()
         count=count+1
         #print(str(count)+'.'+x.strip())
         print("    else if(msg1.substring(2,3)==\""+str(count).zfill(2)+"\"){")
         print("        Serial.print(\"Protocol: "+x.strip()+"\");")
         print("        ac.next.protocol = decode_type_t::"+x.strip()+";")
         print("     }")

def database():
    count=0
    for x in contents:
         x.rstrip()
         count=count+1
         x.strip()
         print(x)
         try:
             c.execute("INSERT OR IGNORE INTO ac_list (protocol) VALUES (?);",(x,))
         except sqlite3.Error as error:
            print("Error: {}".format(error))
            return
    conn.commit()

database()

import sqlite3


########################## DATABASE PART ###########################################

# Database connection
conn = sqlite3.connect('database.db',check_same_thread=False)
c = conn.cursor()


devices_dict=dict()
links_dict=dict()
# read database
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

read_devices()
read_links()
print(devices_dict)
print(links_dict)
print(links_dict[977122286][0])

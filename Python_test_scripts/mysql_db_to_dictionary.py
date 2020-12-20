import mysql.connector as mariadb


########################## DATABASE PART ###########################################

# Database connection
conn = mariadb.connect(host='localhost', database='test', password='abc123', user='root')
c = conn.cursor()

# For creating create db
# Below line  is hide your warning
c.execute("SET sql_notes = 0; ")
# create db here....
c.execute("create database IF NOT EXISTS test")

devices_dict=dict()
links_dict=dict()
# read database
def read_devices():
    c.execute('SELECT * FROM devices')
    for row in c.fetchall():
        #print(row)
        #devices_dict[row[0]]=row[1]
        key=row[0]


        devices_dict[key]=(row[3])
        #print(str(devices_dict[key][1])+" "+str(devices_dict[key][0])+" "+str(devices_dict[key][2]))

def read_links():
    c.execute('SELECT * FROM links')
    for row in c.fetchall():
        #print(row)
        #devices_dict[row[0]]=row[1]
        key=row[0]
        if key not in links_dict:
            links_dict[key] = []
        else:
            links_dict[key].append(row[1])
        for values in links_dict[key]:
            print(values)

read_devices()
read_links()
print(devices_dict)
print(links_dict)
print(links_dict['3257182011'])
for d in links_dict['3257182011']:
    print(d)
#print([k for k,v in links_dict.items() if "3257604729.1" in v])

def check():
    for i in [k for k,v in links_dict.items() if "3257604729.1" in v]:
        print(i)
        print(devices_dict[i])
        if devices_dict[i]==1:
            return True
    return False


#print(check())

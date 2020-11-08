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



# read database
def read_db():
    c.execute('SELECT * FROM devices')
    for row in c.fetchall():
        print(row)


read_db()

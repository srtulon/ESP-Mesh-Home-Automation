import sqlite3

conn= sqlite3.connect('test.db')
c= conn.cursor()

def create_table():
    c.execute('CREATE TABLE IF NOT EXISTS links (id int, link int,status int)')

def data_entry():
    c.execute('INSERT INTO links VALUES (986094,89593859,0)')
    conn.commit()
    c.close()
    conn.close()

def read_db():
    c.execute('SELECT link,status FROM links WHERE id=123123')
    #data=c.fetchall()
    #print(data)
    for row in c.fetchall():
        print(row[0])
        print(row[1])
        
create_table()
#data_entry()
read_db()


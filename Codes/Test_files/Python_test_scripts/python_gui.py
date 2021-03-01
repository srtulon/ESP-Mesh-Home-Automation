import sqlite3
my_conn = sqlite3.connect('database.db')
import paho.mqtt.publish as publish
###### end of connection ####



query="SELECT id FROM pirs"
r_set=my_conn.execute(query);
id = [r for r, in r_set] # create a  list 

query="SELECT id FROM relays"
r_set=my_conn.execute(query);
link = [r for r, in r_set] # create a  list

zero_one=[0,1]
add_delete=['Add','Del']

def function():
    s=''
    if(options6.get()=='Add'):
        s='rla'
    elif(options6.get()=='Del'):
        s='rlr'

    str='^'+s+','+options1.get()+','+options2.get()+','+options3.get()+','+options4.get()+'!'

    print(str)
    publish.single( "device/from", str, hostname="192.168.0.102")


import tkinter as tk
my_w = tk.Tk()
my_w.geometry("600x500")  # Size of the window 
my_w.title("App")  # Adding a title

options1 = tk.StringVar(my_w)
options1.set(id[0]) # default value

options2 = tk.StringVar(my_w)
options2.set(link[0]) # default value

options3 = tk.StringVar(my_w)
options3.set(zero_one[1]) # default value

options4 = tk.StringVar(my_w)
options4.set(zero_one[1]) # default value

options6 = tk.StringVar(my_w)
options6.set(add_delete[0]) # default value


om6 =tk.OptionMenu(my_w, options6, *add_delete)
om6.grid(row=2,column=1)

om1 =tk.OptionMenu(my_w, options1, *id)
om1.grid(row=2,column=2)

om2 =tk.OptionMenu(my_w, options2, *link)
om2.grid(row=2,column=3)

om3 =tk.OptionMenu(my_w, options3, *zero_one)
om3.grid(row=2,column=4)

om4 =tk.OptionMenu(my_w, options4, *zero_one)
om4.grid(row=2,column=5)




button = tk.Button(my_w, text='Send', width=15, command=function)
button.grid(row=2,column=7)


my_w.mainloop()
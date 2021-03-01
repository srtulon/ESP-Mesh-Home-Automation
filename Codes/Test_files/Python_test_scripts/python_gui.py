import sqlite3
import os

database=os.path.dirname(os.path.realpath(__file__))+'/database.db'
my_conn = sqlite3.connect(database,check_same_thread=False)
import paho.mqtt.publish as publish
###### end of connection ####
root = os.path.dirname(os.path.realpath(__file__))
print(root)


#query="SELECT id FROM pirs"
#r_set=my_conn.execute(query);
#id = [r for r, in r_set] # create a  list 

id=[1,1]

#query="SELECT id FROM relays"
#r_set=my_conn.execute(query);
#link = [r for r, in r_set] # create a  list

link=[2,2]

zero_one=[0,1]
add_delete=['Add','Del']

def function1():
    s=''
    if(options6.get()=='Add'):
        s='rla'
    elif(options6.get()=='Del'):
        s='rlr'

    str='^'+s+','+options1.get()+','+options2.get()+','+options3.get()+','+options4.get()+'!'

    print(str)
    publish.single( "device/from", str, hostname="192.168.0.102")


def function2():
    
    str='^'+options7.get()+','+options8.get()+'%'

    print(str)
    publish.single( "device/to"+str, str, hostname="192.168.0.102")


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
tk.Label(my_w, text="Option").grid(row = 1, column = 1)
om6.grid(row=2,column=1)

om1 =tk.OptionMenu(my_w, options1, *id)
tk.Label(my_w, text="Id").grid(row = 1, column = 2)
om1.grid(row=2,column=2)


om2 =tk.OptionMenu(my_w, options2, *link)
tk.Label(my_w, text="Link_Id").grid(row = 1, column = 3)
om2.grid(row=2,column=3)

om3 =tk.OptionMenu(my_w, options3, *zero_one)
tk.Label(my_w, text="Link").grid(row = 1, column = 4)
om3.grid(row=2,column=4)

om4 =tk.OptionMenu(my_w, options4, *zero_one)
tk.Label(my_w, text="Priority").grid(row = 1, column = 5)
om4.grid(row=2,column=5)

button1 = tk.Button(my_w, text='Send', width=15, command=function1)
button1.grid(row=2,column=7)



######## device control #########
options7 = tk.StringVar(my_w)
options7.set(link[0]) # default value

options8 = tk.StringVar(my_w)
options8.set(zero_one[1]) # default value

om7 =tk.OptionMenu(my_w, options6, *link)
tk.Label(my_w, text="Id").grid(row = 4, column = 1)
om7.grid(row=5,column=1)

om8 =tk.OptionMenu(my_w, options6, *zero_one)
tk.Label(my_w, text="Com").grid(row = 4, column = 2)
om8.grid(row=5,column=2)

button2 = tk.Button(my_w, text='Send', width=15, command=function2)
button2.grid(row=5,column=3)

my_w.mainloop()
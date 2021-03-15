import sqlite3
import os
from functools import partial



database=os.path.dirname(os.path.realpath(__file__))+'/database.db'
my_conn = sqlite3.connect(database,check_same_thread=False)
import paho.mqtt.publish as publish
###### end of connection ####
root = os.path.dirname(os.path.realpath(__file__))
print(root)


host="192.168.1.28"

query="SELECT id FROM pirs"
r_set=my_conn.execute(query);
id = [r for r, in r_set] # create a  list 



query="SELECT id FROM relays"
r_set=my_conn.execute(query);
link1 = [r for r, in r_set] # create a  list

link2 = link1 # create a  list

query="SELECT protocol FROM ac_list"
r_set=my_conn.execute(query);
link3 = [r for r, in r_set] # create a  list

query="SELECT id FROM acs"
r_set=my_conn.execute(query);
link4 = [r for r, in r_set] # create a  list

templist=[16,17,18,19,20,21,22,23,24,25,26,27,28,29,30]

zero_one1=[0,1]
zero_one2=[0,1]
zero_one3=[0,1]
zero_one4=[0,1]
add_delete=['Add','Del']

def function1():
    s=''
    if(options6.get()=='Add'):
        s='rla'
    elif(options6.get()=='Del'):
        s='rlr'

    s='^'+s+','+options1.get()+','+options2.get()+','+options3.get()+','+options4.get()+'!'

    print(str)
    publish.single( "device/from", str, hostname=host)


def function2():

    temp = options7.get().split('.')
    ori_id = temp[0]
    print(ori_id)
    relay_num = temp[1]
    status=options8.get()
    str='@' + relay_num + status + '%'
    print(str)
    publish.single( "device/to/"+ori_id, str, hostname=host)

def function3():

    id = options12.get()
    print(id)
    prot=link3.index(options9.get())
    if prot<10:
        prot1='0'+str(prot)
    else:
        prot1=str(prot)
    pow=options10.get()
    tem=options11.get()
    s='@'+prot1+'1'+pow+tem+'%'
    print(s)
    publish.single( "device/to/"+id, s, hostname="192.168.1.18")


import tkinter as tk
my_w = tk.Tk()
my_w.geometry("700x600")  # Size of the window 
my_w.title("App")  # Adding a title

options1 = tk.StringVar(my_w)
options1.set(id[0]) # default value

options2 = tk.StringVar(my_w)
options2.set(link1[0]) # default value

options3 = tk.StringVar(my_w)
options3.set(zero_one1[1]) # default value

options4 = tk.StringVar(my_w)
options4.set(zero_one2[1]) # default value

options6 = tk.StringVar(my_w)
options6.set(add_delete[0]) # default value


om6 =tk.OptionMenu(my_w, options6, *add_delete)
tk.Label(my_w, text="Option").grid(row = 1, column = 1)
om6.grid(row=2,column=1)

om1 =tk.OptionMenu(my_w, options1, *id)
tk.Label(my_w, text="Pir").grid(row = 1, column = 2)
om1.grid(row=2,column=2)


om2 =tk.OptionMenu(my_w, options2, *link1)
tk.Label(my_w, text="Relay").grid(row = 1, column = 3)
om2.grid(row=2,column=3)

om3 =tk.OptionMenu(my_w, options3, *zero_one1)
tk.Label(my_w, text="Link").grid(row = 1, column = 4)
om3.grid(row=2,column=4)

om4 =tk.OptionMenu(my_w, options4, *zero_one2)
tk.Label(my_w, text="Priority").grid(row = 1, column = 5)
om4.grid(row=2,column=5)

button1 = tk.Button(my_w, text='Send', width=15, command=function1)
button1.grid(row=2,column=7)



######## device control #########
options7 = tk.StringVar(my_w)
options7.set(link2[0]) # default value

options8 = tk.StringVar(my_w)
options8.set(zero_one3[1]) # default value

om7 =tk.OptionMenu(my_w, options7, *link2)
tk.Label(my_w, text="Name").grid(row = 4, column = 1)
om7.grid(row=5,column=1)

om8 =tk.OptionMenu(my_w, options8, *zero_one3)
tk.Label(my_w, text="Command").grid(row = 4, column = 2)
om8.grid(row=5,column=2)

button2 = tk.Button(my_w, text='Send', width=15, command=function2)
button2.grid(row=5,column=3)


######## ac control #########
options12 = tk.StringVar(my_w)
options12.set(link4[0]) # default value

om12 =tk.OptionMenu(my_w, options12, *link4)
tk.Label(my_w, text="Name").grid(row = 6, column = 1)
om12.grid(row=7,column=1)

options9 = tk.StringVar(my_w)
options9.set(link3[0]) # default value

options10 = tk.StringVar(my_w)
options10.set(zero_one3[1]) # default value

om9 =tk.OptionMenu(my_w, options9, *link3)
tk.Label(my_w, text="Company").grid(row = 6, column = 2)
om9.grid(row=7,column=2)

om10 =tk.OptionMenu(my_w, options10, *zero_one4)
tk.Label(my_w, text="Power").grid(row = 6, column = 3)
om10.grid(row=7,column=3)

options11 = tk.StringVar(my_w)
options11.set(templist[1]) # default value

om11 =tk.OptionMenu(my_w, options11, *templist)
tk.Label(my_w, text="Temperature").grid(row = 6, column = 4)
om11.grid(row=7,column=4)


button3 = tk.Button(my_w, text='Send', width=15, command=function3)
button3.grid(row=7,column=5)



def degis(v,c):
    print(v)
    print(c)
    
        

action_with_arg = partial(degis, 1)
action_with_arg2 = partial(degis, 2)
buton = tk.Scale(orient = tk.HORIZONTAL,length = 50,to = 1,showvalue = False,sliderlength = 25,label = "Off",command = action_with_arg)
#buton.grid(row=6,column=3)
buton2 = tk.Scale(orient = tk.HORIZONTAL,length = 50,to = 1,showvalue = False,sliderlength = 25,label = "Off",command = action_with_arg2)
#buton2.grid(row=7,column=3)

my_w.mainloop()
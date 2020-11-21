import re
import os

f = open("ac_protocol_names.txt", "r")
contents = f.readlines()
count=0
for x in contents:
     x.rstrip()
     count=count+1
     #print(str(count)+'.'+x.strip())
     print("    case \'"+str(count)+"\':")
     print("        Serial.print(\"Protocol: "+x.strip()+"\");")
     print("        ac.next.protocol = decode_type_t::"+x.strip()+";")
     print("        break;")

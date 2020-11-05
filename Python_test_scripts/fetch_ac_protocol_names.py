import re
import os
#Check if the string starts with "The" and ends with "Spain":

f = open("irac.txt", "r")
contents = f.readlines()
count=0
for x in contents:
  y = re.compile('case (.*):')
  z=y.findall(x)
  if z:
     count=count+1
     print(str(count)+'.'+z[0])
     #x.rstrip()
     #print(x.strip())

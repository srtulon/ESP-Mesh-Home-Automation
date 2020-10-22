import re

#Check if the string starts with "The" and ends with "Spain":

f = open("ac_protocol_names.txt", "r")
count=0
for x in f:
  y = re.compile('case (.*):')
  z=y.findall(x)
  if z:
      count=count+1
      print(str(count)+'.'+z[0])


      

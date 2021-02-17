import requests
import json

x =  '{ "name":"John", "age":30, "city":"New York"}'

# parse x:
y = json.loads(x)

z = requests.get('http://cpiot.cpsdbd.com/api/device-data/'+x)

print(y)
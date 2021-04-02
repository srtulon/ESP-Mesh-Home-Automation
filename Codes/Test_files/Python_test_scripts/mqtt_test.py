import paho.mqtt.client as mqtt

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("test/topic")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+(msg.payload).decode("utf-8"))

client = mqtt.Client(client_id="", clean_session=True, userdata=None, transport="tcp")
#client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

#client.tls_set()  # <--- even without arguments
#client.connect("m24.cloudmqtt.com",12947 ,32947 )

client.username_pw_set(username="srtulon", password="abcde1234")
client.connect("45.248.149.225", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()

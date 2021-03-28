import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("test/topic")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print("Message: "+(msg.payload).decode("utf-8"))
    print("Sending to device")
    publish.single( "device/from/", (msg.payload).decode("utf-8"), hostname="192.168.0.102")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("45.248.149.225", 1883, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()

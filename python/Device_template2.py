import os
import time
import sys
import paho.mqtt.client as mqtt
import json
import random

THINGSBOARD_HOST = '192.168.1.100'
PORT = 1883
DEVICE_ID = "Device_2"

sensor_data = {"device": DEVICE_ID,"data": {'temperature': 0, 'humidity': 0}}
sensor_data1 = {'temperature': 0, 'humidity': 0}
sensor_data2 = {DEVICE_ID:{"ts": str(int(time.time())),"values": {'temperature': 0, 'humidity': 0}}}
sensor_data3 = {'temperature': 0, 'humidity': 0}
sensor_data4 = {DEVICE_ID:{'temperature': 0, 'humidity': 0}}

client = mqtt.Client()

# Set access token
# client.username_pw_set("NMK","Mang0s12")
# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
print(json.dumps({'device':DEVICE_ID}))
client.connect(THINGSBOARD_HOST, PORT, 60)
client.publish('v1/gateway/connect', json.dumps({'device':DEVICE_ID}), 1)
client.loop_start()

try:
    while True:
        humidity = random.randint(0,100)
        temperature = random.randint(0,100)
        print(u"Temperature: {:g}\u00b0C, Humidity: {:g}%".format(temperature, humidity))
        sensor_data["data"]['temperature'] = temperature
        sensor_data["data"]['humidity'] = humidity
        sensor_data1['temperature'] = temperature
        sensor_data1['humidity'] = humidity
        sensor_data2[DEVICE_ID]["values"]['temperature'] = temperature
        sensor_data2[DEVICE_ID]["values"]['humidity'] = humidity
        sensor_data2[DEVICE_ID]["ts"] = str(int(time.time()))
        sensor_data3['temperature'] = temperature
        sensor_data3['humidity'] = humidity
        sensor_data4[DEVICE_ID]['temperature'] = temperature
        sensor_data4[DEVICE_ID]['humidity'] = humidity

        # client.publish('v1/gateway/telemetry', json.dumps(sensor_data), 1)
        # client.publish('v1/devices/me/telemetry', json.dumps(sensor_data1), 1)
        # client.publish('v1/devices/me/telemetry', json.dumps(sensor_data1), 1)
        # client.publish('v1/gateway/telemetry', json.dumps(sensor_data2), 1)
        client.publish('v1/gateway/telemetry', '{"'+DEVICE_ID+'":['+json.dumps(sensor_data3)+"]}", 1)
        # client.publish('v1/gateway/attributes', json.dumps(sensor_data4), 1)

        time.sleep(2)
except KeyboardInterrupt:
    pass

client.publish('v1/gateway/disconnect', json.dumps({'device':DEVICE_ID}), 1)
client.loop_stop()
client.disconnect()
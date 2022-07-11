import paho.mqtt.client as mqtt
import json
import random
import time

THINGSBOARD_HOST = '192.168.1.100'
BROKER_PORT = 1883
THINGSBOARD_PORT = 1884
THINGSBOARD_CLIENT_ID = "mVbQi7yxWMgaubTOE6QY"

def on_disconnect_broker(client, userdata, rc):
    print('BR - Disonnected with result code ' + str(rc))

def on_connect_broker(client, userdata, rc, *extra_params):
    print('BR - Connected with result code ' + str(rc))
    client.subscribe('v1/#')
    # client.subscribe('v1/gateway/telemetry')
    # client.subscribe('v1/gateway/connect')
    # client.subscribe('v1/gateway/disconnect')

def on_message_broker(client, userdata, msg):
    global thingsboard
    print('BR - Topic: ' + msg.topic + '\nMessage: ' + str(msg.payload))
    thingsboard.publish(msg.topic, msg.payload, 1)

def on_message_thingsboard(client, userdata, msg):
    global broker
    print('TB - Topic: ' + msg.topic + '\nMessage: ' + str(msg.payload))
    broker.publish(msg.topic, msg.payload, 1)

def on_disconnect_thingsboard(client, userdata, rc):
    print('TB - Disonnected with result code ' + str(rc))

def on_connect_thingsboard(client, userdata, rc, *extra_params):
    print('TB - Connected with result code ' + str(rc))
    client.subscribe('v1/#')

broker = mqtt.Client()
broker.on_connect = on_connect_broker
broker.on_message = on_message_broker
broker.on_disconnect = on_disconnect_broker
broker.connect(THINGSBOARD_HOST, BROKER_PORT, 60)
broker.loop_start()

thingsboard = mqtt.Client(client_id=THINGSBOARD_CLIENT_ID)
thingsboard.on_connect = on_connect_thingsboard
thingsboard.on_message = on_message_thingsboard
thingsboard.on_disconnect = on_disconnect_thingsboard
thingsboard.connect(THINGSBOARD_HOST, THINGSBOARD_PORT, 60)
thingsboard.loop_start()

try:
    while True:
        pass
        # value = random.randint(0,100)
        # # print("publishing",value)
        # sensor_data['test'] = value
        # client.publish('v1/devices/me/attributes', json.dumps(sensor_data), 1)
        # time.sleep(1)
except KeyboardInterrupt:
    pass

broker.loop_stop()
broker.disconnect()
# thingsboard.loop_stop()
# thingsboard.disconnect()
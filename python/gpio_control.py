import paho.mqtt.client as mqtt
import json
import random
import time

THINGSBOARD_HOST = '192.168.1.100'
Client_ID = "Device_3"

sensor_data = {'test': 0}

# We assume that all GPIOs are LOW
gpio_state = {7: False, 11: False, 12: False, 13: False, 15: False, 16: False, 18: False, 22: False, 29: False,
              31: False, 32: False, 33: False, 35: False, 36: False, 37: False, 38: False, 40: False}

def on_disconnect(client, userdata, rc):
    print("disconnecting reason  "  +str(rc))

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc, *extra_params):
    print(rc)
    print('Connected with result code ' + str(rc))
    # Subscribing to receive RPC requests
    client.subscribe('v1/gateway/#')
    # Sending current GPIO status
    # client.publish('v1/devices/me/attributes', get_gpio_status(), 1)


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print('Topic: ' + msg.topic + '   Message: ' + str(msg.payload))
    # Decode JSON request
    data = json.loads(msg.payload)
    # Check request method
    # print(data)
    return
    # if data['method'] == 'getGpioStatus':
    #     # Reply with GPIO status
    #     client.publish(msg.topic.replace('request', 'response'), get_gpio_status(), 1)
    # elif data['method'] == 'setGpioStatus':
    #     # Update GPIO status and reply
    #     set_gpio_status(data['params']['pin'], data['params']['enabled'])
    #     client.publish(msg.topic.replace('request', 'response'), get_gpio_status(), 1)
    #     client.publish('v1/devices/me/attributes', get_gpio_status(), 1)
    try:
        if(data['data']['method'] == "switchstatus"):
            data2 = {'device': 'Device_3',
                    'id' : data['data']['id'],
                    'data': {'switch' : 'true'}}
            client.publish('v1/gateway/rpc/', json.dumps(data2), 1)
    except:
        print("Not publish")


def get_gpio_status():
    # Encode GPIOs state to json
    return json.dumps(gpio_state)


def set_gpio_status(pin, status):
    # Output GPIOs state
    print(pin,status)
    # Update GPIOs state
    gpio_state[pin] = status

client = mqtt.Client(client_id=Client_ID)
# Register connect callback
client.on_connect = on_connect
# Registed publish message callback
client.on_message = on_message
client.on_disconnect = on_disconnect
# Set access token
# client.username_pw_set("NMK","Mang0s12")
# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
client.connect(THINGSBOARD_HOST, 1883, 60)

# try:
#     client.loop_forever()
# except KeyboardInterrupt:
#     pass

client.loop_start()
val = True
counter =  488

try:
    while True:
        # data = {'Device_3': 
        #             {'switch' : val}}
        # val = not val
        # client.publish('v1/gateway/attributes', json.dumps(data), 1)

        # data = {"device":"Device_3","data":{"id":counter, "method":"setrled", "params":123}}
        data = {"device":"Device_3","data":{"method":"getrled", "params":{}}}
        # data = {"device":"Device_3","method":"getrled", "params":{}}
        # val = not val
        client.publish('v1/gateway/rpc/request', json.dumps(data), 1)
        counter += 1

        # data = {"device":"Device_3",
        # 'data': {"method":"switchstatus", 'params' : val}}
        # val = not val
        # client.publish('v1/gateway/attributes', json.dumps(data), 1)
        # pass
        # value = random.randint(0,100)
        # # print("publishing",value)
        # sensor_data['test'] = value
        # client.publish('v1/devices/me/attributes', json.dumps(sensor_data), 1)
        time.sleep(2)
except KeyboardInterrupt:
    pass

client.loop_stop()
client.disconnect()
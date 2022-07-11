/****************************************************/
// Filename: temperature_data.ino
// Created: Mindiya Karunasinghe
// Change history:
// 25.10.2021 / Mindiya Karunasinghe
/****************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

//Wifi
String ssid = "SLT-WIFI 901";
String password = "Mang0s12";

//MQTT used in PubSubClient lib
String mqtt_broker = "192.168.1.100";
String device_name = "Device_3";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(4, DHT11);//pin=4, type=DHT11|DHT22|DHT21 Single Bus Data Format Data pin of the sensor is connected to ESP201 pin

const int N = 2; // We use 2 variable in this example

String topic, message;
float humidity, temperature;

unsigned long int last_time = 0;
String variable_name[N] = {"humidity", "temperature"};// in this example we use only 2(N) variables
String variable_value[N];

/// <summary>
/// This function connects the network using ssid and password variables
/// </summary>
/// <returns>None</returns>
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);// Station mode connecting to external AP
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/// <summary>
/// This function says to thingsboard that this device is connected, via broker.
/// topic - v1/gateway/connect
/// message - {'device':_Device_name}
/// </summary>
/// <returns>None</returns>
void connected_broker() {
  Serial.println("Connected");
  topic = "v1/gateway/connect";
  message = "{'device':" + device_name + "}";
  client.publish(topic.c_str(), message.c_str());
}

/// <summary>
/// This function says to thingsboard that this device is disconnected, via broker.
/// topic - v1/gateway/disconnect
/// message - {'device':_Device_name}
/// </summary>
/// <returns>None</returns>
void disconnected_broker() {
  Serial.println("Disonnected");
  topic = "v1/gateway/disconnect";
  message = "{'device':" + device_name + "}";
  client.publish(topic.c_str(), message.c_str());
}

/// <summary>
/// This function updates telemetry data in thingsboard, via broker.
/// topic - v1/gateway/telemetry
/// message - {_Device_name:[{_type1:_value1, _type2:_value2, _type3:_value3, ...}]}
/// </summary>
/// <param name="variable_name">String array. The data type name</param>
/// <param name="variable_value">String array. The values of the data types</param>
/// <param name="n">Int. Number of parameters</param>
/// <returns>None</returns>
void send_broker(String variable_name[], String variable_value[], int n) {
  DynamicJsonDocument main_json(200);
  JsonArray data_json = main_json.createNestedArray(device_name);
  DynamicJsonDocument value_json(200);
  message = "";
  topic = "v1/gateway/telemetry";
  for (int i = 0; i < n; i++) {
    value_json[variable_name[i]] = variable_value[i];
  }
  data_json.add(value_json);
  serializeJson(main_json, message);

}

/// <summary>
/// This function is called when the client gets a message in a subsribed topic
/// </summary>
/// <param name="topic">Char pointer. The message topic</param>
/// <param name="payload">Byte pointer. The message content</param>
/// <param name="length">Unsigned int. The message content length</param>
/// <returns>None</returns>
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

/// <summary>
/// This function is used to connect to mqtt broker using device name as the client ID.
/// </summary>
/// <returns>None</returns>
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(device_name.c_str())) {
      connected_broker();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_broker.c_str(), 1883);//connecting to port 1883 (MQTT unencriptd unauthenticated)
  client.setCallback(callback);
  dht.begin();
  randomSeed(micros());
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - last_time > 2000) {
    last_time = millis();
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();//in Celsius
    variable_value[0] = String(humidity);
    variable_value[1] = String(temperature);
    send_broker(variable_name, variable_value, N);// convert to Json > String
    client.publish(topic.c_str(), message.c_str());// publish the String to the broker.
  }
}

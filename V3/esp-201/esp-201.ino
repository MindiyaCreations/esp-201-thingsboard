/****************************************************/
// Filename: esp-201.ino
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

//ESP-201 pin numbers
//RGB LED
#define RED_PIN 15
#define GREEN_PIN 13
#define BLUE_PIN 12
//White LED
#define WHITE_PIN 14
//RELAY
#define RELAY_PIN 16
//BUZZER
#define BUZZER_PIN 5
//DHT Sensor
#define DHT_PIN 4
//Switches
#define S2_PIN 0
#define S3_PIN 2

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHT_PIN, DHT11);//type=DHT11|DHT22|DHT21 Single Bus Data Format.

const int N = 2; // We use 2 variable in this example

String topic, message;
float humidity, temperature;

unsigned long int last_time = 0;
String variable_name[N] = {"humidity", "temperature"};// in this example we use only 2(N) variables
String variable_value[N];

/// <summary>
/// This function initializes the pins.
/// </summary>
/// <returns>None</returns>
void pin_Initialize() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(WHITE_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(S2_PIN, INPUT);
  pinMode(S3_PIN, INPUT);

  rgb_led(0, 0);
  rgb_led(1, 0);
  rgb_led(2, 0);
  white_led(false);
  relay(false);
  buzzer(false);
}

/// <summary>
/// This function changes the color of the RGB leds.
/// </summary>
/// <param name="color">Int. The controlling color. 0 -> Red, 1 -> Green, 2 -> Blue</param>
/// <param name="value">Int. The led value 0 - 255</param>
/// <returns>None</returns>
void rgb_led(int color, int value) {
  switch(color){
    case 0:
      analogWrite(RED_PIN, value);
    break;
    case 1:
      analogWrite(GREEN_PIN, value);
    break;
    case 2:
      analogWrite(BLUE_PIN, value);
    break;
  }
}

/// <summary>
/// This function tuns on and off the white led.
/// </summary>
/// <param name="Turning_ON">Bool. Turning the white led on or off</param>
/// <returns>None</returns>
void white_led(bool Turning_ON) {
  digitalWrite(WHITE_PIN, Turning_ON);
}

/// <summary>
/// This function tuns on and off the relay.
/// </summary>
/// <param name="Turning_ON">Bool. Turning the relay on or off</param>
/// <returns>None</returns>
void relay(bool Turning_ON) {
  digitalWrite(RELAY_PIN, Turning_ON);
}

/// <summary>
/// This function tuns on and off the buzzer.
/// </summary>
/// <param name="Turning_ON">Bool. Turning the buzzer on or off</param>
/// <returns>None</returns>
void buzzer(bool Turning_ON) {
  digitalWrite(BUZZER_PIN, Turning_ON);
}

/// <summary>
/// This function returns if the switch is on or off.
/// </summary>
/// <param name="isS2">Bool. If the switch we need to know the state of is S2.</param>
/// <returns>Bool. According to the switch state</returns>
bool button(bool isS2) {
  if (isS2) {
    return digitalRead(S2_PIN);
  }
  return digitalRead(S3_PIN);
}

/// <summary>
/// This function returns if the String has true or false.
/// </summary>
/// <param name="String">String. The message we need to check if it has true.</param>
/// <returns>Bool. According to the string</returns>
bool is_bool_true(String message){
  if(message == "TRUE" or message == "true" or message == "True"){
    return true;
  }
  return false;
}

/// <summary>
/// This function connects the network using ssid and password variables.
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
/// This function creates the topic and the message for sending telemetry data in thingsboard, via broker.
/// topic - v1/gateway/telemetry
/// message - {_Device_name:[{_type1:_value1, _type2:_value2, _type3:_value3, ...}]}
/// </summary>
/// <param name="variable_name">String array. The data type name</param>
/// <param name="variable_value">String array. The values of the data types</param>
/// <param name="n">Int. Number of parameters</param>
/// <returns>None</returns>
void make_mqtt_message(String variable_name[], String variable_value[], int n) {
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
/// This function is called when the client gets a message in a subsribed topic.
/// Also controls the ouputs if the message was seent to this client.
/// This function controls relay, buzzer, red led, green led, blue led, white led/
/// </summary>
/// <param name="topic">Char pointer. The message topic</param>
/// <param name="payload">Byte pointer. The message content</param>
/// <param name="length">Unsigned int. The message content length</param>
/// <returns>None</returns>
void callback(char* topic, byte* payload, unsigned int length) {
  DynamicJsonDocument main_json(200);
  DeserializationError error = deserializeJson(main_json, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
//  serializeJsonPretty(main_json, Serial);
//  Serial.println();
  if (main_json["device"] != device_name) {
    Serial.println("Not the device thingsboard sent");
    return;
  }
  String method_name = main_json["data"]["method"];
  String params = main_json["data"]["params"];
  Serial.println(method_name);
  Serial.println(params);
  Serial.println();

  if(method_name == "set_relay"){
    relay(is_bool_true(params));
  }else if(method_name == "setbuzzer"){
    buzzer(is_bool_true(params));
  }else if(method_name == "setrled"){
    rgb_led(0, params.toInt());
  }else if(method_name == "setgled"){
    rgb_led(1, params.toInt());
  }else if(method_name == "setbled"){
    rgb_led(2, params.toInt());
  }else if(method_name == "setwhite"){
    white_led(is_bool_true(params));
  }
}

/// <summary>
/// This function creates the topic and the message for sending telemetry data in thingsboard, via broker.
/// topic - v1/gateway/attributes
/// message - {_Device_name:[{_type1:_value1, _type2:_value2, _type3:_value3, ...}]}
/// </summary>
/// <returns>None</returns>
void switch_update(){
  DynamicJsonDocument main_json(200);
  JsonObject data_json = main_json.createNestedObject(device_name);
  data_json["switch"] = not button(false);
  message = "";
  topic = "v1/gateway/attributes";
  serializeJson(main_json, message);
//  serializeJsonPretty(main_json, Serial);
//  Serial.println();
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
      client.subscribe("v1/gateway/rpc");
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
  pin_Initialize();
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
    switch_update();
    client.publish(topic.c_str(), message.c_str());// publish the switch status to the broker.
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();//in Celsius
    variable_value[0] = String(humidity);
    variable_value[1] = String(temperature);
    make_mqtt_message(variable_name, variable_value, N);// convert to Json > String
    client.publish(topic.c_str(), message.c_str());// publish the String to the broker.
  }
}

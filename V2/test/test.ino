#include <thingsboarduino.h>

thingsboarduino device;

void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  device.connect_wifi("SLT-WIFI 901", "Mang0s12");
  device.connect_broker("192.168.1.100", "Device_3", 1883);
}

void loop() {
  device.loop();
  int arr[2] = {random(0, 100), random(0, 100)};
  String names[2] = {"temp", "humidity"};
  device.send_data(names, arr, 2);
  delay(1000);
}

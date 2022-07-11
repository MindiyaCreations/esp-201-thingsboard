void setup() {
  Pin_Initialize();
  Serial.begin(115200);
}

void loop() {
 RGB(1, 0, 0);
 Serial.println("RED");
 delay(1000);
 RGB(0, 1, 0);
 Serial.println("GREEN");
 delay(1000);
 RGB(0, 0, 1);
 Serial.println("BLUE");
 delay(1000);
 RGB(0, 0, 0);

 White_LED(true);
 delay(1000);
 White_LED(false);
 delay(1000);

 Relay(true);
 delay(1000);
 Relay(false);
 delay(1000);

 Buzzer(true);
 delay(1000);
 Buzzer(false);
 delay(1000);

 Serial.print("S2 ");
 Serial.println(Button(true));
 Serial.print("S3 ");
 Serial.println(Button(false));

 readDHTSensor();
}
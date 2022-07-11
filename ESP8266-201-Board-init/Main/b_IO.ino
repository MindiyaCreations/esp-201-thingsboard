#include "DHT.h"
DHT dht(DHT_PIN, DHT11);

void Pin_Initialize() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  pinMode(WHITE_PIN, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(S2_PIN, INPUT);
  pinMode(S3_PIN, INPUT);

  RGB(0, 0, 0);
  White_LED(false);
  Relay(false);
  Buzzer(false);
  dht.begin();
}

void RGB(bool R, bool G, bool B) {
  digitalWrite(RED_PIN, R);
  digitalWrite(GREEN_PIN, G);
  digitalWrite(BLUE_PIN, B);
}

void White_LED(bool Turning_ON) {
  digitalWrite(WHITE_PIN, Turning_ON);
}

void Relay(bool Turning_ON) {
  digitalWrite(RELAY_PIN, Turning_ON);
}

void Buzzer(bool Turning_ON) {
  digitalWrite(BUZZER_PIN, Turning_ON);
}

bool Button(bool isS2) {
  if (isS2) {
    return digitalRead(S2_PIN);
  }
  return digitalRead(S3_PIN);
}

void readDHTSensor(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}

#include "DHT.h"

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
  DHT dht(DHT_PIN, DHT11);
}


void RGB(int R, int G, int B) {
  analogWrite(RED_PIN, R);
  analogWrite(GREEN_PIN, R);
  analogWrite(BLUE_PIN, R);
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

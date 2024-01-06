#include <Arduino.h>
#include "manager.h"
#include "utils.h"
// setup pin connect
const int lampPin = 4; // Modify this according to your setup
const int indicatorPin = 5; // Modify this according to your setup

Manager manager;

void setup() {
  // put your setup code here, to run once:
  pinMode(lampPin, OUTPUT);
  pinMode(indicatorPin, OUTPUT);

  Serial.begin(9600);
  manager.setup();
  manager.add_device(lampPin);
  setTimeFromNTP();
  digitalWrite(indicatorPin, HIGH);
  digitalWrite(lampPin, HIGH);
  delay(1000);
  digitalWrite(lampPin, LOW);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
  manager.loop();
}
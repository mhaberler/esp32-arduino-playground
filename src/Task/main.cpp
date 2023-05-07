// example from https://github.com/pschatzmann/arduino-freertos-addons/tree/main

#include "freertos-all.h"
#include <Arduino.h>
#define TOGGLE(pin) digitalWrite(pin, !digitalRead(pin))

CyclicTask task(
    "fast", 1000, 3, []() { TOGGLE(IMU_PIN); }, 10);
CyclicTask task2(
    "slow", 1000, 2, []() { TOGGLE(REPORT_PIN); }, 50);

void setup() {
  int i = 4711;
  Serial.begin(115200);
  delay(1000);

  pinMode(ISR_PIN, OUTPUT);
  digitalWrite(ISR_PIN, LOW);
  pinMode(IMU_PIN, OUTPUT);
  digitalWrite(IMU_PIN, LOW);
  pinMode(REPORT_PIN, OUTPUT);
  digitalWrite(REPORT_PIN, LOW);
  pinMode(LOOP_PIN, OUTPUT);
  digitalWrite(LOOP_PIN, LOW);

  Serial.println("starting tasks:");
  task.Start(1);
  task2.Start(0);
  delay(300);
  TOGGLE(ISR_PIN);
  task2.setRate(5);
  delay(100);
  task.suspend();
  delay(100);
  task.resume();
  delay(100);
  task.suspend();
  delay(100);
  task.resume();
}

void loop() {
  digitalWrite(LOOP_PIN, HIGH);
  delay(100);
  digitalWrite(LOOP_PIN, LOW);
  delay(100);
}
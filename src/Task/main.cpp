// example from https://github.com/pschatzmann/arduino-freertos-addons/tree/main

#include "CyclicTask.hpp"
#include "freertos-all.h"
#include <Arduino.h>

CyclicTask task(
    "repeat", 1000, 10, []() { Serial.println("hi"); });
CyclicTask task2(
    "repeat2", 1000, 10, []() { Serial.printf("there\n"); });

void setup() {
  int i = 4711;
  Serial.begin(115200);
  delay(1000);
  Serial.println("starting Task:");
  task.Start(1);
  task2.Start(0);
}

void loop() { delay(100); }
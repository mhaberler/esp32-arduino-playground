// example from https://github.com/pschatzmann/arduino-freertos-addons/tree/main

#include "freertos-all.h"
#include <Arduino.h>
#include "CyclicTask.hpp"


// Task task("name",1000,10, [](){ static size_t count=0;
// queue.Enqueue(&(++count));});
Task task(
    "name", 1000, 10, []() { Serial.println("hi"); }, 5);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("starting Task:");
  task.Start(0);
}

void loop() { delay(100); }
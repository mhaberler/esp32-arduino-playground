// example from https://github.com/pschatzmann/arduino-freertos-addons/tree/main


#include "freertos-all.h"
#include <Arduino.h>

Queue queue(100, sizeof(size_t));
Task task("name",1000,10, [](){ static size_t count=0; queue.Enqueue(&(++count));});

void setup() {
  Serial.begin(115200);
  task.Start(0);
}

void loop() {
  size_t result;
  queue.Dequeue(&result);
  Serial.println(result);
}
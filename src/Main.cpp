#include <Arduino.h>
#include "Motor.hpp"

Motor motor(Serial1, 1);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 25, 26);

  Serial.println("Hello, World!");
  
  delay(2000);
}

void loop() {
  // motor.set_speed(...) 等调用
  
  motor.init();
  motor.set_speed(30, 0);

  while (1) {
    delay(200);
  }
}
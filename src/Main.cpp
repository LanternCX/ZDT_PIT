#include <Arduino.h>
#include "Motor.hpp"

Motor motor_x(Serial1, 1);

Motor motor_y(Serial1, 2);

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 25, 26);

  Serial.println("Hello, World!");
  
  delay(2000);

  motor_x.init();
  motor_y.init();
}

void loop() {
  static int cnt = 0;
  static int dir = 1;
  if (cnt % 15 == 0) {
    motor_x.set_speed(5 * dir, 0);
    motor_y.set_speed(30, 0);
    cnt %= 15;
    dir *= -1;
  } else {
    cnt++;
  }
  delay(1000);
}
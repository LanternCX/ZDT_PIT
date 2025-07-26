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
  static bool enable = 1;
  if (cnt % 15 == 0) {
    if (enable) {
      motor_x.set_speed(5 * dir, 0);
      motor_y.set_speed(2 * dir, 0);
      cnt %= 15;
      dir *= -1;
    }
  } else {
    cnt++;
  }

  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    input.trim();

    if (input == "STOP") {
      Serial.println("Stop all motor");
      // motor_x.exit();
      // motor_y.exit();
      motor_y.set_speed(0, 0);
      motor_x.set_speed(0, 0);
      enable = 0;
    } else if (input == "RUN"){
      Serial.println("Motor run");
      enable = 1;
    } else {
      Serial.println("Serial received: " + input);
    }
  }
  delay(1000);
}
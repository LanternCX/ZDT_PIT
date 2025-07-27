#include <Arduino.h>
#include "Stepper.hpp"

Stepper motor_x(Serial1, 1);

Stepper motor_y(Serial1, 2);

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

  static int x_speed = 0;
  static int y_speed = 0;

  static int x_pos = 0;
  static int y_pos = 0;

  // 串口协议处理
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    input.trim();

    if (input.startsWith("STOP")) {
      Serial.println("Stop all motor");
      motor_x.set_speed(0);
      motor_y.set_speed(0);
      enable = 0;
    } 

    if (input.startsWith("RUN")) {
      Serial.println("Motor run");
      enable = 1;
    } 

    if (input.startsWith("SPEED")) {
      // 分割字符串
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      int thirdSpace = input.indexOf(' ', secondSpace + 1);

      if (firstSpace > 0 && secondSpace > 0 && thirdSpace == -1) {
        String xStr = input.substring(firstSpace + 1, secondSpace);
        String yStr = input.substring(secondSpace + 1);

        int x = xStr.toInt();
        int y = yStr.toInt();

        x_speed = x;
        y_speed = y;

        x_speed = max(min(x_speed, 10), -10);
        y_speed = max(min(y_speed, 10), -10);

        Serial.println("x: " + String(x_speed) + " y: " + String(y_speed));
        if (enable) {
          motor_x.set_speed(x_speed);
          motor_y.set_speed(y_speed);
        }
      }
    }

    if (input.startsWith("POS_SET")) {
      // 分割字符串
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      int thirdSpace = input.indexOf(' ', secondSpace + 1);

      if (firstSpace > 0 && secondSpace > 0 && thirdSpace == -1) {
        String xStr = input.substring(firstSpace + 1, secondSpace);
        String yStr = input.substring(secondSpace + 1);

        int x = xStr.toInt();
        int y = yStr.toInt();

        x_pos = x;
        y_pos = y;

        x_pos = max(0, x_pos);
        y_pos = max(0, y_pos);

        Serial.println("x: " + String(x_pos) + " y: " + String(y_pos));
        if (enable) {
          motor_x.set_position(x_pos, 30);
          motor_y.set_position(y_pos, 30);
        }
      }
    }

    if (input.startsWith("POS_ADD")) {
      // 分割字符串
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      int thirdSpace = input.indexOf(' ', secondSpace + 1);

      if (firstSpace > 0 && secondSpace > 0 && thirdSpace == -1) {
        String xStr = input.substring(firstSpace + 1, secondSpace);
        String yStr = input.substring(secondSpace + 1);

        int x = xStr.toInt();
        int y = yStr.toInt();

        x_pos = x;
        y_pos = y;

        Serial.println("x: " + String(x_pos) + " y: " + String(y_pos));
        if (enable) {
          motor_x.add_position(x_pos, 30);
          motor_y.add_position(y_pos, 30);
        }
      }
    }
  }
  delay(1);
}
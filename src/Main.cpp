#include <Arduino.h>
#include "Stepper.hpp"

#include "PTZ.hpp"

PTZ ptz(Serial1, 1, 2);
void serial_on_msg();

void setup() {
  Serial.begin(115200);
  Serial1.begin(115200, SERIAL_8N1, 25, 26);

  Serial.println("Hello, World!");
  
  delay(2000);

  ptz.init();
}

void loop() {
  // 串口协议处理
  if (Serial.available()) {
    serial_on_msg();
  }
  delay(1);
}

void serial_on_msg() {
  static bool enable = 1;
  String input = Serial.readStringUntil('\n');

  input.trim();

  if (input.startsWith("STOP")) {
    Serial.println("Stop all motor");
    ptz.reset();
    enable = 0;
  } 

  if (input.startsWith("RESET")) {
    Serial.println("Stop all motor");
    if (enable) {
      ptz.reset();
    }
  } 

  if (input.startsWith("RUN")) {
    Serial.println("Motor run");
    enable = 1;
  } 

  if (input.startsWith("SPEED")) {
    int firstSpace = input.indexOf(' ');
    int secondSpace = input.indexOf(' ', firstSpace + 1);
    int thirdSpace = input.indexOf(' ', secondSpace + 1);

    if (firstSpace > 0 && secondSpace > 0 && thirdSpace == -1) {
      String xStr = input.substring(firstSpace + 1, secondSpace);
      String yStr = input.substring(secondSpace + 1);

      int x = xStr.toInt();
      int y = yStr.toInt();

      int x_speed = x;
      int y_speed = y;

      x_speed = max(min(x_speed, 10), -10);
      y_speed = max(min(y_speed, 10), -10);

      Serial.println("x: " + String(x_speed) + " y: " + String(y_speed));
      if (enable) {
        ptz.set_x_speed(x_speed);
        ptz.set_y_speed(y_speed);
      }
    }
  }

  if (input.startsWith("DEG_SET")) {
    int firstSpace = input.indexOf(' ');
    int secondSpace = input.indexOf(' ', firstSpace + 1);
    int thirdSpace = input.indexOf(' ', secondSpace + 1);

    if (firstSpace > 0 && secondSpace > 0 && thirdSpace == -1) {
      String xStr = input.substring(firstSpace + 1, secondSpace);
      String yStr = input.substring(secondSpace + 1);

      int x = xStr.toInt();
      int y = yStr.toInt();

      int x_angle = x;
      int y_angle = y;

      x_angle = max(0, x_angle);
      y_angle = max(0, y_angle);

      Serial.println("x: " + String(x_angle) + " y: " + String(y_angle));
      if (enable) {
        ptz.set_x_angle(x_angle);
        ptz.set_y_angle(y_angle);
      }
    }
  }

  if (input.startsWith("DEG_ADD")) {
    int firstSpace = input.indexOf(' ');
    int secondSpace = input.indexOf(' ', firstSpace + 1);
    int thirdSpace = input.indexOf(' ', secondSpace + 1);

    if (firstSpace > 0 && secondSpace > 0 && thirdSpace == -1) {
      String xStr = input.substring(firstSpace + 1, secondSpace);
      String yStr = input.substring(secondSpace + 1);

      int x = xStr.toInt();
      int y = yStr.toInt();

      int x_angle = x;
      int y_angle = y;

      Serial.println("x: " + String(x_angle) + " y: " + String(y_angle));
      if (enable) {
        ptz.add_x_angle(x_angle);
        ptz.add_y_angle(y_angle);
      }
    }
  }
}
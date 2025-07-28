#include <Arduino.h>
#include "Stepper.hpp"

#include "PTZ.hpp"

PTZ ptz(Serial1, 1, 2);
void serial_on_msg();
void draw_circle();
bool draw_circle_flg = 0;
bool draw_sin_flg = 0;

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
  if (draw_circle) {
    draw_circle();
  }
  delay(1);
}

void draw_circle() {
  const int tot = 500;
  const float x0 = 0, y0 = 0; 
  const float r = 3;
  const float a = 2;
  const float b = 3;
  static int t = 0;

  float theta = 2 * PI * t / tot;
  float x = x0 + a * cos(theta);
  float y = y0 + b * sin(theta);

  ptz.move_to(10, x, y, 300, 0);

  // Serial.println("x : " + String(x) + " y: " + String(y));

  t = (t + 1) % tot; // 周期推进
  t++;
  delay(20); // 每20ms更新一次
}

void draw_sin() {
  const int tot = 500;  // 总点数
  const float x0 = 0, y0 = 0;  // 中心点
  const float amplitude = 3;   // 正弦波振幅
  const float wavelength = 2;  // 波长（控制正弦波的周期）
  static int t = 0;

  // x 线性变化，y 为正弦波
  float x = x0 + wavelength * (2 * PI * t / tot);
  float y = y0 + amplitude * sin(2 * PI * t / tot);

  ptz.move_to(10, x, y, 10, 0);

  t = (t + 1) % tot; // 周期推进
  delay(20); // 每20ms更新一次
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
        ptz.sync_all();
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

      Serial.println("x: " + String(x_angle) + " y: " + String(y_angle));
      if (enable) {
        ptz.set_x_angle(x_angle);
        ptz.set_y_angle(y_angle);
        ptz.sync_all();
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
        ptz.sync_all();
      }
    }
  }

  if (input.startsWith("CIRCLE")) {
    Serial.println("Draw Circle");
    draw_circle_flg = !draw_circle_flg;
    draw_sin_flg = 0;
  }

  if (input.startsWith("SIN")) {
    Serial.println("Draw SIN");
    draw_sin_flg = !draw_circle_flg;
    draw_circle_flg = 0;
  }
}
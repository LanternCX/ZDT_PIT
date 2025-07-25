#include <Arduino.h>
#include "Motor.hpp"

Motor::Motor(HardwareSerial& _serial, int _addr) 
    : serial(_serial), addr(_addr) {}

/**
 * @brief 电机初始化
 */
void Motor::init() {
    // 使能电机
    Motor::enable();

    Serial.println("Serial available: " + String(serial.available()));
    // 停止电机运行
    // Motor::stop();
    // 设置电机零点
    // Motor::set_zero();
    return;
}

/**
 * @brief 使能电机
 */
void Motor::enable() {
    uint8_t cmd[16] = {0};
    // 地址
    cmd[0] =  (uint8_t) Motor::addr;

    // 电机使能
    // 功能码
    cmd[1] =  0xF3;
    // 辅助码
    cmd[2] =  0xAB;
    // 使能状态
    cmd[3] =  0x01;
    // 多机同步运动标志  
    cmd[4] =  0x00;
    // 校验字节
    cmd[5] =  0x6B;
    
    Serial.println("Enable: ");
    for (int i = 0; i < 6; i++) {
        Serial.println("idx: " + String(i) + " " + String(cmd[i], HEX));
    }

    serial.write(cmd, 6);
}

/**
 * @brief 关闭电机
 */
void Motor::disable() {
    uint8_t cmd[16] = {0};
    // 地址
    cmd[0] =  (uint8_t) Motor::addr;

    // 电机使能
    // 功能码
    cmd[1] =  0xF3;
    // 辅助码
    cmd[2] =  0xAB;
    // 使能状态
    cmd[3] =  0x01;
    // 多机同步运动标志  
    cmd[4] =  0x00;
    // 校验字节
    cmd[5] =  0x6B;

    serial.write(cmd, 6);
}

/**
 * @brief 设置当前位置为电机零点
 */
void Motor::set_zero() {
    uint8_t cmd[16] = {0};
    // 地址
    cmd[0] =  (uint8_t) Motor::addr;

    // 设置当前位置为零点
    // 功能码
    cmd[1] =  0x0A;
    // 辅助码
    cmd[2] =  0x6D;
    // 校验字节
    cmd[3] =  0x6B;

    serial.write(cmd, 4);
}

/**
 * @brief 停止电机运动，清空状态
 */
void Motor::stop() {
    uint8_t cmd[16] = {0};
    // 地址
    cmd[0] =  (uint8_t) Motor::addr;

    // 停止电机运动, 清空状态
    // 功能码
    cmd[1] =  0xFE;
    // 辅助码                   
    cmd[2] =  0x98;
    // 多机同步运动标志
    cmd[3] =  0x00;
    // 校验字节   
    cmd[4] =  0x6B;

    serial.write(cmd, 5);
}

/**
 * @brief 速度模式
 * @param speed 速度（单位 rpm，范围0 - 5000）, 正值表示 CCW，负值表示 CW
 * @param acc 加速度，范围0 - 255，0是直接启动
 */
void Motor::set_speed(int16_t speed, uint8_t acc) {
    if ((int) speed > 5000 || (int) acc > 255) {
        return;
    }

    uint8_t cmd[16] = {0};

    // 装载命令
    // 地址
    cmd[0] =  (uint8_t) Motor::addr;
    // 功能码        
    cmd[1] =  0xF6;
    // 方向
    cmd[2] =  speed > 0 ? 0x01 : 0x00;
    // 速度(RPM)高8位字节
    cmd[3] =  (uint8_t)(abs(speed) >> 8);
    // 速度(RPM)低8位字节
    cmd[4] =  (uint8_t)(abs(speed) >> 0);
    // 加速度，注意：0是直接启动
    cmd[5] =  acc;
    // 多机同步运动标志
    cmd[6] =  0x00;
    // 校验字节
    cmd[7] =  0x6B;

    Serial.println("Set Speed: ");
    for (int i = 0; i < 8; i++) {
        Serial.println("idx: " + String(i) + " " + String(cmd[i], HEX));
    }
    
    // 发送命令
    serial.write(cmd, 8);
}

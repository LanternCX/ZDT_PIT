#include "Stepper.hpp"

Stepper::Stepper(HardwareSerial& serial, int addr)
    : serial_(serial), addr_(addr) {}

void Stepper::init() {
    enable();
    // stop();
    // set_zero();
}

void Stepper::to_zero() {
    uint8_t data[1] = {0x6D};
    send_command(0x0A, data, 1);
}

void Stepper::enable() {
    uint8_t data[1] = {0x01};
    send_command(0xF3, data, 1);
}

void Stepper::disable() {
    uint8_t data[1] = {0x00};
    send_command(0xF3, data, 1);
}

void Stepper::set_speed(int16_t speed, uint8_t acc, bool is_sync) {
    uint8_t dir = speed > 0 ? 0x01 : 0x00;
    int16_t rpm = abs(speed);
    rpm = rpm > 5000 ? 5000 : rpm;

    uint8_t cmd[5];
    // 方向
    cmd[0] =  dir;
    // 速度(RPM)高8位字节                   
    cmd[1] =  (uint8_t)(rpm >> 8);
    // 速度(RPM)低8位字节
    cmd[2] =  (uint8_t)(rpm >> 0);
    // 加速度，注意：0是直接启动
    cmd[3] =  acc;
    // 多机同步运动标志
    cmd[4] =  is_sync;

    send_command(0xF6, cmd, 5);
}

void Stepper::set_position(uint32_t step, int16_t speed, uint8_t acc, bool is_sync) {
    uint8_t dir = speed > 0 ? 0x01 : 0x00;
    uint16_t rpm = abs(speed);
    rpm = rpm > 5000 ? 5000 : rpm;

    uint32_t clk = step;

    uint8_t cmd[10];
    cmd[0] = dir;
    // 速度高字节
    cmd[1] = (uint8_t)(rpm >> 8);
    // 速度低字节
    cmd[2] = (uint8_t)(rpm);
    // 加速度
    cmd[3] = acc;
    // 脉冲数第 1 字节
    cmd[4] = (uint8_t)(clk >> 24);
    // 脉冲数第 2 字节
    cmd[5] = (uint8_t)(clk >> 16);
    // 脉冲数第 3 字节
    cmd[6] = (uint8_t)(clk >> 8);
    // 脉冲数第 4 字节
    cmd[7] = (uint8_t)(clk);
    // 相位/绝对标志，false为相对运动，true为绝对值运动
    cmd[8] = true;
    // 多机同步运动标志，false为不启用，true为启用
    cmd[9] = is_sync;

    send_command(0xFD, cmd, 10);
}

void Stepper::add_position(int32_t step, uint16_t speed, uint8_t acc, bool is_sync) {
    uint8_t dir = step > 0 ? 0x01 : 0x00;
    int32_t clk = abs(step);
    uint16_t rpm = speed > 5000 ? 5000 : speed;

    uint8_t cmd[10];
    cmd[0] = dir;
    // 速度高字节
    cmd[1] = (uint8_t)(rpm >> 8);
    // 速度低字节
    cmd[2] = (uint8_t)(rpm);
    // 加速度
    cmd[3] = acc;
    // 脉冲数第 1 字节
    cmd[4] = (uint8_t)(clk >> 24);
    // 脉冲数第 2 字节
    cmd[5] = (uint8_t)(clk >> 16);
    // 脉冲数第 3 字节
    cmd[6] = (uint8_t)(clk >> 8);
    // 脉冲数第 4 字节
    cmd[7] = (uint8_t)(clk);
    // 相位/绝对标志，false为相对运动，true为绝对值运动
    cmd[8] = false;
    // 多机同步运动标志，false为不启用，true为启用
    cmd[9] = is_sync;

    send_command(0xFD, cmd, 10);
}

void Stepper::set_zero(bool is_store) {
    uint8_t cmd[2];
    cmd[0] = 0x88;
    cmd[1] = is_store;
    send_command(0x93, cmd, 2);
}

void Stepper::stop(bool is_sync) {
    uint8_t cmd[2];
    cmd[0] = 0x98;
    cmd[1] = is_sync;
    send_command(0xFE, cmd, 2);
}

void Stepper::reset() {
    set_speed(0);
    set_position(0);
}

void Stepper::sync_all(HardwareSerial& serial) {
    uint8_t buf[5];
    buf[0] = 0x3E;
    buf[1] = 0x00;
    buf[2] = 0x09;
    buf[3] = 0x01;
    buf[4] = 0x48;
    serial.write(buf, 5);
}

void Stepper::send_command(uint8_t cmd, const uint8_t* data, uint8_t len) {
    uint8_t buf[20];
    buf[0] = addr_;
    buf[1] = cmd;
    for (uint8_t i = 0; i < len; ++i) {
        buf[2 + i] = data[i];
    }
    buf[2 + len] = 0x6B;

    delay(1);
    serial_.write(buf, 3 + len);
    // 延迟 1ms 防止总线冲突
    delay(1);
}

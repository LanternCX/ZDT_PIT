#include "Stepper.hpp"

Stepper::Stepper(HardwareSerial& serial, int addr)
    : serial_(serial), addr_(addr) {}

    
/**
 * @brief 电机初始化：使能 -> 停止 -> 设零
 */
void Stepper::init() {
    enable();
    // stop();
    // set_zero();
}

/**
 * @brief 将当前位置清零
 */
void Stepper::to_zero() {
    uint8_t data[1] = {0x6D};
    send_command(0x0A, data, 1);
}
    
/**
 * @brief 启用电机
 */
void Stepper::enable() {
    uint8_t data[1] = {0x01};
    send_command(0xF3, data, 1);
}

/**
 * @brief 关闭电机
 */
void Stepper::disable() {
    uint8_t data[1] = {0x00};
    send_command(0xF3, data, 1);
}

/**
 * @brief 速度模式控制
 * @param speed 转速（单位 rpm，正为 CCW，负为 CW，范围 ±5000）
 * @param acc 加速度（0~255），0是直接启动
 * @param is_sync 是否等待同步启动（默认 false）
 */
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

/**
 * @brief 位置模式控制
 * @param speed 转速（单位 rpm，正为 CCW，负为 CW，范围 ±5000）
 * @param acc 加速度（0~255），0 表示直接启动
 * @param clk 脉冲数（单位：step，范围 0 ~ 2^32-1）
 * @param is_absolute 是否为绝对位置模式（true 为绝对位置，false 为相对位置）
 * @param is_sync 是否等待同步启动（默认 false）
 */
void Stepper::set_position(uint32_t clk, int16_t speed, uint8_t acc, bool is_absolute, bool is_sync) {
    uint8_t dir = speed > 0 ? 0x01 : 0x00;
    uint16_t rpm = abs(speed);
    rpm = rpm > 5000 ? 5000 : rpm;

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
    cmd[8] = is_absolute;
    // 多机同步运动标志，false为不启用，true为启用
    cmd[9] = is_sync;

    send_command(0xFD, cmd, 10);
}

/**
 * @brief 设置单圈回零的零点位置
 * @param is_store 是否存储标志，false为不存储，true为存储
 */
void Stepper::set_zero(bool is_store) {
    uint8_t cmd[2];
    cmd[0] = 0x88;
    cmd[1] = is_store;
    send_command(0x93, cmd, 2);
}

/**
 * @brief 立即停止（所有控制模式都通用）
 */
void Stepper::stop(bool is_sync) {
    uint8_t cmd[2];
    cmd[0] = 0x98;
    cmd[1] = is_sync;
    send_command(0xFE, cmd, 2);
}

/**
 * @brief 多机同步运动
 * @param serial 使用的串口
 */
void Stepper::sync_all(HardwareSerial& serial) {
    uint8_t buf[5];
    buf[0] = 0x3E;
    buf[1] = 0x00;
    buf[2] = 0x09;
    buf[3] = 0x01;
    buf[4] = 0x48;
    serial.write(buf, 5);
}

/**
 * @brief 发送控制指令到驱动器
 * @param cmd 指令码
 * @param data 数据数组（最多 4 字节）
 * @param len 数据长度
 */
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

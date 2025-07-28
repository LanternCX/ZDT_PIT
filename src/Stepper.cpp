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

void Stepper::set_position(int32_t step, uint16_t speed, uint8_t acc, bool is_sync) {
    uint8_t dir = step > 0;
    uint16_t rpm = speed > 5000 ? 5000 : speed;
    step %= 3200;
    
    uint32_t clk = abs(step);

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
    uint8_t buf[4];
    buf[0] = 0x00;
    buf[1] = 0xFF;
    buf[2] = 0x66;
    buf[3] = 0x6B;
    delay(1);
    serial.write(buf, 4);
    delay(1);
}

void Stepper::send_command(uint8_t cmd, const uint8_t* data, uint8_t len) {
    uint8_t buf[20];
    buf[0] = addr_;
    buf[1] = cmd;
    for (uint8_t i = 0; i < len; ++i) {
        buf[2 + i] = data[i];
    }
    buf[2 + len] = 0x6B;

    if (STEPPER_DEBUG) {
        String debug;
        for (int i = 0; i < 3 + len; i++) {
            if (buf[i] < 0x10) debug += "0";  // 补0
            debug += String(buf[i], HEX) + " ";
        }
        Serial.println("Send: " + debug);
    }

    delay(1);
    serial_.write(buf, 3 + len);
    // 延迟 1ms 防止总线冲突
    delay(1);
}

float Stepper::get_angle() {
    // 等待所有命令发送完毕
    delay(1);
    // 清空串口缓冲区
    while (serial_.available()) {
        serial_.read();
    }

    // 发送读取实时位置命令
    send_command(0x36, nullptr, 0);

    const int expected_len = 8;
    uint8_t buffer[expected_len];
    int index = 0;

    unsigned long start_time = millis();

    // 滑动窗口，寻找帧头 addr_ + 0x36
    while (true) {
        // 超时保护：2 秒内未找到帧头则退出
        if (millis() - start_time > 2000) {
            Serial.println("Timeout waiting for response header");
            return 0.0f;
        }

        // 确保至少有两个字节
        if (serial_.available() < 2) {
            continue;
        }

        uint8_t b1 = serial_.read();
        uint8_t b2 = serial_.read();
        if (b1 != addr_ || b2 != 0x36) {
            continue;
        }

        // 帧头匹配成功，保存前两个字节
        buffer[0] = b1;
        buffer[1] = b2;

        // 接收后续6字节
        int remaining = 6;
        int i = 2;
        unsigned long last_recv_time = millis();

        while (remaining > 0) {
            if (serial_.available()) {
                buffer[i++] = serial_.read();
                remaining--;
                last_recv_time = millis();
            } else {
                if (millis() - last_recv_time > 100) {
                    Serial.println("Timeout receiving remaining data");
                    return 0.0f;
                }
            }
        }
        break;  // 成功接收到完整帧，跳出主循环
    }

    if (STEPPER_DEBUG) {
        // 打印调试数据
        String debug;
        for (int i = 0; i < 8; i++) {
            if (buffer[i] < 0x10) debug += "0";  // 补0
            debug += String(buffer[i], HEX) + " ";
        }
        Serial.println("Received: " + debug);
    }

    // 解析符号和位置
    bool is_negative = (buffer[2] == 0x01);
    uint32_t pos = ((uint32_t)(buffer[3]) << 24) 
        | ((uint32_t)(buffer[4]) << 16)
        | ((uint32_t)(buffer[5]) << 8)
        | ((uint32_t)(buffer[6]) << 0);

    float angle = pos * 360.0f / 65536.0f;
    if (is_negative) angle = -angle;

    return angle;
}



float Stepper::step_to_degree(int32_t steps) {
    return static_cast<float>(steps) * 360.0f / 3200.0f;
}
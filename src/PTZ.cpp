#include "PTZ.hpp"

PTZ::PTZ(HardwareSerial& _serial, int _addr_x, int _addr_y)
    : serial(_serial), stepper_x(_serial, _addr_x), stepper_y(_serial, _addr_y) {}

void PTZ::init() {
    stepper_x.init();
    stepper_y.init();
}

void PTZ::set_x_speed(int16_t speed, uint8_t acc) {
    stepper_x.set_speed(speed, acc, true);
}

void PTZ::set_y_speed(int16_t speed, uint8_t acc) {
    stepper_y.set_speed(speed, acc, true);
}

int32_t PTZ::degree_to_step(float angle) {
    return static_cast<int32_t>((angle) * 3200.0f / 360.0f + 0.5f);
}

void PTZ::set_x_angle(float angle, int16_t speed, uint8_t acc) {
    int32_t step = degree_to_step(angle);
    stepper_x.set_position(step, speed, acc, true);
}

void PTZ::set_y_angle(float angle, int16_t speed, uint8_t acc) {
    int32_t step = degree_to_step(angle);
    stepper_y.set_position(step, speed, acc, true);
}

void PTZ::add_x_angle(float angle, uint16_t speed, uint8_t acc) {
    int32_t step = degree_to_step(angle);
    stepper_x.add_position(step, speed, acc, true);
}

void PTZ::add_y_angle(float angle, uint16_t speed, uint8_t acc) {
    int32_t step = degree_to_step(angle);
    stepper_y.add_position(step, speed, acc, true);
}

float PTZ::get_x_angle() {
    return stepper_x.get_angle();
}

float PTZ::get_y_angle() {
    return stepper_y.get_angle();
}

void PTZ::sync_all() {
    Stepper::sync_all(PTZ::serial);
}

void PTZ::reset() {
    stepper_x.reset();
    stepper_y.reset();
}

float PTZ::smooth_speed(float current, float last) {
    const float alpha = 0.8f; // 平滑系数 (0.7-0.95)
    return alpha * last + (1 - alpha) * current;
}

void PTZ::move_to(float dis, float x, float y, uint16_t speed, uint8_t acc) {
    // 计算目标角度
    float target_x_angle = atan2(x, dis) * 180.0 / PI;
    float target_y_angle = atan2(y, sqrt(x * x + dis * dis)) * 180.0 / PI;
    
    // 计算角度变化量
    float delta_x = target_x_angle - last_x_angle;
    float delta_y = target_y_angle - last_y_angle;
    
    // 计算动态速度比例 (保持x/y速度比恒定)
    float dist = sqrt(delta_x * delta_x + delta_y * delta_y);
    float speed_ratio_x = (dist > 0.01f) ? fabs(delta_x / dist) : 0;
    float speed_ratio_y = (dist > 0.01f) ? fabs(delta_y / dist) : 0;
    
    // 应用速度平滑
    static float last_speed_x = 0, last_speed_y = 0;
    float current_speed_x = speed * speed_ratio_x * (delta_x > 0 ? 1 : -1);
    float current_speed_y = speed * speed_ratio_y * (delta_y > 0 ? 1 : -1);
    
    float smooth_speed_x = smooth_speed(current_speed_x, last_speed_x);
    float smooth_speed_y = smooth_speed(current_speed_y, last_speed_y);
    
    // 更新记录的速度值
    last_speed_x = smooth_speed_x;
    last_speed_y = smooth_speed_y;
    
    // 设置电机角度
    set_x_angle(target_x_angle, smooth_speed_x, acc);
    set_y_angle(target_y_angle, smooth_speed_y, acc);
    sync_all();
    
    // 更新记录的角度
    last_x_angle = target_x_angle;
    last_y_angle = target_y_angle;
}

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
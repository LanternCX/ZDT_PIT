#include "PTZ.hpp"

PTZ::PTZ(HardwareSerial& _serial, int _addr_x, int _addr_y)
    : serial(_serial), addr_x(_addr_x), addr_y(_addr_y),
      stepper_x(_serial, _addr_x), stepper_y(_serial, _addr_y) {}

void PTZ::init() {
    stepper_x.init();
    stepper_y.init();
}

void PTZ::set_x_speed(int16_t speed, uint8_t acc) {
    stepper_x.set_speed(speed, acc);
}
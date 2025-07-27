#pragma once

#include <Arduino.h>
#include "Stepper.hpp"

class PTZ {
    public:
        PTZ(HardwareSerial& _serial, int _addr_x, int _addr_y);
        void init();
        void set_x_speed(int16_t speed, uint8_t acc);
        void set_y_speed();
    private:
        HardwareSerial& serial;
        int addr_x;
        int addr_y;
        Stepper stepper_x;
        Stepper stepper_y;
};
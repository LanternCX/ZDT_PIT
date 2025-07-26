#pragma once
#include <Arduino.h>

class Motor {
    public:
        Motor(HardwareSerial& _serial, int _addr);
        /**
         * @brief init motor
         */
        virtual void init();

        /**
         * @brief enable motor
         */
        virtual void enable();

        /**
         * @brief disable motor
         */
        virtual void disable();

        /**
         * @brief set motor speed
         */
        virtual void set_speed(int16_t speed, uint8_t acc);

        /**
         * @brief set motor pos
         */
        // virtual void set_pos(int pos);
        
        /**
         * @brief set motor zero
         */
        virtual void set_zero();

        /**
         * @brief stop motor
         */
        virtual void stop();

        /**
         * @brief at exit
         */
        virtual void exit();
    private:
        HardwareSerial& serial;
        int addr;
};
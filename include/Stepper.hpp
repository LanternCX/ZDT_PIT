#pragma once

#include <Arduino.h>

/**
 * @brief 步进电机控制器类（用于 Emm_V5.0 闭环驱动器）
 */
class Stepper {
    public:
        /**
         * @brief 构造函数
         * @param serial 串口引用（如 Serial1）
         * @param addr 电机地址（1~15）
         */
        Stepper(HardwareSerial& serial, int addr);

        /**
         * @brief 电机初始化：使能 -> 停止 -> 设零
         */
        void init();

        /**
         * @brief 启用电机
         */
        void enable();

        /**
         * @brief 禁用电机
         */
        void disable();

        /**
         * @brief 停止电机运行
         * @param sync 是否等待同步启动（默认 false）
         */
        void stop(bool sync = 0);
        
        /**
         * @brief 设置当前位置为零点
         * @param is_store 是否存储（默认 false）
         */
        void set_zero(bool is_store = 0);

        /**
         * @brief 将当前位置清零
         */
        void to_zero();

        /**
         * @brief 速度模式控制
         * @param speed 转速（单位 rpm，正为 CCW，负为 CW，范围 ±5000）
         * @param acc 加速度（0~255），默认 0 即立即到达
         * @param sync 是否等待同步启动（默认 false）
         */
        void set_speed(int16_t speed, uint8_t acc = 0, bool sync = false);

        /**
         * @brief 设置绝对位置
         * @param step 脉冲数（单位：step，正为 CCW，负为 CW，一圈 3200 步）
         * @param speed 转速（单位 rpm，，范围 ±5000）
         * @param acc 加速度（0~255），0 表示直接启动
         * @param is_sync 是否等待同步启动（默认 false）
         */
        void set_position(uint32_t step, int16_t speed = 30, uint8_t acc = 0, bool is_sync = false);
        
        /**
         * @brief 移动相对位置
         * @param step 脉冲数（单位：step）
         * @param speed 转速（单位 rpm，正为 CCW，负为 CW，范围 ±5000）
         * @param acc 加速度（0~255），0 表示直接启动
         * @param is_sync 是否等待同步启动（默认 false）
         */
        void add_position(int32_t step, uint16_t speed = 30, uint8_t acc = 0, bool is_sync = false);

        /**
         * @brief 电机状态清零
         */
        void reset();

        /**
         * @brief 同步启动所有等待同步的电机
         * @param serial 使用的串口
         */
        static void sync_all(HardwareSerial& serial);

    private:
        HardwareSerial& serial_;
        int addr_;

        /**
         * @brief 发送控制指令到驱动器
         * @param cmd 指令码
         * @param data 数据数组（最多 4 字节）
         * @param len 数据长度
         */
        void send_command(uint8_t cmd, const uint8_t* data, uint8_t len);
};


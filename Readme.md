# ZDT-PTZ

基于张大头步进电机的二维云台实现与串口协议二次封装

## 快速开始

1. **环境准备**

   ESP32 + VSCode + PlatformIO

2. **克隆仓库**

   ```
   git clone https://github.com/LanternCX/ZDT-PTZ.git
   cd ZDT-PTZ
   ```

在 `PIO` 中打开仓库并烧入代码就可以通过[串口命令](#串口命令)进行调试

## PTZ 类

二维云台封装

### 快速开始

#### 引入头文件

```cpp
#include <Arduino.h>
#include "PTZ.hpp"
```

#### 初始化

```cpp
// 使用 Serial1 作为云台通信串口，电机地址分别为 1 (水平) 和 2 (垂直)
PTZ ptz(Serial1, 1, 2);
```

#### 在 setup() 中初始化

```cpp
void setup() {
  Serial.begin(115200);
  // 初始化云台串口
  Serial1.begin(115200, SERIAL_8N1, 25, 26);

  // 延时等待硬件就绪
  delay(2000);

  // 初始化云台
  ptz.init();
}
```

## 主要接口

详细的使用请参考 `PTZ.hpp` 中的接口注释

| 方法                             | 说明                                                         |
| -------------------------------- | ------------------------------------------------------------ |
| `init()`                         | 电机初始化                                                   |
| `set_x_speed(speed, acc)`        | 设置水平轴速度，`speed` 单位 rpm (±5000)，`acc` 加速度 (0~255) |
| `set_y_speed(speed, acc)`        | 设置垂直轴速度，同上                                         |
| `set_x_angle(angle, speed, acc)` | 设置水平轴绝对角度，`angle` 单位 °，默认速度 30 rpm          |
| `set_y_angle(angle, speed, acc)` | 设置垂直轴绝对角度，同上                                     |
| `add_x_angle(angle, speed, acc)` | 在当前位置基础上增加水平角度                                 |
| `add_y_angle(angle, speed, acc)` | 在当前位置基础上增加垂直角度                                 |
| `get_x_angle()`                  | 获取当前水平轴角度                                           |
| `get_y_angle()`                  | 获取当前垂直轴角度                                           |
| `reset()`                        | 重置状态                                                     |
| `sync_all()`                     | 同步启动所有等待同步的命令                                   |

------

## Stepper 类

### 快速开始

#### 引入头文件

```cpp
#include <Arduino.h>
#include "Stepper.hpp"
```

#### 初始化

```cpp
// 示例：使用 Serial1，地址为 1
Stepper stepper(Serial1, 1);
```

#### 在 setup() 中初始化

```cpp
void setup() {
    // 串口初始化
    Serial1.begin(115200, SERIAL_8N1, 25, 26);
    // 电机初始化
    stepper.init();
}
```

### 主要接口

详细的使用请参考 `Stepper.hpp` 中的接口注释

| 方法                                      | 说明                                                  |
| ----------------------------------------- | ----------------------------------------------------- |
| `init()`                                  | 电机初始化                                            |
| `enable()`                                | 使能电机                                              |
| `disable()`                               | 禁用电机                                              |
| `stop(sync=false)`                        | 停止运动，可选择是否等待同步启动                      |
| `set_zero(is_store=false)`                | 将当前位置标记为零点，可选择是否存储                  |
| `to_zero()`                               | 清零（快速回零，不存储）                              |
| `set_speed(speed, acc=0, sync=false)`     | 速度模式控制，`speed` rpm (±5000)，`acc` 加速 (0~255) |
| `set_position(step, speed, acc, is_sync)` | 绝对位置控制，`step` 脉冲数 (±3200 步/圈)，其他同上   |
| `add_position(step, speed, acc, is_sync)` | 相对位置控制                                          |
| `reset()`                                 | 重置驱动器状态                                        |
| `get_angle()`                             | 获取当前角度，单位 °                                  |
| `static sync_all(serial)`                 | 同步启动所有等待同步的命令，传入串口引用              |

------

## 串口命令

在 `Main.cpp` 中定义了一些串口命令，可以当作是示例代码

主控接收到串口指令后，通过 `serial_on_msg()` 解析并调用 PTZ 接口。以下是支持的命令：

| 命令前缀  | 参数格式         | 描述                                        | 示例             |
| --------- | ---------------- | ------------------------------------------- | ---------------- |
| `STOP`    | 无               | 停止所有运动并清零                          | `STOP`           |
| `RESET`   | 无               | 停止运动，可选清零（enable 时）             | `RESET`          |
| `RUN`     | 无               | 恢复运行，接收命令生效                      | `RUN`            |
| `SPEED`   | `x y`            | 同步设置水平 (x) 与垂直 (y) 速度 ([-10,10]) | `SPEED 5 -5`     |
| `DEG_SET` | `x y` (整数角度) | 设置水平 (x°) 与垂直 (y°) 绝对角度          | `DEG_SET 90 30`  |
| `DEG_ADD` | `x y` (整数角度) | 在当前角度基础上增加水平 (x°) 与垂直 (y°)   | `DEG_ADD -15 10` |

**注意事项**：

- 字符串以 `\r\n` 结尾都可以
- 参数范围会在代码中通过 `max/min` 限制。
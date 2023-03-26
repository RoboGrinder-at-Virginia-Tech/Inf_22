## 概述
```
├── application
│   └── protocol
├── bsp
│   └── boards
├── components
│   ├── algorithm
│   │   └── Include
│   ├── controller
│   ├── devices
│   └── support
├── doc
│   ├── en
│   └── image
├── Drivers
│   ├── CMSIS
│   │   ├── Device
│   │   │   └── ST
│   │   │       └── STM32F4xx
│   │   │           └── Include
│   │   └── Include
│   └── STM32F4xx_HAL_Driver
│       ├── Inc
│       │   └── Legacy
│       └── Src
├── Inc
├── Jflash_test
├── MDK-ARM
│   ├── DebugConfig
│   ├── RTE
│   │   └── _standard_tpye_c
│   └── standard_tpye_c
├── Middlewares
│   ├── ST
│   │   └── STM32_USB_Device_Library
│   │       ├── Class
│   │       │   └── CDC
│   │       │       ├── Inc
│   │       │       └── Src
│   │       └── Core
│   │           ├── Inc
│   │           └── Src
│   └── Third_Party
│       └── FreeRTOS
│           └── Source
│               ├── CMSIS_RTOS
│               ├── include
│               └── portable
│                   ├── MemMang
│                   └── RVDS
│                       └── ARM_CM4F
├── Src
└── User_Debug

```
- en doc   [readme](doc/en/readme.md)

### 软件环境

 - Toolchain/IDE : MDK-ARM V5
 - STM32F4xx_DFP Packs:2.13.0
 - STM32CubeMx:5.2.1
 - package version: STM32Cube FW_F4 V1.21.1
 - FreeRTOS version: 10.0.1
 - CMSIS-RTOS version: 1.02

### 编程规范

- 变量和函数命名方式遵循 Unix/Linux 风格
- 不需要精确计时的任务，采用自行实现的软件定时器实现，定时精度受任务调度影响

### 注意事项

1.由于发射机构触动开关阻力非常小，轻微转动拨弹电机就可能造成子弹越过触动开关触碰摩擦轮，导致摩擦轮摩擦过大无法启动，**造成摩擦轮磨损和电机启动失败**。因此，开电前务必检查是否有子弹已经越过触动开关。**取出办法**：用手按住一侧摩擦轮，旋转另一个摩擦轮。

2.在发送控制逻辑中，考虑到有清理子弹的功能，在左拨杆处于下方超过3s,进入连发模式，故而如果使用单发，请及时将左拨杆回中。

3.使用键鼠操作时，请将左拨杆回中。

### 陀螺仪校准方法

底层代码集成了陀螺仪校准零漂功能，原理是需要温度升至环境温度10℃后，采集20s静止的陀螺仪输出，作为陀螺仪零漂.并且这个时候遥控器的DBUS会关闭。
触发条件：

1.开发板首次刷入程序或者参数区被清空

2.遥控器在左右拨杆开关处于下方，先摇杆下内八字保持2s,在摇杆下外八字保持2s.

注意：校准时务必保持云台静止，周围无振动干扰源。

### 云台校准方法

底层代码集成了云台自动校准功能，原理是先利用imu校准pitch，左右限位校准yaw。首先抬起pitch，再放下pitch，之后逆时针旋转yaw，最后顺时针旋转yaw.
触发条件：

1.开发板首次刷入程序或者参数区被清空

2.遥控器在左右拨杆开关处于下方，先摇杆下内八字保持2s,在摇杆上外八字保持2s.

注意：校准时务必将底盘放在水平地面；校准中可能出现pitch始终无法到达指定误差范围内，此时是因为云台配重问题导致，可以用手向上掰一下然后松开云台，加速调节过程。



### 底盘校准方法

底层代码集成了底盘电机进入快速设置ID功能，原理是发送ID为0x700的CAN包给底盘电机，这时候C620电调呈现黄灯，之后需要手动将车轮依次旋转。顺序为右前，左前，左后，右后.
触发条件：

1.遥控器在左右拨杆开关处于下方，先摇杆下内八字保持2s,在摇杆上内八字保持2s.//这里写错了

注意：请务必按照顺序旋转车轮。



### 模块离线说明
#### 1.OLED 显示

当车辆的某个模块离线时，可以外接0.96寸OLED显示屏幕，驱动为SDD1306即可，单色与双色OLED需要在OLED.c中改变一个宏定义 OLED_ONE_COLOR 或者 OLED_TWO_COLOR。

OLED屏幕在程序开始1s中显示RM LOGO

![](doc/image/RoboMaster_logo.png)

之后显示错误信息


![](doc/image/error_show.png)


M1-M4代表4个底盘电机,M5代表Yaw电机,M6代表pitch电机,M7代表拨弹电机,DBUS代表遥控器,GYR代表陀螺仪模块,ACC代表加速度,MAG代表磁力计,REF代表裁判系统

#### 2.蜂鸣器提醒

蜂鸣器鸣叫次数按照离线模块的优先级进行错误指示，例如云台电机优先级高于拨弹电机，如果同时发生离线，先指示当前离线设备是云台电机
模块离线对应的状态如下，数字对应蜂鸣器每次鸣叫的次数，按照优先级排序：

![](doc/image/buzzer_warning.png)

#### 3.USB串口输出信息

开发板USB连接电脑，在电脑上打开串口工具接收信息，如图所示

![](doc/image/usb_warning.png)



### 文档

- 使用文档  [2020年外赠步兵机器人使用手册.pdf](doc/cn/2020年外赠步兵机器人使用手册.pdf)
- 开发文档 [2020年外赠步兵机器人开发手册.pdf](doc/cn/2020年外赠步兵机器人开发手册.pdf)

## 快速开始

### 硬件接口

主控板使用 RM 开发板C型，各个功能接口的位置如下：

**硬件接口**

![](doc/image/handware.png)

**云台开发板接口**

![](doc/image/board_handware.png)


### 功能模块

#### 手动模式：

提供遥控器基础控制。

#### 键鼠模式：

提供键盘基础控制。

#### 操作档位说明：

##### 手动档

遥控器控制（底盘跟随云台）：右拨杆上
遥控器控制（底盘不跟随云台）：右拨杆中

- 开、关摩擦轮（左拨杆上拨）
- 单发、连发射击（左拨杆下拨）

## 程序说明

### 程序体系结构
![](doc/image/software_system.png)

#### 体系框架

1. 使用免费及开源的 freertos 操作系统，兼容其他开源协议 license；




### 软件体系


### 硬件体系

1. 主控 MCU：STM32F407IGHx，配置运行频率168MHz
2. 模块通信方式：CAN；CAN设备：电机电调
3. 麦轮安装方式：X型


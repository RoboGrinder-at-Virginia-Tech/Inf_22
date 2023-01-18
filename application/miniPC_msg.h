#ifndef __MINIPC_MSG__
#define __MINIPC_MSG__

#include "main.h"
#include "struct_typedef.h"

/*---------------------------------------------------- Raw Data Msg ----------------------------------------------------*/
//robot_id_t is defined in referee.h file
/*
typedef enum
{
    RED_HERO        = 1,
    RED_ENGINEER    = 2,
    RED_STANDARD_1  = 3,
    RED_STANDARD_2  = 4,
    RED_STANDARD_3  = 5,
    RED_AERIAL      = 6,
    RED_SENTRY      = 7,
		RED_DART				= 8,
		RED_RADAR 			= 9,
    BLUE_HERO       = 101,
    BLUE_ENGINEER   = 102,
    BLUE_STANDARD_1 = 103,
    BLUE_STANDARD_2 = 104,
    BLUE_STANDARD_3 = 105,
    BLUE_AERIAL     = 106,
    BLUE_SENTRY     = 107,
		BLUE_DART				= 108,
		BLUE_RADAR 			= 109,
} robot_id_t;
*/

/*
UI related information - miniPC->Embeded
*/
typedef struct
{
    // = m * 100 = cm
    int16_t detected_enemy_distance; //=0 when not detected
    int16_t aim_pos_distance;

 
}pc_ui_msg_t;

/*
miniPC to control the robot's chassis - miniPC->Embeded
*/
typedef enum
{
   PC_CHASSIS_NO_FOLLOW_YAW, //uint8_t 0
   PC_CHASSIS_FOLLOW_GIMBAL_YAW, //uint8_t 1
   PC_CHASSIS_SPIN, //uint8_t 2
} pc_chassis_mode_e;

typedef struct
{
  int16_t vx_mm; // forward/back
//: m/s * 1000 <-->mm/s 

  int16_t vy_mm; // left/right
  int16_t vw_mm; // ccw positive
//vw_mm: radian/s * 1000

 pc_chassis_mode_e chassis_mode;
}pc_cmd_chassis_control_t;

/*
chassis related info. Send to PC
Embedded -> miniPC
*/
typedef struct
{
  int16_t vx_mm; // forward/back
//: m/s * 1000 <-->mm/s 

  int16_t vy_mm; // left/right
  int16_t vw_mm; // ccw positive
//vw_mm: radian/s * 1000

 uint8_t energy_buff_pct; //zyz说要的底盘能量, 可能来自超级电容剩余能量或裁判系统缓冲能量
//superCap or equivalent energy percentage
}embed_chassis_info_t;

/*
Gimbal related control - miniPC->Embedded
*/
typedef struct
{
    int16_t yaw; //=rad*10000
    int16_t pitch;
    uint8_t is_detect;
    uint8_t shoot;
}pc_cmd_gimbal_ctrl_t;

//cmd_gimbal_control_aid
//cmd_gimbal_control_full_ctrl

/*
Gimbal information - Embedded -> miniPC
*/
typedef struct
{
    int16_t yaw_relative_angle; //= rad * 10000
    int16_t pitch_relative_angle;

    /*quaternion: uint16_t quat[i]: [0],[1],[2],[3]
       In embeded, quat has range:(-1, +1), 
       ->transform: +1->range:(0, +2)
       ->transform: * 10000->quat[i] with 16 bits uint16_t */
    uint16_t quat[4];

    /*
    原始数据: x m/s,
     (x*10)后发给PC, 即通信单位为 dm/s
    */
    // = m/s * 10 = dm/s
    uint16_t shoot_bullet_speed; //anticipated speed

/*
decimal val = hex val
0=0x00                
1=0x01                RED_HERO
3=0x03                RED_STANDARD_1
4=0x04                RED_STANDARD_2
5=0x05                RED_STANDARD_3
6=0x06                RED_AERIAL
7=0x07                RED_SENTRY
                
101=0x65                BLUE_HERO
103=0x67                BLUE_STANDARD_1
104=0x68                BLUE_STANDARD_2
105=0x69                BLUE_STANDARD_3
106=0x6A                BLUE_AERIAL
107=0x6B                BLUE_SENTRY
*/
    uint8_t robot_id;
}embed_gimbal_info_t;
/*---------------------------------------------------- Raw Data Msg - End Above ----------------------------------------------------*/

/*---------------------------------------------------- Processed Data ----------------------------------------------------*/
//uint8_t  autoAimFlag = 1; //自动瞄准开关状态 0关 1自动瞄准
//uint8_t shootCommand = 0x00;//自动开火指令  0x00 = 停火  0xff = 开火
//uint8_t fricCommand = 0x01;// 摩擦轮转速指令  0x01 =低转  0x02 = 高转
//fp32 yawMove = 0;  //云台数据
//fp32 pitchMove = 0;  //云台数据
//uint32_t timeFlag = 0; //
typedef enum
{
	pc_offline,
	miniPC_online
}pc_connection_status_e;//这两个结构体 不同的超级电容共用
typedef struct
{
	//MiniPC 那边发过来的信息
	uint8_t cmd_id;
	uint8_t cv_status; //0自瞄关闭, 1AID, 2LOCK
	int16_t yawCommand; //delta yaw
	int16_t pitchCommand; //delta pitch
	uint16_t dis_raw;
	uint8_t shootCommand;//自动开火指令  0x00 = 停火  0xff = 开火
	
	
	//其它FSM 以及 数据
	fp32 dis; //单位转换之后的距离
	/*如果距离是0 就说明没有识别到目标*/
	uint8_t autoAimFlag; //自动瞄准开关状态 0关 1自动瞄准AID 2自动瞄准LOCK
	
	uint8_t enemy_detected;//0 未识别到, 1识别到了
	
	fp32 yawMove_aid;
	fp32 pitchMove_aid;
	
	fp32 yawMove_absolute;
	fp32 pitchMove_absolute;
	
	pc_connection_status_e miniPC_connection_status;
}pc_info_t;
/*---------------------------------------------------- Processed Data - End Above ----------------------------------------------------*/

extern void init_miniPC_comm_struct_data(void);

#endif

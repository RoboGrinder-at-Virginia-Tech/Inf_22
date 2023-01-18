/**
  ****************************(C) COPYRIGHT 2023 RoboGrinder at Virginia Tech****************************
  * @file       miniPC_msg.c/h
  * @brief      miniPC_msg.  miniPC message related files
  * @note       message information about miniPC <-> TypeC board communication(Tx and Rx)
  *           
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-16-2023     Zelin Shen      basic comminication functions
	*
  *
  @verbatim
  ==============================================================================
	This .c and .h files include specific raw data package & data after process if needed
	File dependency: this file is used with miniPC_comm_task, fifo data structure file, 
	RM CRC8_CRC16 files, lower level USART drivers
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2023 RoboGrinder at Virginia Tech****************************
  */
	
#include "miniPC_msg.h"
#include "miniPC_comm_task.h"
#include "string.h"
#include "stdio.h"
#include "CRC8_CRC16.h"

//包头实体, frame head
pc_comm_frame_header_t pc_receive_header;
pc_comm_frame_header_t pc_send_header;

//消息实体 原始数据, raw origial data
//miniPC -> Embedded
pc_ui_msg_t pc_ui_msg;
pc_cmd_chassis_control_t pc_cmd_chassis_control;
pc_cmd_gimbal_ctrl_t pc_cmd_gimbal_ctrl_aid;
pc_cmd_gimbal_ctrl_t pc_cmd_gimbal_ctrl_ctrl;

//Embedded -> miniPC
embed_chassis_info_t embed_chassis_info;
embed_gimbal_info_t embed_gimbal_info;

void init_miniPC_comm_struct_data(void)
{
	memset(&pc_receive_header, 0, sizeof(pc_comm_frame_header_t));
	memset(&pc_send_header, 0, sizeof(pc_comm_frame_header_t));
	
	memset(&pc_ui_msg, 0, sizeof(pc_ui_msg_t));
	memset(&pc_cmd_chassis_control, 0, sizeof(pc_cmd_chassis_control_t));
	memset(&pc_cmd_gimbal_ctrl_aid, 0, sizeof(pc_cmd_gimbal_ctrl_t));
	memset(&pc_cmd_gimbal_ctrl_ctrl, 0, sizeof(pc_cmd_gimbal_ctrl_t));
	
	memset(&embed_chassis_info, 0, sizeof(embed_chassis_info_t));
	memset(&embed_gimbal_info, 0, sizeof(embed_gimbal_info_t));
	
}



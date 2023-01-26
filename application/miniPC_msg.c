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
#include "detect_task.h"
#include "SuperCap_comm.h"
#include "referee.h"

extern pc_comm_unpack_data_t pc_comm_unpack_data_obj;

//包头实体, frame head
pc_comm_frame_header_t pc_comm_receive_header;
pc_comm_frame_header_t pc_send_header;

//消息实体 原始数据, raw origial data
//miniPC -> Embedded
pc_ui_msg_t pc_ui_msg;
pc_cmd_chassis_control_t pc_cmd_chassis_control;
pc_cmd_gimbal_ctrl_t pc_cmd_gimbal_ctrl_aid;
pc_cmd_gimbal_ctrl_t pc_cmd_gimbal_ctrl_full;
//processed cmd
pc_info_t pc_info; //msg info from pc

//origial information to pc
struct embed_msg_to_pc_t embed_msg_to_pc;

//Embedded -> miniPC
embed_chassis_info_t embed_chassis_info;
embed_gimbal_info_t embed_gimbal_info;

void init_miniPC_comm_struct_data(void)
{
	memset(&pc_comm_receive_header, 0, sizeof(pc_comm_frame_header_t));
	memset(&pc_send_header, 0, sizeof(pc_comm_frame_header_t));
	
	memset(&pc_ui_msg, 0, sizeof(pc_ui_msg_t));
	memset(&pc_cmd_chassis_control, 0, sizeof(pc_cmd_chassis_control_t));
	memset(&pc_cmd_gimbal_ctrl_aid, 0, sizeof(pc_cmd_gimbal_ctrl_t));
	memset(&pc_cmd_gimbal_ctrl_full, 0, sizeof(pc_cmd_gimbal_ctrl_t));
	
	memset(&embed_chassis_info, 0, sizeof(embed_chassis_info_t));
	memset(&embed_gimbal_info, 0, sizeof(embed_gimbal_info_t));
	
	//init information pckg
	embed_msg_to_pc.chassis_move_ptr = get_chassis_pointer();
	embed_msg_to_pc.gimbal_control_ptr = get_gimbal_pointer();
	embed_msg_to_pc.quat_ptr = get_INS_gimbal_quat();
	embed_msg_to_pc.shoot_control_ptr = get_robot_shoot_control();
}

void cmd_process_pc_cmd_chassis_control(void)
{
	pc_info.vx_m = (fp32)pc_cmd_chassis_control.vx_mm / 1000.0f;
	pc_info.vy_m = (fp32)pc_cmd_chassis_control.vy_mm / 1000.0f;
	pc_info.vw_m = (fp32)pc_cmd_chassis_control.vw_mm / 1000.0f;
	
	if(pc_cmd_chassis_control.chassis_mode == 0)
	{
		pc_info.chassis_mode = PC_CHASSIS_NO_FOLLOW_YAW;
	}
	else if(pc_cmd_chassis_control.chassis_mode == 1)
	{
		pc_info.chassis_mode = PC_CHASSIS_FOLLOW_GIMBAL_YAW;
	}
	else if(pc_cmd_chassis_control.chassis_mode == 2)
	{
		pc_info.chassis_mode = PC_CHASSIS_SPIN;
	}
	else
	{
		pc_info.chassis_mode = PC_CHASSIS_NO_FOLLOW_YAW;
	}
	
}

void cmd_process_pc_cmd_gimbal_ctrl_aid(void) //TODO添加数据合理性判断
{
	pc_info.yawMove_aid = 0.003f * (fp32)pc_cmd_gimbal_ctrl_aid.yaw / 10000.0f;
	pc_info.pitchMove_aid = 0.008f * (fp32)pc_cmd_gimbal_ctrl_aid.pitch / 10000.0f;
	pc_info.enemy_detected = pc_cmd_gimbal_ctrl_aid.is_detect;
	pc_info.shootCommand = pc_cmd_gimbal_ctrl_aid.shoot;

	pc_info.cv_gimbal_sts = 1; //aim mode FSM
	
	//Need to handle the erase of miniPC_info.yawMove_absolute and pitchMove?
}

void cmd_process_pc_cmd_gimbal_ctrl_full(void) //TODO添加数据合理性判断
{
	pc_info.yawMove_absolute = (fp32)pc_cmd_gimbal_ctrl_aid.yaw / 10000.0f;
	pc_info.pitchMove_absolute = (fp32)pc_cmd_gimbal_ctrl_aid.pitch / 10000.0f;
	pc_info.enemy_detected = pc_cmd_gimbal_ctrl_aid.is_detect;
	pc_info.shootCommand = pc_cmd_gimbal_ctrl_aid.shoot;

	pc_info.cv_gimbal_sts = 2; //aim mode FSM
	
	//Erase the other
	pc_info.yawMove_aid = 0.0f;
	pc_info.pitchMove_aid = 0.0f;
}

void cmd_process_pc_ui_msg(void) //TODO添加数据合理性判断
{
	pc_info.dis_raw = pc_ui_msg.detected_enemy_distance;
	
	pc_info.dis = (fp32)pc_ui_msg.detected_enemy_distance / 100.0f;
	pc_info.aim_pos_dis = (fp32)pc_ui_msg.aim_pos_distance / 100.0f;
}

void pc_comm_data_solve(uint8_t *frame)
{
		//detect_hook(PC_TOE); //for target of evaluation
	
    uint16_t cmd_id = 0;

    uint8_t index = 0;

    memcpy(&pc_comm_receive_header, frame, sizeof(pc_comm_frame_header_t));

    index += sizeof(pc_comm_frame_header_t);

    memcpy(&cmd_id, frame + index, sizeof(uint16_t));
    index += sizeof(uint16_t);
	
		//for cmdid debug
	  pc_comm_unpack_data_obj.cmd_id_pc_comm_data_solve_debug = cmd_id;

    switch (cmd_id)
    {
        case CHASSIS_REL_CTRL_CMD_ID:
        {
            memcpy(&pc_cmd_chassis_control, frame + index, sizeof(pc_cmd_chassis_control_t));
						cmd_process_pc_cmd_chassis_control();
        }
        break;
        case GIMBAL_REL_AID_CTRL_CMD_ID:
        {
            memcpy(&pc_cmd_gimbal_ctrl_aid, frame + index, sizeof(pc_cmd_gimbal_ctrl_t));
					  cmd_process_pc_cmd_gimbal_ctrl_aid();
        }
        break;
        case GIMBAL_REL_FULL_CTRL_CMD_ID:
        {
            memcpy(&pc_cmd_gimbal_ctrl_full, frame + index, sizeof(pc_cmd_gimbal_ctrl_t));
						cmd_process_pc_cmd_gimbal_ctrl_full();
        }
        break;

        case UI_REL_MSG_CMD_ID:
        {
            memcpy(&pc_ui_msg, frame + index, sizeof(pc_ui_msg_t));
				  	cmd_process_pc_ui_msg();
        }
        break;
       
        default:
        {
            break;
        }
    }
}

void pc_offline_proc()
{
		pc_info.pc_connection_status = PC_OFFLINE;
	  pc_info.cv_gimbal_sts = 0;
}

bool_t pc_is_data_error_proc()
{
		pc_info.pc_connection_status = PC_ONLINE;
		return 0;
}

/* -------------------------------- USART SEND DATA FILL-------------------------------- */
void pc_send_header_msg_data_fill(uint8_t frame_length)
{
	pc_send_header.SOF = PC_HEADER_SOF;
	pc_send_header.frame_length = frame_length;
	pc_send_header.seq = 1;
	//pc_send_header.seq++;
	pc_send_header.CRC8 = get_CRC8_check_sum(&(pc_send_header.SOF), 4, 0xFF);
}

void embed_all_info_update_from_sensor()
{
	/*
	fp32 s_vx_m; // m/s
	fp32 s_vy_m; // m/s
	fp32 s_vw_m; // radian/s
	
	uint8_t energy_buff_pct; //get_superCap_charge_pwr
	
	fp32 yaw_relative_angle; //= rad
  fp32 pitch_relative_angle;

	fp32 quat[4];

  fp32 shoot_bullet_speed; // = m/s

  uint8_t robot_id;
	*/
	embed_msg_to_pc.s_vx_m = embed_msg_to_pc.chassis_move_ptr->vx;
	embed_msg_to_pc.s_vy_m = embed_msg_to_pc.chassis_move_ptr->vy;
	embed_msg_to_pc.s_vw_m = embed_msg_to_pc.chassis_move_ptr->wz;
	embed_msg_to_pc.energy_buff_pct = (uint8_t) get_current_cap_pct();
	embed_msg_to_pc.yaw_relative_angle = embed_msg_to_pc.gimbal_control_ptr->gimbal_yaw_motor.relative_angle;
	embed_msg_to_pc.pitch_relative_angle = embed_msg_to_pc.gimbal_control_ptr->gimbal_pitch_motor.relative_angle;
	
	embed_msg_to_pc.quat[0] = embed_msg_to_pc.quat_ptr[0];
	embed_msg_to_pc.quat[1] = embed_msg_to_pc.quat_ptr[1];
	embed_msg_to_pc.quat[2] = embed_msg_to_pc.quat_ptr[2];
	embed_msg_to_pc.quat[3] = embed_msg_to_pc.quat_ptr[3];
	
	// = (uint16_t)(shoot_control.predict_shoot_speed*10); //anticipated predicated bullet speed
	embed_msg_to_pc.shoot_bullet_speed = embed_msg_to_pc.shoot_control_ptr->predict_shoot_speed;
	embed_msg_to_pc.robot_id = RED_STANDARD_1;
	
	
	
}

void embed_chassis_info_msg_data_update()
{
//	embed_chassis_info.vx_mm = 
//	embed_chassis_info
//	embed_chassis_info
//	embed_chassis_info
//	embed_chassis_info
//	embed_chassis_info
//	embed_chassis_info
//	embed_chassis_info
//	embed_chassis_info
}

void embed_gimbal_info_msg_data_update()
{
}

/* -------------------------------- USART SEND DATA FILL END-------------------------------- */

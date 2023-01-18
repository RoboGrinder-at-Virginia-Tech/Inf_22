#ifndef __MINIPC_COMM_TASK__
#define __MINIPC_COMM_TASK__

#include "main.h"
#include "struct_typedef.h"

/*
Flow of data: software fifo -> DMA buff -> unpack array-ram buffer
1-17-2023: current data size did not exceed 50 bytes
*/

//this value is used for fifo - software fifo buffer
#define MINIPC_COMM_RX_FIFO_BUF_LENGTH 128 //1024

//This is the DMA buff length
#define MINIPC_COMM_UART_DMA_RX_BUF_LENGHT 50 //512

#define PC_HEADER_SOF 0xAF
//size for the protocal unpack array - ram buffer
#define PC_PROTOCOL_FRAME_MAX_SIZE  50 //128

#pragma pack(push, 1)

typedef enum
{
		//Embeded-->PC
    CHASSIS_INFO_CMD_ID = 0x0204,
    GIMBAL_INFO_CMD_ID = 0x0304,
	
		//PC-->Embeded
    CHASSIS_REL_CTRL_CMD_ID = 0x1206,
    GIMBAL_REL_AID_CTRL_CMD_ID = 0x130A,
    GIMBAL_REL_FULL_CTRL_CMD_ID = 0x130B,
    UI_REL_MSG_CMD_ID = 0x1001,
}pc_cmd_id_t;

typedef  struct
{
  uint8_t SOF;
  uint8_t frame_length; //entire frame length
  uint16_t cmd_id; 
} pc_comm_frame_header_t;

typedef enum
{
  PC_COMM_STEP_HEADER_SOF  = 0,
  PC_COMM_STEP_FRAME_LENGTH  = 1,
  PC_COMM_STEP_CMDID   = 2,
  //PC_COMM_STEP_HEADER_CRC8 = 4,
  PC_COMM_STEP_END_CRC16  = 3,
} pc_comm_unpack_step_e;

typedef struct
{
  pc_comm_frame_header_t *p_header;
  uint8_t       data_len;
  uint8_t        protocol_packet[PC_PROTOCOL_FRAME_MAX_SIZE];
  pc_comm_unpack_step_e  unpack_step;
  uint8_t       index;
} pc_comm_unpack_data_t;

#pragma pack(pop)

#endif
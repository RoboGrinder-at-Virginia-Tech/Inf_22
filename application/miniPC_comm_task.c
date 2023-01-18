/**
  ****************************(C) COPYRIGHT 2023 RoboGrinder at Virginia Tech****************************
  * @file       miniPC_comm_task.c/h
  * @brief      miniPC_comm_task.  miniPC communication task and functions
  * @note       The communication task that handles the communication between miniPC <-> TypeC board
	*							both Tx and Rx. 
  *           
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Jan-16-2023     Zelin Shen      basic comminication functions
	*
  *
  @verbatim
  ==============================================================================
  Communication between miniPC <-> TypeC board; both Tx and Rx. 
	This task outlines the overall structural logic for communication. 
	General frame structure and unpack mechanism is also defined in this file.
	Specific datas, msg are defined and handled in other files.
	File dependency: this file is used with miniPC_msg, fifo data structure file, 
	RM CRC8_CRC16 files, lower level USART drivers
  ==============================================================================
  @endverbatim
  ****************************(C) COPYRIGHT 2023 RoboGrinder at Virginia Tech****************************
  */

#include "miniPC_comm_task.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include "bsp_usart.h"
#include "detect_task.h"
#include "CRC8_CRC16.h"
#include "fifo.h"
#include "detect_task.h"
#include "arm_math.h"
#include "user_lib.h"

#include "miniPC_msg.h"
	
#if INCLUDE_uxTaskGetStackHighWaterMark
uint32_t miniPC_comm_task_high_water;
#endif

void pc_unpack_fifo_data(void);
	
fifo_s_t pc_comm_fifo;
uint8_t pc_comm_fifo_buf[MINIPC_COMM_RX_FIFO_BUF_LENGTH];
uint8_t pc_comm_usart1_buf[2][MINIPC_COMM_UART_DMA_RX_BUF_LENGHT];
pc_comm_unpack_data_t pc_comm_unpack_data_obj;

//data after process
//miniPC_info_t miniPC_info;

void pc_communication_task(void const *pvParameters)
{
	init_miniPC_comm_struct_data();
	fifo_s_init(&pc_comm_fifo, pc_comm_fifo_buf, MINIPC_COMM_RX_FIFO_BUF_LENGTH);
	usart1_init(pc_comm_usart1_buf[0], pc_comm_usart1_buf[1], MINIPC_COMM_UART_DMA_RX_BUF_LENGHT);
	
	//miniPC_info.miniPC_connection_status = miniPC_offline;//init connection status
	
	while(1)
	{
		
		//PC-->Embeded; received data unpack
		pc_unpack_fifo_data();
		//Embeded-->PC; send data to PC
		
		//osDelay(10);
		//osDelay(4);
		//osDelay(100);
		vTaskDelay(100);
		
		//record high water mark
#if INCLUDE_uxTaskGetStackHighWaterMark
        miniPC_comm_task_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
	}
}


void pc_unpack_fifo_data(void)
{
  uint8_t byte = 0;
  uint8_t sof = PC_HEADER_SOF;
  pc_comm_unpack_data_t *p_obj = &pc_comm_unpack_data_obj;

  while ( fifo_s_used(&pc_comm_fifo) )
  {
    byte = fifo_s_get(&pc_comm_fifo);
    switch(p_obj->unpack_step)
    {
      case PC_COMM_STEP_HEADER_SOF:
      {
        if(byte == sof)
        {
          p_obj->unpack_step = PC_COMM_STEP_FRAME_LENGTH;
          p_obj->protocol_packet[p_obj->index++] = byte;
        }
        else
        {
          p_obj->index = 0;
        }
      }break;
			
			case PC_COMM_STEP_FRAME_LENGTH: //---------------------------------------------------1-17-2023
			{
			}break;
      
      case STEP_LENGTH_LOW:
      {
        p_obj->data_len = byte;
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_LENGTH_HIGH;
      }break;
      
      case STEP_LENGTH_HIGH:
      {
        p_obj->data_len |= (byte << 8);
        p_obj->protocol_packet[p_obj->index++] = byte;

        if(p_obj->data_len < (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_CMDID_LEN))
        {
          p_obj->unpack_step = STEP_FRAME_SEQ;
        }
        else
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }break;
      case STEP_FRAME_SEQ:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;
        p_obj->unpack_step = STEP_HEADER_CRC8;
      }break;

      case STEP_HEADER_CRC8:
      {
        p_obj->protocol_packet[p_obj->index++] = byte;

        if (p_obj->index == REF_PROTOCOL_HEADER_SIZE)
        {
          if ( verify_CRC8_check_sum(p_obj->protocol_packet, REF_PROTOCOL_HEADER_SIZE) )
          {
            p_obj->unpack_step = STEP_DATA_CRC16;
          }
          else
          {
            p_obj->unpack_step = STEP_HEADER_SOF;
            p_obj->index = 0;
          }
        }
      }break;  
      
      case STEP_DATA_CRC16:
      {
        if (p_obj->index < (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
        {
           p_obj->protocol_packet[p_obj->index++] = byte;  
        }
        if (p_obj->index >= (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
        {
          p_obj->unpack_step = PC_COMM_STEP_HEADER_SOF;
          p_obj->index = 0;

          if ( verify_CRC16_check_sum(p_obj->protocol_packet, REF_HEADER_CRC_CMDID_LEN + p_obj->data_len) )
          {
            referee_data_solve(p_obj->protocol_packet);
          }
        }
      }break;

      default:
      {
        p_obj->unpack_step = PC_COMM_STEP_HEADER_SOF;
        p_obj->index = 0;
      }break;
    }
  }
}
	
	
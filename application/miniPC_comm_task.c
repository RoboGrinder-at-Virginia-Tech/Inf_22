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
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>
#include "detect_task.h"
	
#if INCLUDE_uxTaskGetStackHighWaterMark
uint32_t miniPC_comm_task_high_water;
#endif
	
void client_ui_task(void const *pvParameters)
{
	while(1)
	{
		
		//PC-->Embeded; received data unpack
		
		//Embeded-->PC; send data to PC
		
		//record high water mark
#if INCLUDE_uxTaskGetStackHighWaterMark
        miniPC_comm_task_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
	}
}
	
	
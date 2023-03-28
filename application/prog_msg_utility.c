/**
  ****************************(C) COPYRIGHT 2023 RoboGrinder at Virginia Tech****************************
  * @file       prog_msg_utility.c/h
  * @brief      prog_msg_utility. program utility relared functions
  * @note       This file provide functions related to debug(CPU usage), 
	*							message information about miniPC 
	*							<-> TypeC board communication(Tx and Rx) package freq and package loss rate
  *           
  * @history
  *  Version    Date            Author          Modification
  *  V1.0.0     Mar-28-2023     Zelin Shen      basic utility functions
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
	
#include "prog_msg_utility.h"
#include "main.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"

#include "miniPC_comm_task.h"
#include "string.h"
#include "stdio.h"
#include "CRC8_CRC16.h"
#include "detect_task.h"
#include "SuperCap_comm.h"
#include "referee.h"
#include "arm_math.h"

#include "FreeRTOS.h"

/* RTOS based Run time stats gathering functions. 
In file FreeRTOS.h we defined
#ifndef configGENERATE_RUN_TIME_STATS
	#define configGENERATE_RUN_TIME_STATS 1
#endif
which enabled RTOS to help us do run time stats
*/

//extern volatile uint32_t ulHighFrequencyTimerCounts;    /* 在高精度定时器中断中累加 */
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()    （ulHighFrequencyTimerCounts = 0ul）
//#define portGET_RUN_TIME_COUNTER_VALUE() ulHighFrequencyTimerCounts



	
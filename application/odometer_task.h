#ifndef ODOMETER_TASK_H
#define ODOMETER_TASK_H

#include "struct_typedef.h"

#include "CAN_receive.h"
//#include "gimbal_task.h"
//#include "remote_control.h"
#include "user_lib.h"

typedef struct
{
	 const motor_measure_t *motor_ptr;
	 fp32 total_ecd_count; //当前里程计信息
	 fp32 last_total_ecd_count; //上一次里程计信息
}general_odom_info_t;

extern void odometer_init(void);
extern void odometer_task(void);
extern fp32 get_trig_modor_odom_count(void);

#endif

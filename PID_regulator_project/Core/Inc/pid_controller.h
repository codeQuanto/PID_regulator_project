/*
 * pid_controler.h
 *
 *  Created on: Jan 15, 2025
 *      Author: Igor
 */

#ifndef INC_PID_CONTROLLER_H_
#define INC_PID_CONTROLLER_H_

#include "stm32l073xx.h"

typedef struct{
	int previous_error;		//poprzedni blad dla czlonu rozniczkujacego
	int total_error;		//suma bledow dla czlonu calkuajcego
	float Kp;				//wzmocnienie czlonu proporcjonalnego
	float Ki;				//wzmocnienie czlonu calkuajcego
	float Kd;				//wzmocnienie czlonu rozniczkujacego
	int anti_windup_limit;	//anti-windup - ograniczenie czlonu calkujacego
}pid_struct;

#define MOTOR_Kp 4.8
#define MOTOR_Ki 0
#define MOTOR_Kd 0
#define MOTOR_ANTI_WINDUP 1000

void pid_init(pid_struct *pid_data, float kp_init, float ki_init, float kd_init, int anti_windup_limit_init);
void pid_reset(pid_struct *pid_data);
int pid_calculate(pid_struct *pid_data, int setpoint, int process_variable);


#endif /* INC_PID_CONTROLLER_H_ */

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
	int total_error_limit;	//ograniczenie maksymalnego bledu zeby przyspieszyc dzialanie przy duzych zmianach
}pid_struct;

#define MOTOR_Kp 1.2
#define MOTOR_Ki 0.1
#define MOTOR_Kd 0.2
#define MOTOR_ANTI_WINDUP 1000
#define MOTOR_TOTAL_ERROR_LIMIT 150

void pid_init(pid_struct *pid_data);
void pid_reset(pid_struct *pid_data);
int pid_calculate(pid_struct *pid_data, int setpoint, int process_variable);


#endif /* INC_PID_CONTROLLER_H_ */

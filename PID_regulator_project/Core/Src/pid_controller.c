/*
 * pid_controller.c
 *
 *  Created on: Jan 15, 2025
 *      Author: Igor
 */

#include <pid_controller.h>

void pid_init(pid_struct *pid_data, float kp_init, float ki_init, float kd_init, int anti_windup_limit_init){
	pid_data->previous_error = 0;
	pid_data->total_error = 0;

	pid_data->Kp = kp_init;
	pid_data->Ki = ki_init;
	pid_data->Kd = kd_init;

	pid_data->anti_windup_limit = anti_windup_limit_init;
}

void pid_reset(pid_struct *pid_data){
	pid_data->previous_error = 0;
	pid_data->total_error = 0;
}

int pid_calculate(pid_struct *pid_data, int setpoint, int process_variable){
	int error;
	float p_term, i_term, d_term;

	error = setpoint - process_variable;									//obliczenie uchybu
	pid_data->total_error += error;											//sumowanie uchybu

	p_term = (float)(pid_data->Kp * error);									//odpowiedz czlonu proporcjonalnego
	i_term = (float)(pid_data->Ki * pid_data->total_error);					//odpowiedz czlonu calkujacego
	d_term = (float)(pid_data->Kd * (error - pid_data->previous_error));	//odpowiedz czlonu rozniczkujacego

																			//ograniczenie czlonu calkujacego
	if (i_term >= pid_data->anti_windup_limit) {
		i_term = pid_data->anti_windup_limit;
	} else if(i_term <= -pid_data->anti_windup_limit){
		i_term = -pid_data->anti_windup_limit;
	}

	pid_data->previous_error = error;

	return (int)(p_term + i_term + d_term);									//odpowiedz regulatora
}

/*
 * pid_controller.c
 *
 *  Created on: Jan 15, 2025
 *      Author: Igor
 */

#include <pid_controller.h>

void pid_init(pid_struct *pid_data){
	pid_data->previous_error = 0;
	pid_data->total_error = 0;

	pid_data->Kp = MOTOR_Kp;
	pid_data->Ki = MOTOR_Ki;
	pid_data->Kd = MOTOR_Kd;

	pid_data->anti_windup_limit = MOTOR_ANTI_WINDUP;
	pid_data->total_error_limit = MOTOR_TOTAL_ERROR_LIMIT;
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

	if (pid_data->total_error >= pid_data->total_error_limit) {				//ograniczenie maksymalnego bledu
		pid_data->total_error = pid_data->total_error_limit;
	} else if (pid_data->total_error <= -pid_data->total_error_limit) {
		pid_data->total_error = -pid_data->total_error_limit;
	}

	p_term = (float) (pid_data->Kp * error);								//odpowiedz czlonu proporcjonalnego
	i_term = (float) (pid_data->Ki * pid_data->total_error);				//odpowiedz czlonu calkujacego
	d_term = (float) (pid_data->Kd * (error - pid_data->previous_error));	//odpowiedz czlonu rozniczkujacego

																			//ograniczenie czlonu calkujacego
	if (i_term >= pid_data->anti_windup_limit) {
		i_term = pid_data->anti_windup_limit;
	} else if (i_term <= -pid_data->anti_windup_limit) {
		i_term = -pid_data->anti_windup_limit;
	}

	pid_data->previous_error = error;

	return (int)(p_term + i_term + d_term);									//odpowiedz regulatora
}

//obsluzyc case gdy PWM=100% a obroty i tak sa za wolne

/*
 * encoder.c
 *
 *  Created on: Jan 14, 2025
 *      Author: Igor
 */

#include <motor.h>

void motor_init(motor_struct *motor, TIM_HandleTypeDef *encoder_tim, TIM_HandleTypeDef *interrupt_tim){

	motor->timer = encoder_tim;
	motor->resolution = TIMER_CONFIGURATION_BOTH_EDGES * ENCODER_RESOLUTION * MOTOR_GEAR;

	motor->pulse_count = 0;
	motor->measured_speed = 0;
	motor->set_speed = 0;
	motor->actual_PWM = 0;
	motor->actual_dir = cw;

	//HAL_TIM_Encoder_Start(encoder_tim, TIM_CHANNEL_ALL);
	//HAL_TIM_Base_Start_IT(interrupt_tim);
}

void motor_update_count(motor_struct *motor){
	motor->pulse_count = (int16_t)__HAL_TIM_GET_COUNTER(motor->timer);
	__HAL_TIM_SET_COUNTER(motor->timer, 0);
}

void motor_calculate_speed(motor_struct *motor){
	motor_update_count(motor);
	motor->measured_speed = (motor->pulse_count * TIMER_INTERRUPT_FREQUENCY * SECONDS_IN_MINUTE)/ (motor->resolution);

	int output = pid_calculate(&(motor->pid_controller), motor->set_speed, motor->measured_speed);


	if (output >= 1000) { //na razie na sztywno 1000 jako maksymalna wartosc PWM
			output = 1000;
	} else if (output <= -1000) {
		output = -1000;
	}

	motor->actual_PWM += output;

	if (motor->actual_PWM >= 0 ) {
		Cytron_Set_Motor_Direction(cw);
		Cytron_Set_Motor_Speed(motor->actual_PWM);
	} else{
		Cytron_Set_Motor_Direction(ccw);
		Cytron_Set_Motor_Speed(-motor->actual_PWM); //potencjalny problem gdy ta wartosc wykroczy poza zakres uint16_t
	}
}

void motor_set_RPM_speed(motor_struct *motor, int RPM_speed){
	if (RPM_speed != motor->set_speed) {
		pid_reset(&(motor->pid_controller));
		motor->set_speed = RPM_speed;
	}
}

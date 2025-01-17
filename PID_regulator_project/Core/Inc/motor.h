/*
 * encoder.h
 *
 *  Created on: Jan 14, 2025
 *      Author: Igor
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32l0xx.h"
#include "pid_controller.h"
#include "cytronDriver.h"

typedef struct{
	TIM_HandleTypeDef *timer;	//timer obslugujacy enkoder silnika

	uint16_t resolution;		//ilosc zliczen timera na obrot walu

	int32_t pulse_count;		//zliczone impulsy
	int32_t measured_speed;		//wyliczona predkosc
	int32_t set_speed;			//zadana predkosc

	int32_t actual_PWM;			//wartosc PWM

	pid_struct pid_controller;
}motor_struct;

#define ENCODER_RESOLUTION				8 //wynika z hardware'u
#define TIMER_CONFIGURATION_BOTH_EDGES	4 //ilosc zliczen na jeden impuls
#define MOTOR_GEAR						120
#define TIMER_INTERRUPT_FREQUENCY					10 //Hz
#define SECONDS_IN_MINUTE				60

void motor_init(motor_struct *motor, TIM_HandleTypeDef *encoder_tim, TIM_HandleTypeDef *interrupt_tim);
void motor_update_count(motor_struct *motor);
void motor_calculate_speed(motor_struct *motor);
void motor_set_RPM_speed(motor_struct *motor, int RPM_speed);

#endif /* INC_MOTOR_H_ */

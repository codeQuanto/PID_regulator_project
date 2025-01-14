/*
 * cytronDriver.h
 *
 * Prosty sterownik do obsługi silnika DC za pomocą sygnału PWM.
 * Sterownik umożliwia kontrolę prędkości i kierunku obrotów silnika.
 * Używa timera (TIM2) do generowania sygnału PWM na odpowiednich kanałach.
 *
 * Funkcje:
 * - Cytron_Set_Motor_Direction(Cytron_Direction direction): Ustawia kierunek obrotów silnika (cw - zgodnie z ruchem wskazówek zegara, ccw - przeciwnie).
 * - Cytron_Set_Motor_Speed(uint8_t speed): Ustawia prędkość silnika (wartość PWM od 0 do wartości ARR).
 * - Cytron_Motor_Init(): Inicjalizuje sterownik silnika z początkową prędkością 0 i kierunkiem obrotów cw.
 *
 * Wymagania:
 * - Timer (TIM2) skonfigurowany do generowania sygnału PWM.
 * - Kanały PWM (TIM_CHANNEL_1 i TIM_CHANNEL_2) przypisane do odpowiednich pinów mikrokontrolera.
 *
 *  Created on: Dec 1, 2024
 *      Author: Igor
 */


#ifndef INC_CYTRONDRIVER_H_
#define INC_CYTRONDRIVER_H_

#include "stm32l0xx.h"

extern TIM_HandleTypeDef htim2;
extern uint32_t workingChannel; //zmienna pomocnicza do wybrania odpowiedniego kanalu w zaleznosci od kierunku obrotow

typedef enum {
	cw = 0, ccw = 1
} Cytron_Direction;


void Cytron_Set_Motor_Direction(Cytron_Direction direction);
void Cytron_Set_Motor_Speed(uint8_t speed);
void Cytron_Motor_Init();

#endif /* INC_CYTRONDRIVER_H_ */

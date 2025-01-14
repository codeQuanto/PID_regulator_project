/*
 * cytronDriver.c
 *
 * Implementacja sterownika silnika DC z użyciem sygnału PWM generowanego przez timer mikrokontrolera STM32.
 * Sterownik umożliwia ustawienie prędkości silnika poprzez kontrolowanie wypełnienia sygnału PWM.
 * Dodatkowo, umożliwia ustawienie kierunku obrotów silnika.
 *
 * Funkcje:
 * - Cytron_Set_Motor_Direction(Cytron_Direction direction): Ustawia kierunek obrotów silnika (cw - zgodnie z ruchem wskazówek zegara, ccw - przeciwnie).
 * - Cytron_Set_Motor_Speed(uint8_t speed): Ogranicza wartość prędkości do maksymalnej wartości ARR timera i ustawia wypełnienie sygnału PWM.
 * - Cytron_Motor_Init(): Inicjalizuje sterownik silnika, ustawia kierunek obrotów na cw i prędkość na 0, a także uruchamia odpowiednie kanały PWM timera.
 *
 * Wymagania:
 * - Timer (TIM2) skonfigurowany do generowania sygnału PWM.
 * - Kanały PWM (TIM_CHANNEL_1 i TIM_CHANNEL_2) przypisane do odpowiednich pinów mikrokontrolera.
 *
 *
 *  Created on: Dec 1, 2024
 *      Author: Igor
 */

#include "cytronDriver.h"

uint32_t workingChannel;

//dzialanie tej funkcji wynika z dzialania sterownika: in1 high, in2 low -> cw, in1 low, in2 high -> ccw
void Cytron_Set_Motor_Direction(Cytron_Direction direction){
	if (direction == cw) {
		workingChannel = TIM_CHANNEL_1;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0); //ustawienie wypelnienia pwm in 2 na 0%
	}else if (direction == ccw) {
		workingChannel = TIM_CHANNEL_2;
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
	}
}

void Cytron_Set_Motor_Speed(uint8_t speed){
	//jesli wypelnienie jest wieksze niz maksymalne to przypisuje maksymalna wartosc
	if (speed >= htim2.Instance->ARR) {
		speed = htim2.Instance->ARR;
	}

	__HAL_TIM_SET_COMPARE(&htim2, workingChannel, speed);
}

//ustawienie domyslnych wartosci startowych i uruchomienie timera
void Cytron_Motor_Init(){
	Cytron_Set_Motor_Direction(cw);
	Cytron_Set_Motor_Speed(0);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

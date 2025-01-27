/*
 * cytronDriver.c
 *
 * Implementacja sterownika silnika DC z użyciem sygnału PWM generowanego przez timer mikrokontrolera STM32.
 *
 *  Created on: Dec 1, 2024
 *      Author: Igor
 */

#include "cytronDriver.h"

/**
 * @brief Ustawia kierunek obrotów silnika.
 * Kierunek wynika z działania sterownika: in1 high, in2 low -> cw; in1 low, in2 high -> ccw.
 *
 * @param driver Wskaźnik do struktury sterownika silnika.
 * @param direction Kierunek obrotów (cw - zgodnie z ruchem wskazówek zegara, ccw - przeciwnie).
 */
void Cytron_Set_Motor_Direction(driver_struct *driver, Cytron_Direction direction) {
	if (direction == cw) {
		driver->working_channel = TIM_CHANNEL_1;
		__HAL_TIM_SET_COMPARE(driver->timer, TIM_CHANNEL_2, 0); // Ustawienie wypełnienia PWM in2 na 0%
	} else if (direction == ccw) {
		driver->working_channel = TIM_CHANNEL_2;
		__HAL_TIM_SET_COMPARE(driver->timer, TIM_CHANNEL_1, 0); // Ustawienie wypełnienia PWM in1 na 0%
	}
}

/**
 * @brief Ustawia prędkość obrotów silnika.
 * Prędkość ograniczona do maksymalnej wartości ARR timera.
 *
 * @param driver Wskaźnik do struktury sterownika silnika.
 * @param speed Wartość PWM określająca prędkość (0 do wartości ARR).
 */
void Cytron_Set_Motor_Speed(driver_struct *driver, uint16_t speed) {
	if (speed >= driver->timer->Instance->ARR) {
		speed = driver->timer->Instance->ARR;
	}
	__HAL_TIM_SET_COMPARE(driver->timer, driver->working_channel, speed);
}

/**
 * @brief Inicjalizuje sterownik silnika z domyślnymi wartościami.
 * Ustawia kierunek na zgodny z ruchem wskazówek zegara (cw) i prędkość na 0.
 *
 * @param driver Wskaźnik do struktury sterownika silnika.
 * @param PWM_timer Wskaźnik do timera generującego PWM.
 */
void Cytron_Motor_Init(driver_struct *driver, TIM_HandleTypeDef *PWM_timer) {
	driver->timer = PWM_timer;
	driver->working_channel = TIM_CHANNEL_1;
	Cytron_Set_Motor_Direction(driver, cw);
	Cytron_Set_Motor_Speed(driver, 0);
}

/*
 * cytronDriver.h
 *
 * Sterownik silnika DC do obsługi prędkości i kierunku obrotów za pomocą PWM.
 *
 * Struktury:
 * - driver_struct: Zawiera wskaźnik do timera PWM, kierunek obrotów i aktualny kanał timera.
 *
 *  Created on: Dec 1, 2024
 *      Author: Igor
 */

#ifndef INC_CYTRONDRIVER_H_
#define INC_CYTRONDRIVER_H_

#include "stm32l0xx.h"

/**
 * @brief Enum określający kierunek obrotów silnika.
 * - cw: zgodnie z ruchem wskazówek zegara.
 * - ccw: przeciwnie do ruchu wskazówek zegara.
 */
typedef enum {
	cw = 0, /* Clockwise*/
	ccw = 1 /* Counterclockwise*/
} Cytron_Direction;

/**
 * @brief Struktura reprezentująca sterownik silnika DC.
 * - timer: Wskaźnik do timera generującego PWM.
 * - direction: Kierunek obrotów silnika.
 * - working_channel: Aktualnie pracujący kanał timera.
 */
typedef struct {
	TIM_HandleTypeDef *timer; /* Timer generujący PWM*/
	Cytron_Direction direction; /* Kierunek obrotów*/

	uint32_t working_channel; /* Aktualnie pracujący kanał timera*/
} driver_struct;

/**
 * @brief Ustawia kierunek obrotów silnika.
 * @param driver: Wskaźnik do struktury sterownika silnika.
 * @param direction: Kierunek obrotów (cw lub ccw).
 */
void Cytron_Set_Motor_Direction(driver_struct *driver,
		Cytron_Direction direction);

/**
 * @brief Ustawia prędkość obrotów silnika.
 * @param driver: Wskaźnik do struktury sterownika silnika.
 * @param speed: Wartość PWM określająca prędkość (0 do wartości ARR).
 */
void Cytron_Set_Motor_Speed(driver_struct *driver, uint16_t speed);

/**
 * @brief Inicjalizuje sterownik silnika z początkową prędkością 0 i domyślnym kierunkiem.
 * @param driver: Wskaźnik do struktury sterownika silnika.
 * @param PWM_timer: Wskaźnik do timera generującego PWM.
 */
void Cytron_Motor_Init(driver_struct *driver, TIM_HandleTypeDef *PWM_timer);

#endif /* INC_CYTRONDRIVER_H_ */

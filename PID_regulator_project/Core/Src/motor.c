/*
 * motor.c
 *
 *  Created on: Jan 14, 2025
 *      Author: Igor
 *
 *
 * Implementacja funkcji sterowania silnikiem, w tym inicjalizacja, obliczanie prędkości oraz
 * integracja z regulatorem PID i sterownikiem silnika Cytron.
 */

#include <motor.h>

/**
 * @brief Inicjalizuje strukturę sterownika silnika.
 *
 * Ustawia domyślne wartości parametrów struktury oraz inicjalizuje
 * rozdzielczość enkodera na podstawie zdefiniowanych stałych.
 *
 * @param motor Wskaźnik do struktury motor_struct reprezentującej sterownik silnika.
 * @param encoder_tim Wskaźnik do uchwytu timera obsługującego enkoder.
 */
void motor_init(motor_struct *motor, TIM_HandleTypeDef *encoder_tim) {
	motor->timer = encoder_tim;
	motor->resolution = TIMER_CONFIGURATION_BOTH_EDGES * ENCODER_RESOLUTION
			* MOTOR_GEAR;

	motor->pulse_count = 0;
	motor->measured_speed = 0;
	motor->set_speed = 0;
	motor->actual_PWM = 0;
}

/**
 * @brief Aktualizuje licznik impulsów enkodera.
 *
 * Zczytuje aktualną wartość z licznika timera obsługującego enkoder,
 * zapisuje ją do struktury sterownika silnika, a następnie zeruje licznik timera.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 */
void motor_update_count(motor_struct *motor) {
	motor->pulse_count = (int16_t) __HAL_TIM_GET_COUNTER(motor->timer);
	__HAL_TIM_SET_COUNTER(motor->timer, 0);
}

/**
 * @brief Oblicza aktualną prędkość obrotową silnika w RPM.
 *
 * Na podstawie liczby impulsów odczytanych z enkodera i parametrów
 * systemu (częstotliwości przerwań, rozdzielczości enkodera oraz przekładni)
 * wylicza prędkość obrotową silnika. Wynik jest przekazywany do kontrolera PID,
 * a wynik jego działania aktualizuje wartość sygnału PWM sterującego silnikiem.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 */
void motor_calculate_speed(motor_struct *motor) {
	motor_update_count(motor);
	motor->measured_speed = (motor->pulse_count * TIMER_INTERRUPT_FREQUENCY
			* SECONDS_IN_MINUTE) / (motor->resolution);

	int output = pid_calculate(&(motor->pid_controller), motor->set_speed,
			motor->measured_speed);

	if (output >= 1000) { /* Maksymalna wartość PWM, ograniczenie zbyt gwałtownych zmian w jednym kroku*/
		output = 1000;
	} else if (output <= -1000) {
		output = -1000;
	}

	motor->actual_PWM += output;

	if (motor->actual_PWM >= 0) {
		Cytron_Set_Motor_Direction(&(motor->driver), cw);
		Cytron_Set_Motor_Speed(&(motor->driver), motor->actual_PWM);
	} else {
		Cytron_Set_Motor_Direction(&(motor->driver), ccw);
		Cytron_Set_Motor_Speed(&(motor->driver), -motor->actual_PWM);
	}
}

/**
 * @brief Ustawia zadaną prędkość obrotową silnika w RPM.
 *
 * Jeżeli nowa prędkość różni się od bieżącej, resetuje stan kontrolera PID
 * i zapisuje nową wartość zadanej prędkości.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 * @param RPM_speed Zadana prędkość obrotowa w RPM.
 */
void motor_set_RPM_speed(motor_struct *motor, int RPM_speed) {
	if (RPM_speed != motor->set_speed) {
		pid_reset(&(motor->pid_controller));
		motor->set_speed = RPM_speed;
	}
}

/**
 * @brief Zatrzymuje silnik.
 *
 * Resetuje zadaną prędkość, zmierzoną prędkość oraz sygnał PWM
 * do wartości początkowych. Ustawia prędkość PWM na 0.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 */
void motor_stop(motor_struct *motor) {
	motor->set_speed = 0;
	motor->measured_speed = 0;
	motor->actual_PWM = 0;
	Cytron_Set_Motor_Speed(&(motor->driver), 0);
}

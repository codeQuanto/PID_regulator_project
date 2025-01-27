/*
 * motor.h
 *
 *  Created on: Jan 14, 2025
 *      Author: Igor
 *
 * Nagłówek dla funkcji sterowania silnikiem, w tym deklaracje struktury motor_struct oraz
 * funkcji do inicjalizacji, obliczania prędkości, ustawiania zadanej prędkości oraz zatrzymywania silnika.
 * Zawiera również definicje stałych konfiguracyjnych związanych z enkoderem i sterowaniem silnikiem.
 */

#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32l0xx.h"
#include "pid_controller.h"
#include "cytronDriver.h"

/**
 * @brief Struktura reprezentująca sterownik silnika.
 *
 * Zawiera informacje o timerze obsługującym enkoder, rozdzielczości enkodera, liczbie impulsów,
 * zmierzonej prędkości, zadanej prędkości, wartości PWM oraz strukturze PID i drivera silnika.
 */
typedef struct {
	TIM_HandleTypeDef *timer;	///< Wskaźnik do timera obsługującego enkoder silnika

	uint16_t resolution;		///< Liczba zliczeń timera na jeden obrót wału

	int32_t pulse_count;		///< Zliczone impulsy
	int32_t measured_speed;		///< Obliczona prędkość
	int32_t set_speed;			///< Zadana prędkość

	int32_t actual_PWM;			///< Wartość PWM

	driver_struct driver;			///< Struktura sterownika silnika
	pid_struct pid_controller;	///< Struktura kontrolera PID
} motor_struct;

// Definicje stałych konfiguracyjnych dla enkodera i silnika
#define ENCODER_RESOLUTION				8  ///< Wynika z hardware'u
#define TIMER_CONFIGURATION_BOTH_EDGES	4  ///< Liczba zliczeń na jeden impuls
#define MOTOR_GEAR						120 ///< Przełożenie silnika
#define TIMER_INTERRUPT_FREQUENCY		10 ///< Częstotliwość przerwań (Hz)
#define SECONDS_IN_MINUTE				60 ///< Sekundy w minucie

/**
 * @brief Inicjalizuje strukturę sterownika silnika.
 *
 * Ustawia parametry sterownika silnika, w tym rozdzielczość enkodera oraz timer.
 *
 * @param motor Wskaźnik do struktury motor_struct reprezentującej sterownik silnika.
 * @param encoder_tim Wskaźnik do uchwytu timera obsługującego enkoder.
 */
void motor_init(motor_struct *motor, TIM_HandleTypeDef *encoder_tim);

/**
 * @brief Aktualizuje licznik impulsów enkodera.
 *
 * Zczytuje bieżącą wartość licznika timera i zapisuje ją do struktury silnika,
 * a następnie resetuje licznik.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 */
void motor_update_count(motor_struct *motor);

/**
 * * @brief Oblicza prędkość obrotową silnika w RPM i aktualizuje sygnał PWM sterujący silnikiem.
 *
 * Na podstawie liczby impulsów z enkodera oraz ustawień systemu (częstotliwość przerwań,
 * rozdzielczość enkodera, przekładnia) wylicza prędkość obrotową silnika.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 */
void motor_calculate_speed(motor_struct *motor);

/**
 * @brief Ustawia zadaną prędkość obrotową silnika w RPM.
 *
 * Resetuje stan kontrolera PID i zapisuje nową wartość zadanej prędkości.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 * @param RPM_speed Zadana prędkość obrotowa w RPM.
 */
void motor_set_RPM_speed(motor_struct *motor, int RPM_speed);

/**
 * @brief Zatrzymuje silnik.
 *
 * Resetuje wszystkie parametry sterowania silnikiem do wartości początkowych
 * i ustawia prędkość PWM na 0.
 *
 * @param motor Wskaźnik do struktury motor_struct.
 */
void motor_stop(motor_struct *motor);

#endif /* INC_MOTOR_H_ */

/*
 * pid_controller.h
 *
 *  Created on: Jan 15, 2025
 *      Author: Igor
 *
 * Nagłówek zawierający definicję struktury i funkcji regulatora PID.
 * Regulator PID jest używany do sterowania prędkością silnika, uwzględniając błąd
 * między zadanym a rzeczywistym stanem (np. prędkością), oraz oblicza odpowiednią wartość
 * sterującą, która pozwala na utrzymanie zadanej prędkości.
 */

#ifndef INC_PID_CONTROLLER_H_
#define INC_PID_CONTROLLER_H_

#include "stm32l073xx.h"

/**
 * @brief Struktura przechowująca dane regulatora PID.
 *
 * Zawiera zmienne niezbędne do obliczeń PID, takie jak błędy z poprzednich cykli,
 * współczynniki PID, limity dla sumy błędów oraz parametr anti-windup.
 */
typedef struct{
	int previous_error;        ///< Błąd z poprzedniego cyklu dla członu różniczkującego
	int total_error;           ///< Całkowity błąd sumowany dla członu całkującego
	float Kp;                  ///< Wzmocnienie członu proporcjonalnego
	float Ki;                  ///< Wzmocnienie członu całkującego
	float Kd;                  ///< Wzmocnienie członu różniczkującego
	int anti_windup_limit;     ///< Ograniczenie dla członu całkującego (zapobiega efektowi windup)
	int total_error_limit;     ///< Limit maksymalnego błędu (przyspiesza działanie przy dużych zmianach)
}pid_struct;

#define MOTOR_Kp 1.2            ///< Wartość wzmocnienia członu proporcjonalnego
#define MOTOR_Ki 0.1            ///< Wartość wzmocnienia członu całkującego
#define MOTOR_Kd 0.2            ///< Wartość wzmocnienia członu różniczkującego
#define MOTOR_ANTI_WINDUP 1000  ///< Limit dla członu całkującego (anti-windup)
#define MOTOR_TOTAL_ERROR_LIMIT 150  ///< Limit maksymalnego błędu całkowitego

/**
 * @brief Inicjalizuje dane struktury PID.
 *
 * Funkcja ustawia domyślne wartości współczynników PID oraz limity błędów,
 * przygotowując strukturę do użycia w dalszych obliczeniach.
 *
 * @param pid_data Wskaźnik do struktury pid_struct, którą należy zainicjalizować.
 */
void pid_init(pid_struct *pid_data);

/**
 * @brief Resetuje dane regulatora PID.
 *
 * Funkcja zeruje błędy, zarówno bieżący, jak i całkowity, przygotowując
 * regulator do nowego cyklu obliczeniowego.
 *
 * @param pid_data Wskaźnik do struktury pid_struct, której stan ma zostać zresetowany.
 */
void pid_reset(pid_struct *pid_data);

/**
 * @brief Oblicza wartość sterującą na podstawie algorytmu PID.
 *
 * Funkcja oblicza odpowiedź regulatora PID na podstawie uchybu (różnicy między
 * wartością zadaną a rzeczywistą) oraz współczynników PID. Uwzględnia również
 * ograniczenia dla sumy błędów i członu całkującego (zapobiegając efektowi windup).
 *
 * @param pid_data Wskaźnik do struktury pid_struct, zawierającej dane regulatora PID.
 * @param setpoint Zadana wartość, do której dąży system (np. zadana prędkość).
 * @param process_variable Rzeczywista wartość zmiennej procesowej (np. zmierzona prędkość).
 *
 * @return Wartość sterująca, która powinna zostać zastosowana w systemie (np. wartość PWM).
 */
int pid_calculate(pid_struct *pid_data, int setpoint, int process_variable);

#endif /* INC_PID_CONTROLLER_H_ */

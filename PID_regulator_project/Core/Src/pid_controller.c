/*
 * pid_controller.c
 *
 *  Created on: Jan 15, 2025
 *      Author: Igor
 *
 * Implementacja algorytmu PID do sterowania prędkością silnika.
 * Zawiera funkcje inicjalizacji, resetowania stanu regulatora oraz obliczania
 * wartości sterującej na podstawie uchybu (różnicy między wartością zadaną a rzeczywistą).
 */

#include <pid_controller.h>

/**
 * @brief Inicjalizuje struktury PID.
 *
 * Funkcja ustawia początkowe wartości parametrów PID, w tym współczynniki Kp, Ki, Kd,
 * ograniczenie dla sumy błędów (total_error) oraz ograniczenie dla członu całkującego
 * (anti_windup_limit). Inicjalizuje również zmienne przechowujące błąd z poprzedniego
 * cyklu oraz całkowity błąd.
 *
 * @param pid_data Wskaźnik do struktury pid_struct reprezentującej regulator PID.
 */
void pid_init(pid_struct *pid_data){
	pid_data->previous_error = 0;  ///< Inicjalizacja błędu z poprzedniego cyklu
	pid_data->total_error = 0;     ///< Inicjalizacja całkowitego błędu

	/* Ustawienie współczynników PID z wartości predefiniowanych*/
	pid_data->Kp = MOTOR_Kp;
	pid_data->Ki = MOTOR_Ki;
	pid_data->Kd = MOTOR_Kd;

	/* Ustawienie limitów*/
	pid_data->anti_windup_limit = MOTOR_ANTI_WINDUP;
	pid_data->total_error_limit = MOTOR_TOTAL_ERROR_LIMIT;
}

/**
 * @brief Resetuje stan regulatora PID.
 *
 * Funkcja zeruje błędy, zarówno bieżący, jak i całkowity, przygotowując regulator
 * do nowego cyklu obliczeń.
 *
 * @param pid_data Wskaźnik do struktury pid_struct reprezentującej regulator PID.
 */
void pid_reset(pid_struct *pid_data){
	pid_data->previous_error = 0;  ///< Zerowanie błędu z poprzedniego cyklu
	pid_data->total_error = 0;     ///< Zerowanie całkowitego błędu
}

/**
 * @brief Oblicza wartość sterującą regulatora PID.
 *
 * Funkcja oblicza odpowiedź regulatora PID na podstawie uchybu, czyli różnicy
 * między zadanym punktem (setpoint) a zmienną procesową (process_variable).
 * Wykonywane są obliczenia dla członu proporcjonalnego, całkującego oraz różniczkującego,
 * a następnie sumowanie wyników tych członów. Dodatkowo stosowane są ograniczenia
 * na sumę błędów oraz człon całkujący, aby zapobiec efektowi "windup".
 *
 * @param pid_data Wskaźnik do struktury pid_struct reprezentującej regulator PID.
 * @param setpoint Zadana wartość, do której dąży system.
 * @param process_variable Rzeczywista wartość zmiennej procesowej (np. zmierzona prędkość).
 *
 * @return Wartość sterująca, która ma być zastosowana w systemie (np. wartość PWM).
 */
int pid_calculate(pid_struct *pid_data, int setpoint, int process_variable){
	int error;
	float p_term, i_term, d_term;

	/* Obliczanie uchybu*/
	error = setpoint - process_variable;

	/* Sumowanie uchybu do całkowitego błędu*/
	pid_data->total_error += error;

	/* Ograniczenie maksymalnego błędu*/
	if (pid_data->total_error >= pid_data->total_error_limit) {
		pid_data->total_error = pid_data->total_error_limit;
	} else if (pid_data->total_error <= -pid_data->total_error_limit) {
		pid_data->total_error = -pid_data->total_error_limit;
	}

	/* Obliczanie odpowiedzi dla członów PID*/
	p_term = (float) (pid_data->Kp * error);                            ///< Odpowiedź członu proporcjonalnego
	i_term = (float) (pid_data->Ki * pid_data->total_error);             ///< Odpowiedź członu całkującego
	d_term = (float) (pid_data->Kd * (error - pid_data->previous_error));///< Odpowiedź członu różniczkującego

	/* Ograniczenie członu całkującego, aby uniknąć "windup"*/
	if (i_term >= pid_data->anti_windup_limit) {
		i_term = pid_data->anti_windup_limit;
	} else if (i_term <= -pid_data->anti_windup_limit) {
		i_term = -pid_data->anti_windup_limit;
	}

	/* Zapisanie bieżącego uchybu jako poprzedniego błędu*/
	pid_data->previous_error = error;

	/* Zwrócenie sumy wszystkich członów PID jako wartości sterującej*/
	return (int)(p_term + i_term + d_term);
}

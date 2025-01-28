/**
 * @file: I2C_LCD.h
 * Driver Name: [[ I2C_LCD Display ]]
 * SW Layer:   ECUAL
 * Created on: Jan 28, 2024
 * Author:     Khaled Magdy
 * -------------------------------------------
 * For More Information, Tutorials, etc.
 * Visit Website: www.DeepBlueMbedded.com
 *
 */

#ifndef I2C_LCD_H_
#define I2C_LCD_H_

#include "stdint.h"

#define I2C_LCD_MAX	1	// Maximum Number of I2C_LCD Modules in Your Project
#define I2C_LCD_1	0	// I2C_LCD Instance Number 1 (Add more if you need)

//-----[ Prototypes For All User External Functions ]-----
/**
 * @brief Wyświetla liczbę w formacie 4-znakowym na ekranie LCD.
 *
 * @Author: Igor
 * Funkcja formatuje liczbę całkowitą do formatu czteroznakowego (np. " 12 " dla liczby 12) i wyświetla ją na określonej pozycji ekranu LCD.
 * Liczba może być zarówno dodatnia, jak i ujemna. Funkcja obsługuje liczby w zakresie od -999 do 999.
 * Wartości większe niż 999 są wyświetlane tylko w setkach, dziesiątkach i jedności, natomiast liczby mniejsze niż 0 mają wyświetlany znak '-' na początku.
 *
 * @param I2C_LCD_InstanceIndex Indeks instancji LCD (jeśli obsługiwane są różne urządzenia LCD).
 * @param value Liczba całkowita, która ma zostać wyświetlona na ekranie.
 * @param Col Kolumna, w której ma zostać ustawiony kursor (indeks zaczyna się od 0).
 * @param Row Wiersz, w którym ma zostać ustawiony kursor (indeks zaczyna się od 0).
 */
void I2C_LCD_DisplayMotorFormat(uint8_t I2C_LCD_InstanceIndex, int32_t value, uint8_t Col, uint8_t Row); //moja funckja do wyswietlania wartosci predkosci na odpowiednich miejscach

void I2C_LCD_Init(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_Clear(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_Home(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_SetCursor(uint8_t I2C_LCD_InstanceIndex, uint8_t Col, uint8_t Row);
void I2C_LCD_WriteChar(uint8_t I2C_LCD_InstanceIndex, char Ch);
void I2C_LCD_WriteString(uint8_t I2C_LCD_InstanceIndex, char* Str);

void I2C_LCD_ShiftLeft(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_ShiftRight(uint8_t I2C_LCD_InstanceIndex);

void I2C_LCD_Backlight(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_NoBacklight(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_Display(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_NoDisplay(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_Cursor(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_NoCursor(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_Blink(uint8_t I2C_LCD_InstanceIndex);
void I2C_LCD_NoBlink(uint8_t I2C_LCD_InstanceIndex);

void I2C_LCD_CreateCustomChar(uint8_t I2C_LCD_InstanceIndex, uint8_t CharIndex, const uint8_t* CharMap);
void I2C_LCD_PrintCustomChar(uint8_t I2C_LCD_InstanceIndex, uint8_t CharIndex);

#endif /* I2C_LCD_H_ */

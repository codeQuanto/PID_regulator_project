Projekt PID do kontroli prędkości obrotowej silnika DC

Projekt implementuje algorytm PID do precyzyjnej kontroli prędkości obrotowej silnika DC.
Został opracowany w środowisku STM32Cube IDE, wykorzystując mikrokontroler Nucleo L073RZ i bibliotekę HAL.

W projekcie wykorzystane zostały następujące peryferia:
  -Timery do generowania PWM, odczytu enkodera i generowania przerwań
  -ADC w trybie DMA do odczytu wartości z potencjometra
  -UART do komunikacji z komputerem
  -I2C do wyświetlania danych na ekranie LCD
  
Dodatkowo, dane są monitorowane w czasie rzeczywistym przy pomocy STM32CubeMonitor.
Projekt obejmuje również stworzenie własnych bibliotek do sterowania silnikiem, algorytmu PID oraz obsługi silnika,
a także wykorzystanie gotowej biblioteki do obsługi LCD.

Dokumentacja projektu znajduje się w katalogu [docs2](./docs2), w tym [zdjecia pogladowe](.docs2/images) 
W szczególności dokumentacja wygenerowana w Doxygen znajduje się w pliku [index.html](https://codequanto.github.io/PID_regulator_project/).
Była to moja pierwsza styczność z Doxygen dlatego dokumentacja pozostawia jeszcze wiele do życzenia;) 

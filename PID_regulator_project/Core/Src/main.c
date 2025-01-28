/* USER CODE BEGIN Header */
/**
 * @file main.c
 * @brief Plik nagłówkowy do sterowania silnikiem DC z wykorzystaniem algorytmu PID.
 *
 * Plik główny projektu regulacego prędkości obrotowej silnika DC, dostosowaną do zmieniającego
 * się obciążenia. Algorytm PID zapewnia precyzyjną kontrolę prędkości
 * i stabilność pracy silnika w różnych warunkach.
 *
 * Program obsługuje sterowanie silnikiem oraz przetwarzanie danych z ADC przy użyciu DMA.
 * Timer generuje przerwanie co 100 ms, w którym realizowane są operacje sterujące silnikiem oraz
 * ustawiane flagi do dalszej komunikacji (UART oraz LCD).
 * Pętla główna odpowiedzialna jest za kontrolowanie tych flag i warunkowe obsłużenie UART oraz LCD.
 * Nadrzędna flaga pochodząca od przycisku warunkuje wykonywanie całego programu (nie blokuje kontrolera)
 *
 * Kluczowe funkcje:
 * 1. **Timer Interrupt (100 ms)** -  uaktualnienie odczytu z DMA, sterowanie silnikiem oraz
 * 		wystawianie flagi do komunikacji.
 * 2. **ADC w trybie DMA** - pobieranie danych z czujników w sposób bezpośredni, bez angażowania procesora,
 * 		co pozwala na asynchroniczne zbieranie próbek.
 * 3. **Pętla główna** - analizowanie flagi z przerwania, obsługa komunikacji.
 * 4. **EXTI Interrupt** - przerwanie działania programu, wyłączenie silnika
 *
 *  Created on: Dec 1, 2024
 *      Author: Igor
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cytronDriver.h"
#include "motor.h"
#include "string.h"
#include "stdio.h"
#include "./I2C_LCD_Inc/I2C_LCD.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim21;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM21_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
motor_struct motor_instance; /**< Instancja silnika DC */

volatile int flag_turn_on_off = 0; /**< Flaga do włączania/wyłączania silnika */
volatile int flag_send_data = 0; /**< Flaga wskazująca na konieczność wysłania danych przez UART */
volatile int flag_refresh_LCD = 0; /**< Flaga do odświeżania wyświetlacza LCD */

char uart_buffer[100]; /**< Bufor do komunikacji UART */
int lcd_refresh_counter = 0; /**< Licznik odświeżania wyświetlacza LCD */

volatile static uint16_t adc_value[1];/**< Tablica przechowująca wartość odczytaną z ADC. */
int new_speed = 0;/**< Obliczona prędkość silnika na podstawie odczytu z ADC. */

volatile uint32_t last_EXTI_interrupt_time; /**< Zmienna do cyfrowego debouncingu przycisku */

/* USER CODE END 0 */

/**
 * @brief  Główna funkcja programu, zawiera inicjalizację peryferiów oraz obsługę wystawionych flag.
 *
 * Funkcja inicjalizuje wszystkie peryferia, w tym ADC, PWM, enkoder, UART i LCD. Następnie
 * w pętli głównej programu monitoruje flagi i warunkowo wysyła dane przez UART oraz aktualizuje stan ekranu LCD
 *
 * @retval int Zwraca 0 w przypadku poprawnego zakończenia.
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM21_Init();
	MX_USART2_UART_Init();
	MX_ADC_Init();
	MX_I2C1_Init();
	/* USER CODE BEGIN 2 */

	/* Inicjalizacja LCD */
	I2C_LCD_Init(I2C_LCD_1);
	I2C_LCD_SetCursor(I2C_LCD_1, 0, 0);
	I2C_LCD_WriteString(I2C_LCD_1, "SYSTEM");
	I2C_LCD_SetCursor(I2C_LCD_1, 0, 1);
	I2C_LCD_WriteString(I2C_LCD_1, "INITIALISATION");
	HAL_Delay(1000);

	/* Inicjalizacja silnika i PID */
	Cytron_Motor_Init(&(motor_instance.driver), &htim2);
	motor_init(&motor_instance, &htim3);
	pid_init(&(motor_instance.pid_controller));

	/* Uruchomienie PWM, enkodera oraz licznika generującego przerwanie */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); /*timer do generacji PWM*/
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL); /*timer do obslugi enkodera*/
	HAL_TIM_Base_Start_IT(&htim21); /*timer do generowania przerwania*/

	/* Uruchomienie i kalibracja ADC */
	HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc, (uint32_t*) adc_value, 1);

	/* Ustawienia początkowe na LCD */
	I2C_LCD_Clear(I2C_LCD_1);
	I2C_LCD_SetCursor(I2C_LCD_1, 0, 0);
	I2C_LCD_WriteString(I2C_LCD_1, "SET RPM: 0");
	I2C_LCD_SetCursor(I2C_LCD_1, 0, 1);
	I2C_LCD_WriteString(I2C_LCD_1, "ACT RPM: 0");

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		/* Jeżeli silnik jest włączony, przetwórz dane wejściowe i wyświetl informacje */
		if (flag_turn_on_off != 0) {

			/* Jeśli flaga wysyłania danych jest ustawiona, wyślij dane przez UART */
			if (flag_send_data) {
				flag_send_data = 0;
				HAL_UART_Transmit(&huart2, (uint8_t*) uart_buffer,
						strlen(uart_buffer), HAL_MAX_DELAY);
			}

			/* Jeśli flaga odświeżania LCD jest ustawiona, zaktualizuj wyświetlacz */
			if (flag_refresh_LCD) {
				flag_refresh_LCD = 0;
				I2C_LCD_DisplayMotorFormat(I2C_LCD_1, motor_instance.set_speed,
						9, 0);
				I2C_LCD_DisplayMotorFormat(I2C_LCD_1,
						motor_instance.measured_speed, 9, 1);
			}
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2
			| RCC_PERIPHCLK_I2C1;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC_Init(void) {

	/* USER CODE BEGIN ADC_Init 0 */

	/* USER CODE END ADC_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC_Init 1 */

	/* USER CODE END ADC_Init 1 */

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc.Instance = ADC1;
	hadc.Init.OversamplingMode = ENABLE;
	hadc.Init.Oversample.Ratio = ADC_OVERSAMPLING_RATIO_256;
	hadc.Init.Oversample.RightBitShift = ADC_RIGHTBITSHIFT_8;
	hadc.Init.Oversample.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
	hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
	hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ContinuousConvMode = ENABLE;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.DMAContinuousRequests = ENABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc.Init.LowPowerAutoWait = DISABLE;
	hadc.Init.LowPowerFrequencyMode = DISABLE;
	hadc.Init.LowPowerAutoPowerOff = DISABLE;
	if (HAL_ADC_Init(&hadc) != HAL_OK) {
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel to be converted.
	 */
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC_Init 2 */

	/* USER CODE END ADC_Init 2 */

}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x00503D58;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}

	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE)
			!= HAL_OK) {
		Error_Handler();
	}

	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 1.6 - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 1000 - 1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_Encoder_InitTypeDef sConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 65535;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 15;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 15;
	if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIMEx_RemapConfig(&htim3, TIM3_TI1_GPIO) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */

}

/**
 * @brief TIM21 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM21_Init(void) {

	/* USER CODE BEGIN TIM21_Init 0 */

	/* USER CODE END TIM21_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM21_Init 1 */

	/* USER CODE END TIM21_Init 1 */
	htim21.Instance = TIM21;
	htim21.Init.Prescaler = 1600 - 1;
	htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim21.Init.Period = 1000 - 1;
	htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim21) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM21_Init 2 */

	/* USER CODE END TIM21_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void) {

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
 * @brief Callback do obsługi przerwania timera.
 *
 * Ta funkcja jest wywoływana, gdy upłynie okres timer'a htim21. Aktualizuje odczyt ADC, oblicza prędkość
 * silnika, przygotowuje dane do wysłania przez UART oraz zarządza flagami wysłania danych i odświeżania wyświetlacza LCD.
 * Przygotowanie danych do wysyłki w tym miejscu gwarantuje, że dane wysłane przez UART i wyświetlone na ekranie
 * pochodzą z tego samego punktu czasowego.
 *
 * @param htim Pointer do struktury TIM_HandleTypeDef zawierającej informacje o timerze.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == htim21.Instance) {
		if (flag_turn_on_off == 1) {

			/* Oblicz nową prędkość na podstawie odczytu z ADC, zakres <-150,150>*/
			new_speed = (adc_value[0] * 300) / 4095 - 150;
			motor_set_RPM_speed(&motor_instance, new_speed);

			/*Wysteruj nową prędkość*/
			motor_calculate_speed(&motor_instance);

			/*Przygotuj dane do wysłania przez UART*/
			snprintf(uart_buffer, sizeof(uart_buffer),
					"Set: %ld, Measured: %ld\r\n", motor_instance.set_speed,
					motor_instance.measured_speed);
			flag_send_data = 1;

			lcd_refresh_counter++;
			if (lcd_refresh_counter >= 5) { /*flaga odswiezania ekranu zglaszana co 5 przerwanie*/
				flag_refresh_LCD = 1;
				lcd_refresh_counter = 0;
			}

		}
	}

}

/**
 * @brief Callback do obsługi przerwania zewnętrznego (przycisk).
 *
 * Funkcja ta jest wywoływana po naciśnięciu przycisku (GPIO_Pin == B1_Pin).
 * Wykonuje debouncing oraz w zależności od stanu flagi włącza lub wyłącza silnik,
 * a także zarządza podświetleniem wyświetlacza LCD.
 *
 * @param GPIO_Pin Numer pinu, który wywołał przerwanie.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	/*po wcisnieciu przycisku*/
	if (GPIO_Pin == B1_Pin) {
		uint32_t current_time = HAL_GetTick();
		if (current_time - last_EXTI_interrupt_time > 50) { /*cyfrowy Debouncing 50 ms*/
			/*jesli flaga wlaczenia byla rowna 0 to zmiana na 1*/
			if (flag_turn_on_off == 0) {
				motor_update_count(&motor_instance); /*zeby zresetowac licznik po wznowieniu*/
				flag_turn_on_off = 1;
				I2C_LCD_Backlight(I2C_LCD_1);
				/*jesli flaga wlaczenia byla rowna 1 to zastopuj silnik*/
			} else if (flag_turn_on_off == 1) {
				I2C_LCD_NoBacklight(I2C_LCD_1);
				motor_stop(&motor_instance);
				flag_turn_on_off = 0;
			}
		}
		last_EXTI_interrupt_time = current_time;
	}

}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

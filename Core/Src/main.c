/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "gpio.h"
#include "i2c.h"
#include "rtc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "ssd1306_tests.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_DateTypeDef data;
RTC_TimeTypeDef tempo;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t segundos, minutos, horas, dia_semana, dia, mes, ano;
uint8_t horario_uv[] = {10, 12, 18, 20};  // lista formada por pares, sempre acrescentar uma hora pra ligar e uma pra desligar
uint8_t estado_uv = 0;
uint8_t horas_restantes, minutos_restantes, segundos_restantes;
char text_buffer[50];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
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
    MX_RTC_Init();
    MX_I2C2_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    ssd1306_Init();
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        HAL_RTC_GetTime(&hrtc, &tempo, RTC_FORMAT_BIN);
        segundos = tempo.Seconds;
        minutos = tempo.Minutes;
        horas = tempo.Hours;

        for (uint8_t i = 0; i < (sizeof(horario_uv) - 1); i += 2) {
            estado_uv = i;
            if (horas < horario_uv[i]) {
                break;
            } else if (horas < horario_uv[i + 1]) {
                estado_uv++;
                break;
            } else if (i == (sizeof(horario_uv) - 2)) {
                estado_uv = 0;
            }
        }
        HAL_GPIO_WritePin(rele1_GPIO_Port, rele1_Pin, !(estado_uv % 2));
        HAL_GPIO_WritePin(rele2_GPIO_Port, rele2_Pin, !(estado_uv % 2));

        uint8_t i = horas;
        horas_restantes = 0;
        while (i != horario_uv[estado_uv]) {
            i++;
            horas_restantes++;
            if (i >= 24) {
                i = 0;
            }
        }
        horas_restantes--;
        minutos_restantes = 59 - minutos;
        segundos_restantes = 60 - segundos;
        if (segundos_restantes == 60) {
            segundos_restantes = 0;
            minutos_restantes++;
        }
        if (minutos_restantes == 60) {
            minutos_restantes = 0;
            horas_restantes++;
        }

        ssd1306_Fill(Black);
        ssd1306_DrawRectangle(0, 0, 127, 15, White);
        ssd1306_DrawRectangle(0, 16, 127, 63, White);
        ssd1306_SetCursor(13, 3);
        sprintf(text_buffer, "Hora: %02d:%02d:%02d", horas, minutos, segundos);
        ssd1306_WriteString(text_buffer, Font_7x10, White);
        if (!(estado_uv % 2)) {
            ssd1306_SetCursor(12, 19);
            ssd1306_WriteString(" Liga em: ", Font_11x18, White);
        } else {
            ssd1306_SetCursor(6, 19);
            ssd1306_WriteString("Desliga em:", Font_11x18, White);
        }
        ssd1306_SetCursor(12, 43);
        sprintf(text_buffer, " %02d:%02d:%02d ", horas_restantes, minutos_restantes, segundos_restantes);
        ssd1306_WriteString(text_buffer, Font_11x18, White);
        ssd1306_UpdateScreen();
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

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

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

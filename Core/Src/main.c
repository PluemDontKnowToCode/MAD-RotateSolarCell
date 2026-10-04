/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "eth.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "ILI9341_Touchscreen.h"
#include "string.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "TeamIcon.h"
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

/* USER CODE BEGIN PV */
uint16_t fontColor;
uint16_t BgColor;
uint8_t uipage = 0x00;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ILI9341_Draw_Image_With_Pos(const uint16_t *img, uint16_t startX, uint16_t startY,
                                 uint16_t img_w, uint16_t img_h, uint16_t bgColor)
{
    ILI9341_Set_Address(startX, startY, startX + img_w - 1, startY + img_h - 1);

    uint32_t total_pixels = (uint32_t)img_w * img_h;
    for (uint32_t i = 0; i < total_pixels; i++)
    {
        uint16_t px = ((uint16_t)img[i * 2] << 8) | img[i * 2 + 1];

        if (px == 0x0000) px = bgColor;      // null = transparent

        ILI9341_Write_Data(px >> 8);         // high byte first
        ILI9341_Write_Data(px & 0xFF);
    }
}
//Color scale rgb 255 255 255
uint16_t mixedColor(uint8_t r, uint8_t g, uint8_t b)
{
	// Clamp inputs to valid 0-100 range just in case
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	// Scale 0-100 -> RGB565 bit ranges directly (5-bit R, 6-bit G, 5-bit B)
	uint16_t r5 = ((uint32_t)r * 31) / 255;  // 0-31
	uint16_t g6 = ((uint32_t)g * 63) / 255;  // 0-63
	uint16_t b5 = ((uint32_t)b * 31) / 255;  // 0-31

	return (r5 << 11) | (g6 << 5) | b5;
}
#define FONT_GAP_COLS 1   // blank columns at the right of each glyph (check your font)

float ILI9341_Text_Width(const char *Text, float Size)
{
    uint16_t n = strlen(Text);
    if (n == 0) return 0;
    return (n * 6 - FONT_GAP_COLS) * Size;
}

void StartUI()
{
	ILI9341_Fill_Screen(BgColor);
	ILI9341_Draw_Image_With_Pos(icon, 94, 10, 131, 145, BgColor);

	ILI9341_Draw_Text("Solar tracker dashboard", 27, 164, fontColor, 2, BgColor);

	uint32_t start = HAL_GetTick();

	while(HAL_GetTick() - start <= 2000)
	{
		HAL_Delay(20);
	}
	HAL_UART_Transmit(&huart3, (uint8_t*)"Change UI\n\r", 11, 200);
	uipage = 0x01;
}
void DashboardUI()
{
	ILI9341_Fill_Screen(BgColor);
	//45 13
	ILI9341_Draw_Text("Solar tracker dashboard", 45, 13, fontColor, 1.7f, BgColor);
	while(1)
	{
		//process
	}
}
void TrackingUI()
{

}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

	//Color initialize
	BgColor = mixedColor(0x18, 0x18,0x1B);
	fontColor = mixedColor(0xF1, 0xF5, 0xF9);
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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_RNG_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();//initial driver setup to drive ili9341
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);


  while (1)
  {

	  switch (uipage)
	  {
	  	  case 0x00:
	  		  StartUI();
	  		  break;
	  	  case 0x01:
	  		  DashboardUI();
	  		  break;
	  	  case 0x02:
	  		  TrackingUI();
	  		  break;
	  	  default:
	  		  StartUI();
	  		  break;
	  }
	  HAL_Delay(20);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  uint16_t flag = 0xffff;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

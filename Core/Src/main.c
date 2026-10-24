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
#include "adc.h"
#include "dma.h"
#include "eth.h"
#include "i2c.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <ldr.h>
#include <INA219.h>
#include "stdio.h"
#include "string.h"
#include "ILI9341_STM32_Driver.h"

#include "ui.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INA219_ADDRESS_1  INA219_ADDRESS  // ปกติคือ 0x40
#define INA219_ADDRESS_2  0x41
#define INA219_ADDRESS_3  0x44

// Should Check again for sure
// หน่วยเป็น Volt
#define batteryMax 12.6
#define batteryMin 9.00
// ถือว่า low เป็น 20% ที่ถ่านก้อนละ 3.58 V
#define batteryLow 10.74f

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//0x00 Start UI
//0x01 DashboardUI
//0x02 Control UI
uint8_t uipage = 0x00;

//Solar cell angle
uint16_t x = 0, y = 0;

//Solar Cell
//Mock data
uint16_t solarCellWatt = 10;
uint16_t solarCellAmpare = 10;
uint16_t solarCellVolt = 10;

//0
//1
uint8_t trackingModeState = 0;

//////////////////////BATTERY//////////////////////////
uint8_t batteryPercent = 0;
//0 Chg
//1 disChg
uint8_t batteryChargingState = 0;

//Show how much battery current capacity as range
//0 : 0 - 33 %
//1 : 34 - 67 %
//2 :: 68 - 100 %
uint8_t batterySizeState = 0;
///////////////////////////////////////////////////////
//Load
//Mock data
uint16_t loadWatt = 10;
uint16_t loadAmpare = 10;
uint16_t loadVolt = 10;


/////////////////  CHOOSE SOURCE ////////////////////////
//0 battery
//1 utility grid
uint8_t sourceState = 0;
/////////////////////////////////////////////////////////
/////////////////  INA219_sensor initial value  ////////////////////////
INA219_t ina219_solar;
INA219_t ina219_battery;
INA219_t ina219_load;

uint8_t ina219_address[3] = {INA219_ADDRESS_1, INA219_ADDRESS_2, INA219_ADDRESS_3};
INA219_t* ina219_list[3] = {&ina219_solar,&ina219_solar,&ina219_load};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void displayNumber(uint16_t num){
	char str[10];

	sprintf(str,"%d \r\n",num);
	HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), 1000);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void WakeI2C_signal(INA219_t* sensor, uint8_t address_sensor, uint8_t max_retry){
	uint8_t retry_count = 0;

	while(!INA219_Init(sensor, &hi2c1, address_sensor) && retry_count < max_retry - 1){
		retry_count++;
		HAL_Delay(100);
	}

	if(retry_count >= max_retry - 1) {
	    // print บอกว่า ไม่เจอ ไม่ก็แสดงผลบนจอ lcd
	    printf("Solar Sensor Error: Not Found!\n");
	} else {
	    printf("Solar Sensor Initialized.\n");
	}
}
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_RNG_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();//initial driver setup to drive ili9341
  My_Color_Init();



  // using Handle I2C 1 -> ใช้ I2C1 ของ board, PB6 for SCL and PB9 for SDA -> Address เดิม น่าจะ 0x40
  WakeI2C_signal(&ina219_solar, INA219_ADDRESS_1, 3);
  // using Handle I2C 1 เหมือนกัน address 0x41
  WakeI2C_signal(&ina219_battery, INA219_ADDRESS_2, 3);
  // using Handle I2C 1 เหมือนกัน address 0x42
  WakeI2C_signal(&ina219_load, INA219_ADDRESS_3, 3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
  StartUI_Init();

  while (1)
  {

	  switch (uipage)
	  {
	  	  case 0x01:
	  		  DashboardUI_Init();
	  		  break;
	  	  case 0x02:
	  		  TrackingUI_Init();
	  		  break;
	  	  default:
	  		  StartUI_Init();
	  		  break;
	  }

	  while(1)
	  {
		  //process
		  for (int i=0; i<3; i++){
		  		 // solar -> battery -> load
		  		  if (HAL_I2C_IsDeviceReady(&hi2c1, (ina219_address[i] << 1),3 ,2) == HAL_OK){
		  			  continue;
		  		  }
		  		  else {
		  			  WakeI2C_signal(ina219_list[i], ina219_address[i], 1);
		  			  printf("try to wake i2c sensor address : %x \n",ina219_address[i]);
		  		  }
		  	  }


		  	  // ==========  solar  ===========
		  	  // return milli 10^-3
		  	  float volt_s = INA219_ReadBusVoltage(&ina219_solar) / 1000.0;
		  	  float amp_s  = INA219_ReadCurrent(&ina219_solar) / 1000.0;
		  	  // choose 1 line
		  //	  float pow_s  = INA219_ReadPower(&ina219_solar) / 1000.0;
		  	  float pow_s  = INA219_GetAVGMiliWatt(&ina219_solar) / 1000.0;

		  	  // ==========  battery   ===========
		  	  // check battery state of charge -> return 0-100 % ?
		  	  // need to pass params in milli volt
		  	  float batt_soc = INA219_GetBatteryLife(&ina219_battery, batteryMax*1000, batteryMin*1000);

		  	  // for checking when battery is run out
		  	  if (INA219_HealthCheck(&ina219_battery, batteryLow, batt_soc) == Battery_LOW){
		  		  printf("Battery now is LOW : %f",batt_soc);
		  	  }
		  	  // don't know that if we need this statement, So I will write it down
		  	  else if(INA219_HealthCheck(&ina219_battery, batteryLow, batt_soc) == Battery_OK){
		  		  printf("Battery now is OK : %f",batt_soc);
		  	  }

		  	  // ==========  load   ===========
		  	  float volt_l = INA219_ReadBusVoltage(&ina219_load) / 1000.0;
		  	  float amp_l  = INA219_ReadCurrent(&ina219_load) / 1000.0;
		  //	  float pow_l  = INA219_ReadPower(&ina219_solar) / 1000.0;
		  	  float pow_l  = INA219_GetAVGMiliWatt(&ina219_load) / 1000.0;

		  	  // Find Watt ที่ใช้ตั้งแต่เปิดเครื่องมา
		  	  // return เป็น มิลลิวัตต์-มิลลิวินาที (mW·ms) เอาไป / 1000 ให้เป็น sec
		  	  float power_use = INA219_GetTotalPowerUsed(&ina219_load) / 1000.0;

		  	  // show value on terminal by using USART3
		  //	  displayNumber(pow_s);
		  //	  displayNumber(pow_l);

		  	  // MAP Variable from sensor to LCD
		  	  batteryPercent = (uint8_t)batt_soc;
		  	  float f_num[] = {volt_s, amp_s, pow_s, volt_l, amp_l, pow_l, power_use};
		  	  uint16_t lcd_num[6]; // now not use power_use
		  	  for (int i=0;i<6;i++){
		  		  lcd_num[i] = (uint16_t)(f_num[i] + 0.5f); //0.5 for >0.5 -> upper
		  	  }

		  	  solarCellVolt   = lcd_num[0];
		  	  solarCellAmpare = lcd_num[1];
		  	  solarCellWatt   = lcd_num[2];
		  	  loadVolt        = lcd_num[3];
		  	  loadAmpare      = lcd_num[4];
		  	  loadWatt        = lcd_num[5];
		  if (uipage == 0x01)
		  {
			  if(DashboardUI_Loop() == 1)
			  {
				  break;
			  }
		  }

		  else if(uipage == 0x02)
		  {
			  if(TrackingUI_Loop() == 1)
			  {

				  break;
			  }
		  }

		  HAL_Delay(50);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  uint16_t flag = 0xffff;

/////////////////  Read Current Sensor  ////////////////////////

	  // ตอนนี้ให้มัน check ทุก loop แล้วก็ลอง wake ตลอด


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

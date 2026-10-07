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
#include "ILI9341_Touchscreen.h"
#include "string.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "ILI9341_Myhelperfunction.h"

//image include
#include "TeamIcon.h"
#include "SolarCellIcon.h"
#include "Zap.h"
#include "battery.h"
#include "backButton.h"
#include "circleButton.h"
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
uint16_t SubBgColor;
uint16_t buttonColor;
uint16_t unButtonColor;
uint16_t circleButtonColor;


//0x00 Start UI
//0x01 DashboardUI
//0x02 Control UI
uint8_t uipage = 0x00;

uint8_t ui1InitState = 0;
uint8_t ui2InitState = 0;

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
uint8_t batteryState = 0;

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

char s_trackingDisplayBuffer[25] = "\0";
char s_posBuffer[25] = "\0";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void My_Color_Init()
{
	BgColor = mixedColor(0x18, 0x18,0x1B);
	fontColor = mixedColor(0xF1, 0xF5, 0xF9);
	SubBgColor = mixedColor(0x27, 0x27, 0x2A);
	buttonColor = mixedColor(0x9E, 0xF5, 0xCF);
	circleButtonColor = mixedColor(0x51, 0xDA, 0xCF);
	unButtonColor = mixedColor(0x4E, 0x4E, 0x4E);
}


void StartUI()
{
	ILI9341_Fill_Screen(BgColor);
	//draw image at 94 10 //W 131 //H 145

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
void ToggleChooseSource(Rectangle rect5_1, Rectangle rect5_2)
{
	if(sourceState == 0)
	{
		//use battery
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, buttonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, WHITE);
		ILI9341_Draw_Text("Battery", rect5_1.X0 + 3, rect5_1.Y0 + 19, BLACK, 1, buttonColor);

		//use utility
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, unButtonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, WHITE);
		ILI9341_Draw_Text("Utility", rect5_2.X0 + 9, rect5_2.Y0 + 11, fontColor, 0.8f, unButtonColor);
		ILI9341_Draw_Text("grid", rect5_2.X0 + 13, rect5_2.Y0 + 25, fontColor, 1.2f, unButtonColor);
	}
	else
	{
		//use battery
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, unButtonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, WHITE);
		ILI9341_Draw_Text("Battery", rect5_1.X0 + 3, rect5_1.Y0 + 19, fontColor, 1, unButtonColor);

		//use utility
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, buttonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, WHITE);
		ILI9341_Draw_Text("Utility", rect5_2.X0 + 9, rect5_2.Y0 + 11, BLACK, 0.8f, buttonColor);
		ILI9341_Draw_Text("grid", rect5_2.X0 + 13, rect5_2.Y0 + 25, BLACK, 1.2f, buttonColor);
	}
}

void UpdateBatteryUI(Rectangle rect)
{
	if(batteryPercent <= 33 && (batterySizeState != 0 || ui1InitState == 1))
	{
		ILI9341_Draw_Image_With_Pos(battery_low, rect.X0 + 40, rect.Y0 + 21, 42, 42, SubBgColor);
		batterySizeState = 0;
	}
	else if(batteryPercent <= 67 && batteryPercent >= 34 && (batterySizeState != 1 || ui1InitState == 1))
	{
		ILI9341_Draw_Image_With_Pos(battery_half, rect.X0 + 40, rect.Y0 + 21, 42, 42, SubBgColor);
		batterySizeState = 1;
	}
	else if(batteryPercent <= 100 && batteryPercent >= 68 && (batterySizeState != 2 || ui1InitState == 1))
	{
		ILI9341_Draw_Image_With_Pos(battery_high, rect.X0 + 40, rect.Y0 + 21, 42, 42, SubBgColor);
		batterySizeState = 2;
	}

	char temp[32];
	char* state = (batteryState == 0) ? "CHG" : "DCHG";
	sprintf(temp, "SOC: %u %% %s", batteryPercent, state);
	ILI9341_Draw_Text(temp, rect.X0 + 10, rect.Y0 + 64, fontColor, 1, SubBgColor);

	//Time left
	//How dafuck to calculate this


}
void DashboardUI()
{
	ui1InitState = 1;
	ILI9341_Fill_Screen(BgColor);
	//45 13
	ILI9341_Draw_Text("Solar tracker dashboard", 45, 13, fontColor, 1.7f, BgColor);

	//Solar power display
	Rectangle rect1 = {0, 47, 0 + 101, 47 + 100};
	ILI9341_Draw_Filled_Rectangle_Coord(rect1.X0, rect1.Y0, rect1.X1, rect1.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect1.X0, rect1.Y0, rect1.X1, rect1.Y1, BLACK);
	ILI9341_Draw_Text("Solar power", rect1.X0 + 7, rect1.Y0 + 5, fontColor, 1.2f, SubBgColor);
	ILI9341_Draw_Image_With_Pos(solar_cell_image, rect1.X0 + 28, rect1.Y0 + 22, 45, 36, SubBgColor);

	//Battery display
	Rectangle rect2 = {101, 47, 101 + 118, 47 + 100};
	ILI9341_Draw_Filled_Rectangle_Coord(rect2.X0, rect2.Y0, rect2.X1, rect2.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect2.X0, rect2.Y0, rect2.X1, rect2.Y1, BLACK);
	ILI9341_Draw_Text("Battery Left", rect2.X0 + 15, rect2.Y0 + 4, fontColor, 1.2f, SubBgColor);
	UpdateBatteryUI(rect2);

	//Load display
	Rectangle rect3 = {219, 47, 219 + 101, 47 + 100};
	ILI9341_Draw_Filled_Rectangle_Coord(rect3.X0, rect3.Y0, rect3.X1, rect3.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect3.X0, rect3.Y0, rect3.X1, rect3.Y1, BLACK);
	ILI9341_Draw_Text("LOAD", rect3.X0 + 34, rect3.Y0 + 5, fontColor, 1.2f, SubBgColor);
	ILI9341_Draw_Image_With_Pos(zap_image, rect3.X0 + 34, rect3.Y0 + 24, 32, 32, SubBgColor);

	//tracker button
	Rectangle rect4 = {0, 147, 0 + 160, 147 + 92};
	ILI9341_Draw_Filled_Rectangle_Coord(rect4.X0, rect4.Y0, rect4.X1, rect4.Y1, buttonColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect4.X0, rect4.Y0, rect4.X1, rect4.Y1, WHITE);
	ILI9341_Draw_Text("Tracking Control Mode", rect4.X0 + 5, rect4.Y0 + 12, BLACK, 1.2f, buttonColor);


	const char *modeText = (trackingModeState == 0) ? "Auto" : "Manual";
	sprintf(s_trackingDisplayBuffer, "tracking mode: %s", modeText);
	ILI9341_Draw_Text(s_trackingDisplayBuffer, rect4.X0 + 13, rect4.Y0 + 38, BLACK, 1, buttonColor);

	//choose source
	Rectangle rect5 = {160, 147, 160 + 160, 147 + 92};
	ILI9341_Draw_Filled_Rectangle_Coord(rect5.X0, rect5.Y0, rect5.X1, rect5.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect5.X0, rect5.Y0, rect5.X1, rect5.Y1, BLACK);
	ILI9341_Draw_Text("Choose Source", rect5.X0 + 19, rect5.Y0 + 3, fontColor, 1.2f, SubBgColor);

	Rectangle rect5_1 = {185, 172, 185 + 47, 172 + 50};
	Rectangle rect5_2 = {251, 172, 251 + 47, 172 + 50};
	ToggleChooseSource(rect5_1, rect5_2);
	ui1InitState = 0;
	char temp[50];
	while(1)
	{
		//process

		//Update value
		//Solar cell power
		sprintf(temp, "%u.%u W", solarCellWatt / 10, solarCellWatt % 10);
		ILI9341_Draw_Text(temp, rect1.X0 + 31, rect1.Y0 + 63, fontColor, 1, SubBgColor);

		sprintf(temp, "%d V / %d A", solarCellVolt, solarCellAmpare);
		ILI9341_Draw_Text(temp, rect1.X0 + 16, rect1.Y0 + 78, fontColor, 1, SubBgColor);

		//battery
		UpdateBatteryUI(rect2);

		//load
		sprintf(temp, "%u.%u W", loadWatt / 10, loadWatt % 10);
		ILI9341_Draw_Text(temp, rect3.X0 + 31, rect3.Y0 + 63, fontColor, 1, SubBgColor);

		sprintf(temp, "%d V / %d A", loadVolt, loadAmpare);
		ILI9341_Draw_Text(temp, rect3.X0 + 16, rect3.Y0 + 78, fontColor, 1, SubBgColor);

		//tracking mode dynamic
		sprintf(s_posBuffer, "X:%d  Y:%d", x, y);
		ILI9341_Draw_Text(s_posBuffer, rect4.X0 + 38, rect4.Y0 + 65, BLACK, 1.3f, buttonColor);

		if (TP_Touchpad_Pressed())
		{
			uint16_t x_pos = 0;
			uint16_t y_pos = 0;
			uint16_t position_array[2];

			if (TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
			{
				//Map X, Y
				x_pos = position_array[1];
				y_pos = ILI9341_WIDTH - position_array[0];

				char counter_buff[50];
				sprintf(counter_buff, "POS X: %.3d POS Y: %.3d\n\r", x_pos, y_pos);
				HAL_UART_Transmit(&huart3, (uint8_t*)counter_buff, strlen(counter_buff), 200);
				if(PointInRectangle(x_pos, y_pos, rect5_1) && sourceState == 0)
				{
					sourceState = 1;
					ToggleChooseSource(rect5_1, rect5_2);
				}
				else if(PointInRectangle(x_pos, y_pos, rect5_2) && sourceState == 1)
				{
					sourceState = 0;
					ToggleChooseSource(rect5_1, rect5_2);
				}
				else if(PointInRectangle(x_pos, y_pos, rect4))
				{
					uipage = 0x02;
					HAL_UART_Transmit(&huart3, (uint8_t*)"Change UI\n\r", 11, 200);
					break;
				}
			}
		}
//		batteryPercent++;
//		if(batteryPercent > 100)
//			batteryPercent = 0;
		HAL_Delay(50);
	}
}
void ToggleTrackMode(Rectangle rect5_1, Rectangle rect5_2)
{
	const char *modeText = (trackingModeState == 0) ? "Auto" : "Manual";
	uint8_t len = sprintf(s_trackingDisplayBuffer, "tracking mode: %s", modeText);
	HAL_UART_Transmit(&huart3, (uint8_t*)s_trackingDisplayBuffer, len, 200);
	ILI9341_Draw_Text(s_trackingDisplayBuffer, 191, 97, fontColor, 1, BgColor);
	if(trackingModeState == 0)
	{
		//auto
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, buttonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, WHITE);
		ILI9341_Draw_Text("Auto", rect5_1.X0 + 3, rect5_1.Y0 + 19, BLACK, 1, buttonColor);

		//manual
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, unButtonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, WHITE);
		ILI9341_Draw_Text("Manual", rect5_2.X0 + 3, rect5_2.Y0 + 19, fontColor, 0.8f, unButtonColor);
	}
	else
	{
		//auto
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, unButtonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, WHITE);
		ILI9341_Draw_Text("Auto", rect5_1.X0 + 3, rect5_1.Y0 + 19, fontColor, 1, unButtonColor);

		//manual
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, buttonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, WHITE);
		ILI9341_Draw_Text("Manual", rect5_2.X0 + 3, rect5_2.Y0 + 19, BLACK, 0.8f, buttonColor);
	}
}
void TrackingUI()
{
	ui2InitState = 1;
	ILI9341_Fill_Screen(BgColor);
	ILI9341_Draw_Text("Tracking Mode", 91, 8, fontColor, 1.7f, BgColor);

	//back button
	Rectangle backButtonHitBox = {0,0, 0 + 30, 0 + 30};
	ILI9341_Draw_Image_With_Pos(backButton, backButtonHitBox.X0, backButtonHitBox.Y0, 30, 30, BgColor);

	//power
	Rectangle powerDisplayArea = {222, 40, 222 + 59, 40 + 45};
	ILI9341_Draw_Filled_Rectangle_Coord(powerDisplayArea.X0, powerDisplayArea.Y0, powerDisplayArea.X1, powerDisplayArea.Y1, BLACK);
	ILI9341_Draw_Text("Power", powerDisplayArea.X0 + 7, powerDisplayArea.Y0, fontColor, 1, BLACK);

	//draw button
	//left
	Rectangle leftButton = {2, 108, 2 + 56, 108 + 56};

	ILI9341_Draw_Image_With_Pos(circleButton, leftButton.X0, leftButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("left", 20, 129, fontColor, 1, circleButtonColor);
	//right
	Rectangle rightButton = {118, 108, 118 + 56, 108 + 56};
	ILI9341_Draw_Image_With_Pos(circleButton, rightButton.X0, rightButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("right", 134,127, fontColor, 1, circleButtonColor);
	//up
	Rectangle upButton = {59 ,66 ,59 + 56, 66 + 56};
	ILI9341_Draw_Image_With_Pos(circleButton,upButton.X0, upButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("up", 79, 86, fontColor, 1, circleButtonColor);

	//down
	Rectangle downButton = {59, 155, 59 + 56, 155 + 56};
	ILI9341_Draw_Image_With_Pos(circleButton, downButton.X0, downButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("down", 73, 175, fontColor, 1, circleButtonColor);

	//Choose mode
	Rectangle rect5_1 = {198, 120, 198 + 47, 120 + 50};
	Rectangle rect5_2 = {260, 120, 260 + 47, 120 + 50};
	ToggleTrackMode(rect5_1, rect5_2);

	//position update
	Rectangle positionDisplay = {191, 195, 191 + 112, 195 + 32};
	ILI9341_Draw_Text("Solar Panel Position", positionDisplay.X0, positionDisplay.Y0, fontColor, 1, BgColor);

	ui2InitState = 0;

	char temp[50];
	while(1)
	{
		//track mode update
		if (TP_Touchpad_Pressed())
		{
			uint16_t x_pos = 0;
			uint16_t y_pos = 0;
			uint16_t position_array[2];

			if (TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
			{
				//Map X, Y
				x_pos = position_array[1];
				y_pos = ILI9341_WIDTH - position_array[0];

				char counter_buff[50];
				sprintf(counter_buff, "POS X: %.3d POS Y: %.3d\n\r", x_pos, y_pos);
				HAL_UART_Transmit(&huart3, (uint8_t*)counter_buff, strlen(counter_buff), 200);
				if(PointInRectangle(x_pos, y_pos, backButtonHitBox))
				{
					uipage = 0x01;
					HAL_UART_Transmit(&huart3, (uint8_t*)"Change UI\n\r", 11, 200);
					break;
				}
				else if(PointInRectangle(x_pos, y_pos, leftButton))
				{
					if(x > 0) x--;
					HAL_Delay(200);
					HAL_UART_Transmit(&huart3, (uint8_t*)"x--\n\r", 5, 200);
				}
				else if(PointInRectangle(x_pos, y_pos, rightButton))
				{
					x++;
					HAL_Delay(200);
					HAL_UART_Transmit(&huart3, (uint8_t*)"x++\n\r", 5, 200);
				}
				else if(PointInRectangle(x_pos, y_pos, upButton))
				{
					y++;
					HAL_Delay(200);
					HAL_UART_Transmit(&huart3, (uint8_t*)"y++\n\r", 5, 200);
				}
				else if(PointInRectangle(x_pos, y_pos, downButton))
				{
					if (y > 0) y--;
					HAL_Delay(200);
					HAL_UART_Transmit(&huart3, (uint8_t*)"y--\n\r", 5, 200);
				}
				else if(PointInRectangle(x_pos, y_pos, rect5_1) && trackingModeState == 0)
				{
					trackingModeState = 1;
					ToggleTrackMode(rect5_1, rect5_2);
				}
				else if(PointInRectangle(x_pos, y_pos, rect5_2) && trackingModeState == 1)
				{
					trackingModeState = 0;
					ToggleTrackMode(rect5_1, rect5_2);
				}
			}
		}
		//power update
		sprintf(temp, "%u.%u W", solarCellWatt / 10, solarCellWatt % 10);
		ILI9341_Draw_Text(temp, powerDisplayArea.X0 + 12, powerDisplayArea.Y0 + 26, fontColor, 1, BLACK);

		sprintf(s_posBuffer, "X:%d  Y:%d", x, y);
		ILI9341_Draw_Text(s_posBuffer, positionDisplay.X0 + 13, positionDisplay.Y0 + 17, fontColor, 1.3f, BgColor);
		HAL_Delay(20);
	}
}

/* USER CODE END 0 */

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
  /* USER CODE BEGIN 2 */
  ILI9341_Init();//initial driver setup to drive ili9341
  My_Color_Init();
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

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  uint16_t flag = 0xffff;
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

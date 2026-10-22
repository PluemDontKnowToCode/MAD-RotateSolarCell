/*
 * ui.c
 *
 *  Created on: 28 ก.ย. 2569
 *      Author: User
 */
#include "ILI9341_Myhelperfunction.h"
#include "ui.h"
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
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
#include "ui.h"

uint16_t fontColor;
uint16_t BgColor;
uint16_t SubBgColor;
uint16_t buttonColor;
uint16_t unButtonColor;
uint16_t circleButtonColor;

void My_Color_Init()
{
	BgColor = mixedColor(0x18, 0x18,0x1B);
	fontColor = mixedColor(0xF1, 0xF5, 0xF9);
	SubBgColor = mixedColor(0x27, 0x27, 0x2A);
	buttonColor = mixedColor(0x9E, 0xF5, 0xCF);
	circleButtonColor = mixedColor(0x51, 0xDA, 0xCF);
	unButtonColor = mixedColor(0x4E, 0x4E, 0x4E);
}
uint8_t ui1InitState = 0;
uint8_t ui2InitState = 0;

char s_trackingDisplayBuffer[25] = "\0";
char s_posBuffer[25] = "\0";
//torch screen position
uint16_t x_pos = 0;
uint16_t y_pos = 0;
uint16_t position_array[2];

void StartUI_Init()
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
		//Select battery
		//battery
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, buttonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, WHITE);
		ILI9341_Draw_Text("Battery", rect5_1.X0 + 3, rect5_1.Y0 + 19, BLACK, 1, buttonColor);

		//utility
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, unButtonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, WHITE);
		ILI9341_Draw_Text("Utility", rect5_2.X0 + 9, rect5_2.Y0 + 11, fontColor, 0.8f, unButtonColor);
		ILI9341_Draw_Text("grid", rect5_2.X0 + 13, rect5_2.Y0 + 25, fontColor, 1.2f, unButtonColor);
		HAL_UART_Transmit(&huart3, (uint8_t*)"Use battery\n\r", 13, 200);
	}
	else
	{
		//Select utility
		//battery
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, unButtonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_1.X0, rect5_1.Y0, rect5_1.X1, rect5_1.Y1, WHITE);
		ILI9341_Draw_Text("Battery", rect5_1.X0 + 3, rect5_1.Y0 + 19, fontColor, 1, unButtonColor);

		//utility
		ILI9341_Draw_Filled_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, buttonColor);
		ILI9341_Draw_Hollow_Rectangle_Coord(rect5_2.X0, rect5_2.Y0, rect5_2.X1, rect5_2.Y1, WHITE);
		ILI9341_Draw_Text("Utility", rect5_2.X0 + 9, rect5_2.Y0 + 11, BLACK, 0.8f, buttonColor);
		ILI9341_Draw_Text("grid", rect5_2.X0 + 13, rect5_2.Y0 + 25, BLACK, 1.2f, buttonColor);
		HAL_UART_Transmit(&huart3, (uint8_t*)"Use Utility\n\r", 13, 200);
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
	char* state = (batteryChargingState == 0) ? "CHG" : "DCHG";
	sprintf(temp, "SOC: %u %% %s", batteryPercent, state);
	ILI9341_Draw_Text(temp, rect.X0 + 10, rect.Y0 + 64, fontColor, 1, SubBgColor);

	//Time left
	//How dafuck to calculate this


}
Rectangle rect1 = {0, 47, 0 + 101, 47 + 100};

Rectangle rect2 = {101, 47, 101 + 118, 47 + 100};

Rectangle rect3 = {219, 47, 219 + 101, 47 + 100};

Rectangle rect4 = {0, 147, 0 + 160, 147 + 92};

Rectangle rect5 = {160, 147, 160 + 160, 147 + 92};

Rectangle rect5_1 = {185, 172, 185 + 47, 172 + 50};
Rectangle rect5_2 = {251, 172, 251 + 47, 172 + 50};
void DashboardUI_Init()
{
	ui1InitState = 1;
	ILI9341_Fill_Screen(BgColor);
	//45 13
	ILI9341_Draw_Text("Solar tracker dashboard", 45, 13, fontColor, 1.7f, BgColor);

	//Solar power display

	ILI9341_Draw_Filled_Rectangle_Coord(rect1.X0, rect1.Y0, rect1.X1, rect1.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect1.X0, rect1.Y0, rect1.X1, rect1.Y1, BLACK);
	ILI9341_Draw_Text("Solar power", rect1.X0 + 7, rect1.Y0 + 5, fontColor, 1.2f, SubBgColor);
	ILI9341_Draw_Image_With_Pos(solar_cell_image, rect1.X0 + 28, rect1.Y0 + 22, 45, 36, SubBgColor);

	//Battery display

	ILI9341_Draw_Filled_Rectangle_Coord(rect2.X0, rect2.Y0, rect2.X1, rect2.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect2.X0, rect2.Y0, rect2.X1, rect2.Y1, BLACK);
	ILI9341_Draw_Text("Battery Left", rect2.X0 + 15, rect2.Y0 + 4, fontColor, 1.2f, SubBgColor);
	UpdateBatteryUI(rect2);

	//Load display

	ILI9341_Draw_Filled_Rectangle_Coord(rect3.X0, rect3.Y0, rect3.X1, rect3.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect3.X0, rect3.Y0, rect3.X1, rect3.Y1, BLACK);
	ILI9341_Draw_Text("LOAD", rect3.X0 + 34, rect3.Y0 + 5, fontColor, 1.2f, SubBgColor);
	ILI9341_Draw_Image_With_Pos(zap_image, rect3.X0 + 34, rect3.Y0 + 24, 32, 32, SubBgColor);

	//tracker button

	ILI9341_Draw_Filled_Rectangle_Coord(rect4.X0, rect4.Y0, rect4.X1, rect4.Y1, buttonColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect4.X0, rect4.Y0, rect4.X1, rect4.Y1, WHITE);
	ILI9341_Draw_Text("Tracking Control Mode", rect4.X0 + 5, rect4.Y0 + 12, BLACK, 1.2f, buttonColor);


	const char *modeText = (trackingModeState == 0) ? "Auto" : "Manual";
	sprintf(s_trackingDisplayBuffer, "tracking mode: %s", modeText);
	ILI9341_Draw_Text(s_trackingDisplayBuffer, rect4.X0 + 13, rect4.Y0 + 38, BLACK, 1, buttonColor);

	//choose source

	ILI9341_Draw_Filled_Rectangle_Coord(rect5.X0, rect5.Y0, rect5.X1, rect5.Y1, SubBgColor);
	ILI9341_Draw_Hollow_Rectangle_Coord(rect5.X0, rect5.Y0, rect5.X1, rect5.Y1, BLACK);
	ILI9341_Draw_Text("Choose Source", rect5.X0 + 19, rect5.Y0 + 3, fontColor, 1.2f, SubBgColor);


	ToggleChooseSource(rect5_1, rect5_2);
	ui1InitState = 0;

}
uint8_t DashboardUI_Loop()
{
	char temp[50];
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
				return 1;
			}
		}
	}
//		batteryPercent++;
//		if(batteryPercent > 100)
//			batteryPercent = 0;
	HAL_Delay(50);
	return 0;
}
static void WaitForTouchRelease(void)
{
    while (TP_Touchpad_Pressed())
    {
        HAL_Delay(10);
    }

    HAL_Delay(30);  // debounce
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
Rectangle backButtonHitBox = {0,0, 0 + 30, 0 + 30};

Rectangle powerDisplayArea = {222, 40, 222 + 59, 40 + 45};

Rectangle leftButton = {2, 108, 2 + 56, 108 + 56};

Rectangle rightButton = {118, 108, 118 + 56, 108 + 56};

Rectangle upButton = {59 ,66 ,59 + 56, 66 + 56};

Rectangle downButton = {59, 155, 59 + 56, 155 + 56};

Rectangle positionDisplay = {191, 195, 191 + 112, 195 + 32};

Rectangle trackArea5_1 = {198, 120, 198 + 47, 120 + 50};
Rectangle trackArea5_2 = {260, 120, 260 + 47, 120 + 50};

void TrackingUI_Init()
{
	ui2InitState = 1;
	ILI9341_Fill_Screen(BgColor);
	ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
	ILI9341_Draw_Text("Tracking Mode", 91, 8, fontColor, 1.7f, BgColor);

	//back button
	ILI9341_Draw_Image_With_Pos(backButton, backButtonHitBox.X0, backButtonHitBox.Y0, 30, 30, BgColor);

	//power
	ILI9341_Draw_Filled_Rectangle_Coord(powerDisplayArea.X0, powerDisplayArea.Y0, powerDisplayArea.X1, powerDisplayArea.Y1, BLACK);
	ILI9341_Draw_Text("Power", powerDisplayArea.X0 + 7, powerDisplayArea.Y0, fontColor, 1, BLACK);

	//draw button
	//left
	ILI9341_Draw_Image_With_Pos(circleButton, leftButton.X0, leftButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("left", 20, 129, fontColor, 1, circleButtonColor);

	//right
	ILI9341_Draw_Image_With_Pos(circleButton, rightButton.X0, rightButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("right", 134,127, fontColor, 1, circleButtonColor);

	//up
	ILI9341_Draw_Image_With_Pos(circleButton,upButton.X0, upButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("up", 79, 86, fontColor, 1, circleButtonColor);

	//down
	ILI9341_Draw_Image_With_Pos(circleButton, downButton.X0, downButton.Y0, 56, 56, BgColor);
	ILI9341_Draw_Text("down", 73, 175, fontColor, 1, circleButtonColor);

	//Choose mode
	ToggleTrackMode(trackArea5_1, trackArea5_2);

	//position update
	ILI9341_Draw_Text("Solar Panel Position", positionDisplay.X0, positionDisplay.Y0, fontColor, 1, BgColor);

	ui2InitState = 0;
}

uint8_t TrackingUI_Loop()
{
	char temp[50];
	//track mode update
	if (TP_Touchpad_Pressed())
	{
		if (TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
		{
			//Map X, Y
			x_pos = position_array[1];
			y_pos = ILI9341_HEIGHT - position_array[0];

			char counter_buff[50];
			sprintf(counter_buff, "POS X: %.3d POS Y: %.3d\n\r", x_pos, y_pos);
			HAL_UART_Transmit(&huart3, (uint8_t*)counter_buff, strlen(counter_buff), 200);

//				ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
			if(PointInRectangle(x_pos, y_pos, backButtonHitBox))
			{
				uipage = 0x01;
				HAL_UART_Transmit(&huart3, (uint8_t*)"Change UI\n\r", 11, 200);
				return 1;

			}
			else if(PointInRectangle(x_pos, y_pos, leftButton))
			{
				if(x > 0) x--;
				HAL_UART_Transmit(&huart3, (uint8_t*)"x--\n\r", 5, 200);
				WaitForTouchRelease();
			}
			else if(PointInRectangle(x_pos, y_pos, rightButton))
			{
				if(x < X_POS_LIMIT) x++;
				HAL_UART_Transmit(&huart3, (uint8_t*)"x++\n\r", 5, 200);
				WaitForTouchRelease();
			}
			else if(PointInRectangle(x_pos, y_pos, upButton))
			{
				if(y < Y_POS_LIMIT) y++;
				HAL_UART_Transmit(&huart3, (uint8_t*)"y++\n\r", 5, 200);
				WaitForTouchRelease();
			}
			else if(PointInRectangle(x_pos, y_pos, downButton))
			{
				if (y > 0) y--;
				HAL_UART_Transmit(&huart3, (uint8_t*)"y--\n\r", 5, 200);
				WaitForTouchRelease();
			}
			else if(PointInRectangle(x_pos, y_pos, trackArea5_1) && trackingModeState == 0)
			{
				trackingModeState = 1;
				ToggleTrackMode(trackArea5_1, trackArea5_2);
			}
			else if(PointInRectangle(x_pos, y_pos, trackArea5_2) && trackingModeState == 1)
			{
				trackingModeState = 0;
				ToggleTrackMode(trackArea5_1, trackArea5_2);
			}
		}
	}

	//power update
	sprintf(temp, "%u.%u W", solarCellWatt / 10, solarCellWatt % 10);
	ILI9341_Draw_Text(temp, powerDisplayArea.X0 + 12, powerDisplayArea.Y0 + 26, fontColor, 1, BLACK);

	sprintf(s_posBuffer, "X:%d  Y:%d", x, y);
	ILI9341_Draw_Text(s_posBuffer, positionDisplay.X0 + 13, positionDisplay.Y0 + 17, fontColor, 1.3f, BgColor);
	return 0;
}



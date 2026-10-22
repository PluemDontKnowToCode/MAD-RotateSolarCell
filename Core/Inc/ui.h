/*
 * ui.h
 *
 *  Created on: 28 ก.ย. 2569
 *      Author: User
 */

#ifndef INC_UI_H_
#define INC_UI_H_

#include "main.h"

extern uint16_t fontColor;
extern uint16_t BgColor;
extern uint16_t SubBgColor;
extern uint16_t buttonColor;
extern uint16_t unButtonColor;
extern uint16_t circleButtonColor;

extern uint8_t uipage;

//Solar cell angle
extern uint16_t x, y;

//Solar Cell
//Mock data
extern uint16_t solarCellWatt;
extern uint16_t solarCellAmpare;
extern uint16_t solarCellVolt;

//0
//1
extern uint8_t trackingModeState;

//////////////////////BATTERY//////////////////////////
extern uint8_t batteryPercent;
//0 Chg
//1 disChg
extern uint8_t batteryChargingState;

//Show how much battery current capacity as range
//0 : 0 - 33 %
//1 : 34 - 67 %
//2 :: 68 - 100 %
extern uint8_t batterySizeState;
///////////////////////////////////////////////////////
//Load
//Mock data
extern uint16_t loadWatt;
extern uint16_t loadAmpare;
extern uint16_t loadVolt;


/////////////////  CHOOSE SOURCE ////////////////////////
//0 battery
//1 utility grid
extern uint8_t sourceState;



void My_Color_Init();
void StartUI_Init();
void DashboardUI_Init();
uint8_t DashboardUI_Loop();
void TrackingUI_Init();
uint8_t TrackingUI_Loop();
#endif /* INC_UI_H_ */

#include "tim.h"
#include "usart.h"
#include "stdio.h"

void servo_write_rotate(float angle, uint8_t channel)
{
	if(angle <= 0.1f) angle = 0.2f;
	//	f = 50 Hz, T = 20 ms
	// คำนวณหาค่า Pulse เป็น float เพื่อความแม่นยำสูง
	// 0 องศา = 1000 ticks (1ms), 180 องศา = 2000 ticks (2ms)
	// ช่วงต่างของ Pulse คือ 1000 ticks สำหรับ 180 องศา (คิดเป็น 1000/180 = 5.55 ticks ต่อ 1 องศา)
	uint32_t channels[2] = { TIM_CHANNEL_4, TIM_CHANNEL_3 }; // this is PB11, PB10
	float pulse_float = 173.0f+(angle*(1000.0f/180.0f));
	__HAL_TIM_SET_COMPARE(&htim2, channels[channel], (uint32_t)pulse_float);
}

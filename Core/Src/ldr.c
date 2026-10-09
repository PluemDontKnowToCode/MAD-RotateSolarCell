#include <ldr.h>
extern ADC_HandleTypeDef hadc1;

const uint16_t LDR_PINS[4] = { GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6 };
const uint8_t LDR_AMOUNT = 4;
volatile uint8_t ldr_cplt_flag = 0;


void LDR_start_read(uint32_t *dest)
{
	HAL_ADC_Start_DMA(&hadc1, dest, LDR_AMOUNT);
}

void LDR_stop_read()
{
	HAL_ADC_Stop_DMA(&hadc1);
	ldr_cplt_flag = 0;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if(hadc1.Instance != hadc->Instance) return;
	ldr_cplt_flag = 1;
}

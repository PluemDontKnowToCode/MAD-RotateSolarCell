#include <ldr.h>

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
	if(*hadc != hadc1) return ;
	ldr_cplt_flag = 1;
}

#include <ldr.h>

void LDR_READ(uint32_t *dest)
{
	HAL_ADC_Start_DMA(&hadc1, dest, LDR_AMOUNT);
}

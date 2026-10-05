#ifndef LDR_G
#define LDR_G

#include <gpio.h>

const GPIO_TypeDef* LDR_GPIO[4] = { GPIOA, GPIOA, GPIOA, GPIOA };
const uint16_t LDR_PINS[4] = { GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6 };
const uint8_t LDR_AMOUNT = 4;
uint8_t ldr_cplt_flag = 0;

void LDR_start_read(uint32_t *dest);
void LDR_stop_read();

#endif

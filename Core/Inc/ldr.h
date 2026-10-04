#ifndef LDR_G
#define LDR_G

#include <gpio.h>

const uint16_t LDR_PINS[4] = { GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6 };
const uint8_t LDR_AMOUNT = 4;

void LDR_READ(uint32_t *dest);

#endif

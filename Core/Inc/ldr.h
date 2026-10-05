#ifndef LDR_G
#define LDR_G

#include <gpio.h>


// ย้ายไป define ใน ldr.c แทนนะ ตรงนี้ใช้ extern ไปก่อน พอดีมัน build ไม่ผ่านอะ
//const uint16_t LDR_PINS[4] = { GPIO_PIN_0, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6 };
//const uint8_t LDR_AMOUNT = 4;

extern const uint16_t LDR_PINS[4];
extern const uint8_t LDR_AMOUNT;

void LDR_READ(uint32_t *dest);

#endif

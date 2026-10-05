#include <ILI9341_Myhelperfunction.h>
#include "main.h"
#include "spi.h"
#include "ILI9341_STM32_Driver.h"



uint8_t PointInCircle(uint16_t px, uint16_t py, uint16_t cx, uint16_t cy, uint16_t radius)
{
    int32_t dx = (int32_t)px - (int32_t)cx;
    int32_t dy = (int32_t)py - (int32_t)cy;
    return (dx * dx + dy * dy) <= (int32_t)(radius * radius);
}
uint8_t PointInRectangle(uint16_t px, uint16_t py, Rectangle rect)
{
	if((px > rect.X0) && (px < rect.X1) && (py > rect.Y0) && (py < rect.Y1))
		return 1;
	else
		return 0;
}
void ILI9341_Draw_Image_With_Pos(const uint8_t *img, uint16_t startX, uint16_t startY,
                                 uint16_t img_w, uint16_t img_h, uint16_t bgColor)
{
	static uint8_t rowBuf[320 * 2];          // one row, max screen width
	uint8_t bgHi = bgColor >> 8, bgLo = bgColor & 0xFF;

	ILI9341_Set_Address(startX, startY, startX + img_w - 1, startY + img_h - 1);

	HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN, GPIO_PIN_SET);     // data mode
	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_RESET);   // CS low once

	for (uint16_t y = 0; y < img_h; y++)
	{
		const uint8_t *src = img + (uint32_t)y * img_w * 2;
		for (uint16_t x = 0; x < img_w; x++)
		{
			uint8_t hi = src[x * 2], lo = src[x * 2 + 1];
			if (hi == 0 && lo == 0) { hi = bgHi; lo = bgLo; }     // transparent
			rowBuf[x * 2]     = hi;
			rowBuf[x * 2 + 1] = lo;
		}
		HAL_SPI_Transmit(&hspi5, rowBuf, img_w * 2, 100);
	}

	HAL_GPIO_WritePin(LCD_CS_PORT, LCD_CS_PIN, GPIO_PIN_SET);     // CS high once
}


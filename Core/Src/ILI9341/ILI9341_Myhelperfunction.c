#include <ILI9341_Myhelperfunction.h>
#include "main.h"
#include "spi.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"

//Color scale rgb 255 255 255
uint16_t mixedColor(uint8_t r, uint8_t g, uint8_t b)
{
	// Clamp inputs to valid 0-100 range just in case
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;

	// Scale 0-100 -> RGB565 bit ranges directly (5-bit R, 6-bit G, 5-bit B)
	uint16_t r5 = ((uint32_t)r * 31) / 255;  // 0-31
	uint16_t g6 = ((uint32_t)g * 63) / 255;  // 0-63
	uint16_t b5 = ((uint32_t)b * 31) / 255;  // 0-31

	return (r5 << 11) | (g6 << 5) | b5;
}
uint8_t PointInCircle(uint16_t px, uint16_t py, Circle c)
{
    int32_t dx = (int32_t)px - (int32_t)c.X;
    int32_t dy = (int32_t)py - (int32_t)c.Y;
    return (dx * dx + dy * dy) <= (int32_t)(c.radius * c.radius);
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


static uint16_t ILI9341_Min_U16(uint16_t a, uint16_t b)
{
    return (a < b) ? a : b;
}

static uint16_t ILI9341_Max_U16(uint16_t a, uint16_t b)
{
    return (a > b) ? a : b;
}

/* Draw an outline rounded rectangle between two corner coordinates. */
void ILI9341_Draw_Hollow_Round_Rectangle_Coord(
    uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1,
    uint16_t Colour, uint16_t Radius)
{
    uint16_t left   = ILI9341_Min_U16(X0, X1);
    uint16_t right  = ILI9341_Max_U16(X0, X1);
    uint16_t top    = ILI9341_Min_U16(Y0, Y1);
    uint16_t bottom = ILI9341_Max_U16(Y0, Y1);

    uint16_t maxRadius = (right - left < bottom - top)
                       ? (right - left) / 2
                       : (bottom - top) / 2;

    if (Radius > maxRadius) Radius = maxRadius;

    if (Radius == 0)
    {
        ILI9341_Draw_Hollow_Rectangle_Coord(left, top, right, bottom, Colour);
        return;
    }

    /* Straight edges between the four corner arcs. */
    ILI9341_Draw_Horizontal_Line(left + Radius, top,
                                 right - left - 2 * Radius, Colour);
    ILI9341_Draw_Horizontal_Line(left + Radius, bottom,
                                 right - left - 2 * Radius, Colour);
    ILI9341_Draw_Vertical_Line(left, top + Radius,
                               bottom - top - 2 * Radius, Colour);
    ILI9341_Draw_Vertical_Line(right, top + Radius,
                               bottom - top - 2 * Radius, Colour);

    /* Midpoint-circle points, restricted to each corner. */
    int32_t x = (int32_t)Radius;
    int32_t y = 0;
    int32_t error = 1 - x;

    while (x >= y)
    {
        ILI9341_Draw_Pixel(left + Radius - x,  top + Radius - y, Colour);
        ILI9341_Draw_Pixel(left + Radius - y,  top + Radius - x, Colour);
        ILI9341_Draw_Pixel(right - Radius + x, top + Radius - y, Colour);
        ILI9341_Draw_Pixel(right - Radius + y, top + Radius - x, Colour);

        ILI9341_Draw_Pixel(left + Radius - x,  bottom - Radius + y, Colour);
        ILI9341_Draw_Pixel(left + Radius - y,  bottom - Radius + x, Colour);
        ILI9341_Draw_Pixel(right - Radius + x, bottom - Radius + y, Colour);
        ILI9341_Draw_Pixel(right - Radius + y, bottom - Radius + x, Colour);

        y++;
        if (error < 0)
            error += 2 * y + 1;
        else
        {
            x--;
            error += 2 * (y - x) + 1;
        }
    }
}

/* Draw a filled rounded rectangle between two corner coordinates. */
void ILI9341_Draw_Filled_Round_Rectangle_Coord(
    uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1,
    uint16_t Colour, uint16_t Radius)
{
    uint16_t left   = ILI9341_Min_U16(X0, X1);
    uint16_t right  = ILI9341_Max_U16(X0, X1);
    uint16_t top    = ILI9341_Min_U16(Y0, Y1);
    uint16_t bottom = ILI9341_Max_U16(Y0, Y1);

    uint16_t maxRadius = (right - left < bottom - top)
                       ? (right - left) / 2
                       : (bottom - top) / 2;

    if (Radius > maxRadius) Radius = maxRadius;

    if (Radius == 0)
    {
        ILI9341_Draw_Filled_Rectangle_Coord(left, top, right, bottom, Colour);
        return;
    }

    /* Fill the center bands and the four rounded corners. */
    ILI9341_Draw_Filled_Rectangle_Coord(left + Radius, top,
                                        right - Radius, bottom, Colour);
    ILI9341_Draw_Filled_Rectangle_Coord(left, top + Radius,
                                        right, bottom - Radius, Colour);

    ILI9341_Draw_Filled_Circle(left + Radius,  top + Radius,    Radius, Colour);
    ILI9341_Draw_Filled_Circle(right - Radius, top + Radius,    Radius, Colour);
    ILI9341_Draw_Filled_Circle(left + Radius,  bottom - Radius, Radius, Colour);
    ILI9341_Draw_Filled_Circle(right - Radius, bottom - Radius, Radius, Colour);
}


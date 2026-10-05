#ifndef ILI9341_Myhelperfunction_H
#define ILI9341_Myhelperfunction_H
#include <stdint.h>

#define ILI9341_WIDTH 320
#define ILI9341_HEIGHT 240
typedef struct
{
	uint16_t X0;
	uint16_t Y0;
	uint16_t X1;
	uint16_t Y1;
} Rectangle;

/* Convert 8-bit R, G, B (0-255) to RGB565 */
uint16_t mixedColor(uint8_t r, uint8_t g, uint8_t b);
 
/* Returns 1 if point (px, py) is inside the circle centred at (cx, cy) */
uint8_t PointInCircle(uint16_t px, uint16_t py, uint16_t cx, uint16_t cy, uint16_t radius);
 
/* Returns 1 if point (px, py) is strictly inside the rectangle */
uint8_t PointInRectangle(uint16_t px, uint16_t py,
                         Rectangle rect);
 
/* Draw an RGB565 image stored as bytes (high byte first).
   Pixels equal to 0x0000 are treated as transparent and replaced by bgColor. */
void ILI9341_Draw_Image_With_Pos(const uint8_t *img, uint16_t startX, uint16_t startY,
                                 uint16_t img_w, uint16_t img_h, uint16_t bgColor);
#endif

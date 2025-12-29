#include <stdint.h>
#include "vga.h"

/* Base address in your system (same as your original) */
#define VGA_BASE ((volatile uint8_t*) 0x08000000)

static volatile uint8_t *screenbuffer = (volatile uint8_t *)VGA_BASE;

/* Fill the whole screen buffer with a color.
   Note: used primarily to initialize the background once. */
void vga_clear(uint8_t color) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        screenbuffer[i] = color;
    }
}

/* Draw a single pixel with safety clipping. */
void vga_draw_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    screenbuffer[y * SCREEN_WIDTH + x] = color;
}

/* Draw a filled rectangle. Bound check internally using vga_draw_pixel. */
void vga_draw_rect(int x, int y, int w, int h, uint8_t color) {
    /* Clip rectangle to screen bounds for safety */
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w;
    int y1 = y + h;
    if (x1 > SCREEN_WIDTH)  x1 = SCREEN_WIDTH;
    if (y1 > SCREEN_HEIGHT) y1 = SCREEN_HEIGHT;

    for (int yy = y0; yy < y1; yy++) {
        int base = yy * SCREEN_WIDTH;
        for (int xx = x0; xx < x1; xx++) {
            screenbuffer[base + xx] = color;
        }
    }
}

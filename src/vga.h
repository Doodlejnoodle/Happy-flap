#ifndef VGA_H
#define VGA_H

#include <stdint.h>

/* Screen size - match your hardware */
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

/* Color constants (match your existing palette) */
#define COLOR_SKY   0x29  /* background (was called COLOR_BLACK in original) */
#define COLOR_WHITE 0xFF
#define COLOR_BIRD  0xFC
#define COLOR_PIPE  0x1D

/* Basic drawing API used by the game code */
void vga_clear(uint8_t color);
void vga_draw_pixel(int x, int y, uint8_t color);
void vga_draw_rect(int x, int y, int w, int h, uint8_t color);

/* Flappy game API (these are called from labmain.c) */
void flappy_init(void);
void flappy_update(int jump_pressed);
int  flappy_check_collision(void);
int  flappy_get_score(void);

#endif /* VGA_H */

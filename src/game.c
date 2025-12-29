#include <stdint.h>
#include "vga.h"

/* ====== Gameplay constants ====== */
#define BIRD_SIZE   12
#define PIPE_WIDTH  20
#define PIPE_GAP    80      /* wider gap for easier play */
#define PIPE_SPEED  1       /* slower pipe movement */
#define GRAVITY     0.3f    /* softer gravity */
#define JUMP_SPEED -4.0f    /* smaller upward jump */
#define GROUND_Y    (SCREEN_HEIGHT - 16)

/* ====== Lightweight pseudo-random generator ====== */
static uint32_t prng_state = 1;
static uint32_t rand_fast(void) {
    prng_state = prng_state * 1103515245 + 12345;
    return (prng_state >> 16) & 0x7FFF;
}
static void srand_fast(uint32_t seed) {
    prng_state = seed ? seed : 1;
}

/* ====== Game state ====== */
static int bird_x, bird_y;
static int old_bird_x, old_bird_y;
static float velocity_f;

static int pipe_x, pipe_gap_y;
static int old_pipe_x;

static int score;
static int game_over;

/* ====== Helpers ====== */
static void draw_bird_at(int x, int y) {
    vga_draw_rect(x, y, BIRD_SIZE, BIRD_SIZE, COLOR_BIRD);
}

static void erase_bird_at(int x, int y) {
    vga_draw_rect(x, y, BIRD_SIZE, BIRD_SIZE, COLOR_SKY);
}

static void draw_pipe_at(int x, int gap_y) {
    if (gap_y > 0)
        vga_draw_rect(x, 0, PIPE_WIDTH, gap_y, COLOR_PIPE);
    int bottom_y = gap_y + PIPE_GAP;
    if (bottom_y < SCREEN_HEIGHT)
        vga_draw_rect(x, bottom_y, PIPE_WIDTH, SCREEN_HEIGHT - bottom_y, COLOR_PIPE);
}

static void erase_pipe_at(int x, int gap_y) {
    if (gap_y > 0)
        vga_draw_rect(x, 0, PIPE_WIDTH, gap_y, COLOR_SKY);
    int bottom_y = gap_y + PIPE_GAP;
    if (bottom_y < SCREEN_HEIGHT)
        vga_draw_rect(x, bottom_y, PIPE_WIDTH, SCREEN_HEIGHT - bottom_y, COLOR_SKY);
}

/* ====== Initialization ====== */
void flappy_init(void) {
    srand_fast(1); // deterministic seed for now

    vga_clear(COLOR_SKY);

    bird_x = 80;
    bird_y = SCREEN_HEIGHT / 2 - BIRD_SIZE / 2;
    old_bird_x = bird_x;
    old_bird_y = bird_y;
    velocity_f = 0;

    pipe_x = SCREEN_WIDTH;
    pipe_gap_y = 60 + (rand_fast() % (SCREEN_HEIGHT - 120));
    old_pipe_x = pipe_x;

    score = 0;
    game_over = 0;

    draw_pipe_at(pipe_x, pipe_gap_y);
    draw_bird_at(bird_x, bird_y);
}

/* ====== Game update ====== */
void flappy_update(int jump_pressed) {
    if (game_over) {
        if (jump_pressed) {
            flappy_init();
        }
        return;
    }

    old_bird_x = bird_x;
    old_bird_y = bird_y;
    old_pipe_x = pipe_x;

    if (jump_pressed)
        velocity_f = JUMP_SPEED;

    velocity_f += GRAVITY;
    bird_y += (int)velocity_f;

    if (bird_y < 0) {
        bird_y = 0;
        velocity_f = 0;
    }
    if (bird_y + BIRD_SIZE > GROUND_Y) {
        bird_y = GROUND_Y - BIRD_SIZE;
        velocity_f = 0;
        game_over = 1;
    }

    pipe_x -= PIPE_SPEED;
    if (pipe_x + PIPE_WIDTH < 0) {
        pipe_x = SCREEN_WIDTH;
        pipe_gap_y = 40 + (rand_fast() % (SCREEN_HEIGHT - 120));
        score++;
    }

    /* ---- Partial redraw ---- */
    erase_bird_at(old_bird_x, old_bird_y);
    erase_pipe_at(old_pipe_x, pipe_gap_y);

    draw_pipe_at(pipe_x, pipe_gap_y);
    draw_bird_at(bird_x, bird_y);

    /* ---- Collision check ---- */
    int bx0 = bird_x, bx1 = bird_x + BIRD_SIZE;
    int by0 = bird_y, by1 = bird_y + BIRD_SIZE;

    int px0 = pipe_x, px1 = pipe_x + PIPE_WIDTH;
    int top_by1 = pipe_gap_y;
    int bottom_by0 = pipe_gap_y + PIPE_GAP;

    if (!(bx1 <= px0 || bx0 >= px1)) {
        if (by0 < top_by1 || by1 > bottom_by0)
            game_over = 1;
    }
}

/* ====== Status ====== */
int flappy_check_collision(void) {
    return game_over;
}

int flappy_get_score(void) {
    return score;
}

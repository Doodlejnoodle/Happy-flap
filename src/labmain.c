/* labmain.c – Kombinerad version med timer-driven 7-seg + LEDs + prime-utskrift */

#include <stdint.h>

//extra
#include "vga.h"
#include "game.h"

extern void print(const char *s);
//extern void print_dec(unsigned int n);
//extern int nextprime(int);
extern void enable_interrupt(void);
extern void delay(int);

/* Device-registers (memory mapped) */
#define LEDS           ((volatile int*)0x04000000)
#define SWITCHES       ((volatile int*)0x04000010)
#define TMR1_STAT      ((volatile unsigned short*)0x04000020)
#define TMR1_CTRL      ((volatile unsigned short*)0x04000024)
#define TMR1_PERIODL   ((volatile unsigned short*)0x04000028)
#define TMR1_PERIODH   ((volatile unsigned short*)0x0400002C)
#define DISPLAY_BASE   0x04000050
#define DISPLAY_OFFSET 0x10


#define BTN2           ((volatile int*)0x040000d0)


//EXTRA
#define BUTTON_INTERRUPT    ((volatile unsigned short*)0x040000d8)
#define BUTTON_EDGE         ((volatile unsigned short*)0x040000dc)

/* --- Seven-seg encoding (0–9). 0 = segment on for this HW mapping) --- */
static const int segm_encoding[10] = {
    0b11000000, 0b11111001, 0b10100100, 0b10110000,
    0b10011001, 0b10010010, 0b10000010, 0b11111000,
    0b10000000, 0b10010000
};

/* --- Globals shared between IRQ and main --- */
volatile int seconds = 0;
volatile int minutes = 0;
volatile int hours   = 0;

/* startup LED sequence */
volatile int led_value = 0;       // value shown on first 4 LEDs during start
volatile int led_sequence_done = 0;

/* timer tick counting (0.1s ticks -> count 10 -> ~1s) */
volatile int timeoutcount = 0;

/* Prime state */
//int prime = 1234567;

/* ===== I/O helper functions (assignment parts c–g) ===== */

void set_leds(int led_mask) {
    /* Only 10 LSB are used on board */
    *LEDS = led_mask & 0x3FF;
}

void set_displays(int display_number, int value) {
    if (display_number < 0 || display_number > 5) return;
    volatile int *addr = (volatile int *)(DISPLAY_BASE + display_number * DISPLAY_OFFSET);
    *addr = value;
}

int get_sw(void) {
    return *SWITCHES & 0x3FF; /* 10 LSB from switches */
}

int get_btn(void) {
    return *BTN2 & 0x1; /* least significant bit = second button */
}

/* Convenience: put a decimal digit (0-9) onto a 7-seg display_number */
void display_digit_seg(int display_number, int digit) {
    if (digit < 0 || digit > 9) digit = 0;
    set_displays(display_number, segm_encoding[digit]);
}

/* Show current time on all 6 displays: leftmost = hours (tens, ones),
   middle = minutes, rightmost = seconds (two digits) */
void update_all_displays_from_time(int score) {
    int s = score;
    for (int i = 0; i < 6; i++) {
        int digit = s % 10;
        display_digit_seg(i, digit);
        s /= 10;
    }
}

/* ===== Timer init (labinit) ===== */
void labinit(void) {
    /* Setup period for ~0.1s ticks assuming 30 MHz clock:
       period = (30_000_000 / 10) - 1 */
    unsigned int period = (30000000 / 10) - 1;
    *TMR1_PERIODL = (unsigned short)(period & 0xFFFF);
    *TMR1_PERIODH = (unsigned short)(period >> 16);

    /* start timer + enable interrupt (bits as before) */
    *TMR1_CTRL = 0x7;

    *BUTTON_INTERRUPT = 0x1; //enable switch NUMBER 3

    /* enable global interrupts in CPU */
    enable_interrupt();
}

/* ===== Interrupt handler (timer-driven) =====
   Expected to be called from interrupt vector with 'cause' argument. */
void handle_interrupt(unsigned cause) {
    if (cause == 18){
        int button = get_btn() >> 2 & 0x1;
        unsigned short edge = *BUTTON_EDGE;

        if(edge){
            flappy_update(button);
            //add code here


            *BUTTON_EDGE = button;       

            
        } 
    }else if(cause == 16){    
        /* Clear timer flag if set */
        if (*TMR1_STAT & 0x1) {
            *TMR1_STAT = 0;
        }

        timeoutcount++;
        if (timeoutcount >= 10) { /* ~1 second elapsed */
            timeoutcount = 0;
        
        } 
    }
}




/* ===== Main: init, prime loop + button-handling to set time ===== */
int main(void) {
    /* Initialize timer + interrupts and clear outputs */
    labinit();
    flappy_init();
    print("Welcome to happy flap");
    
    while (1) {
        int jump = get_btn();
        flappy_update(jump);
        delay(60);

        int score = flappy_get_score();
        update_all_displays_from_time(score);

        if (flappy_check_collision()) {
            print("Game Over\n");
            delay(500);
            flappy_init();
        }
        delay(50);
    }

    return 0;
}

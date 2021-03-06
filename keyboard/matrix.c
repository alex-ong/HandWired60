#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "print.h"
#include "debug.h"
#include "util.h"
#include "matrix.h"
#include "debounce_matrix.h"

//#define BENCHMARK_MATRIX
#ifdef BENCHMARK_MATRIX
    #include "timer.h"
#endif


/* matrix state(1:on, 0:off) */
static matrix_row_t matrix[MATRIX_ROWS];
static matrix_row_t raw_matrix[MATRIX_ROWS];

static matrix_row_t read_cols(void);
static void init_cols(void);
static void unselect_rows(void);
static void select_row(uint8_t row);

inline
uint8_t matrix_rows(void)
{
    return MATRIX_ROWS;
}

inline
uint8_t matrix_cols(void)
{
    return MATRIX_COLS;
}

void matrix_init(void)
{
    // disable JTAG
    MCUCR |= (1 << JTD);
    MCUCR |= (1 << JTD);

    // initialize row and col
    unselect_rows();
    init_cols();

    // initialize matrix state: all keys off
    for (uint8_t i=0; i < MATRIX_ROWS; i++) {
        matrix[i] = 0;
        raw_matrix[i] = 0;
    }

    // initialize our debounce matrix
    debounce_matrix_init();
}

#ifdef BENCHMARK_MATRIX
    static int scans = 0;
    static uint16_t last_print_out = 0;
    static int last_timer = 0;
static void benchmark(void)
{
    scans++;
    uint16_t timer = timer_read();
    if (timer != last_timer && timer != last_timer + 1)
    {        
        print ("MS:\n");
        print_dec(timer);
        print ("->");
        print_dec(last_timer);
        print("\n");
    }

    last_timer = timer;
    if ((timer % 1000 == 0) && (timer != last_print_out))
    {
        print("Benchmark:");
        print("\n");
        print_dec(timer);
        print("\n");
        print_dec(scans);
        print("\n");
        print("-------");
        scans = 0;
        last_print_out = timer;
    }
}
#endif


uint8_t matrix_scan(void)
{	
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        select_row(i);        
        // must sleep for two nops before calling read_cols()
        // or get corrupted data        
        asm volatile ("nop"); asm volatile ("nop");        
        matrix_row_t cols = read_cols();
        raw_matrix[i] = cols;
        unselect_rows();
    }

    //debounce our keys. feed in raw matrix, let it write to final matrix.
    update_debounce_matrix(raw_matrix, matrix);

    #ifdef BENCHMARK_MATRIX
        benchmark();
    #endif
    return 1;
}

bool matrix_is_modified(void)
{
    return true;
}

inline
bool matrix_is_on(uint8_t row, uint8_t col)
{
    return (matrix[row] & ((matrix_row_t)1<<col));
}

inline
matrix_row_t matrix_get_row(uint8_t row)
{
    return matrix[row];
}

void matrix_print(void)
{
    print("\nr/c 0123456789ABCDEF\n");
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        phex(row); print(": ");
        pbin_reverse16(matrix_get_row(row));
        print("\n");
    }
}

uint8_t matrix_key_count(void)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        count += bitpop16(matrix[i]);
    }
    return count;
}

static void init_cols(void) {
    DDRC  &= ~(1 << 6);
    PORTC |=  (1 << 6);
    DDRD  &= ~(1 << 7);
    PORTD |=  (1 << 7);
    DDRE  &= ~(1 << 6);
    PORTE |=  (1 << 6);
    DDRB  &= ~(1 << 4 | 1 << 5 | 1 << 6 | 1 << 2 | 1 << 3 | 1 << 1);
    PORTB |=  (1 << 4 | 1 << 5 | 1 << 6 | 1 << 2 | 1 << 3 | 1 << 1);
    DDRF  &= ~(1 << 7 | 1 << 6 | 1 << 5 | 1 << 4);
    PORTF |=  (1 << 7 | 1 << 6 | 1 << 5 | 1 << 4);
}

static matrix_row_t read_cols(void) {
    return
        (PINC & (1 << 6) ? 0 : (1UL <<  0)) |
        (PIND & (1 << 7) ? 0 : (1UL <<  1)) |
        (PINE & (1 << 6) ? 0 : (1UL <<  2)) |
        (PINB & (1 << 4) ? 0 : (1UL <<  3)) |
        (PINB & (1 << 5) ? 0 : (1UL <<  4)) |
        (PINB & (1 << 6) ? 0 : (1UL <<  5)) |
        (PINB & (1 << 2) ? 0 : (1UL <<  6)) |
        (PINB & (1 << 3) ? 0 : (1UL <<  7)) |
        (PINB & (1 << 1) ? 0 : (1UL <<  8)) |
        (PINF & (1 << 7) ? 0 : (1UL <<  9)) |
        (PINF & (1 << 6) ? 0 : (1UL << 10)) |
        (PINF & (1 << 5) ? 0 : (1UL << 11)) |
        (PINF & (1 << 4) ? 0 : (1UL << 12));
}

static void unselect_rows(void) {
    DDRD  &= ~0b00011111;
    PORTD &= ~0b00011111;
}

static void select_row(uint8_t row) {
    switch (row) {
        case 0:
            DDRD  |=  (1 << 3);
            PORTD &= ~(1 << 3);
            break;
        case 1:
            DDRD  |=  (1 << 2);
            PORTD &= ~(1 << 2);
            break;
        case 2:
            DDRD  |=  (1 << 1);
            PORTD &= ~(1 << 1);
            break;
        case 3:
            DDRD  |=  (1 << 0);
            PORTD &= ~(1 << 0);
            break;
        case 4:
            DDRD  |=  (1 << 4);
            PORTD &= ~(1 << 4);
            break;
    }    
};
/*
Original TMK algorithm
Has debounce timers per row.
*/
#include "matrix.h"
#include "debounce_matrix.h"
#include <util/delay.h>
#include "timer.h"
#ifndef DEBOUNCE
    #define DEBOUNCE 5
#endif

static uint8_t debouncing[MATRIX_ROWS]; //debouncing timer in milliseconds.
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static uint8_t last_tick = 0;

void debounce_matrix_init(void)
{	
    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        matrix_debouncing[i] = 0;
        debouncing[i] = DEBOUNCE;
    }
}

bool tick_changed(void)
{
    uint8_t new_tick = timer_read() % 0xFF; //update timer.
    bool result = new_tick == last_tick;
    last_tick = new_tick;
    return result;
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    bool new_tick = tick_changed();
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix_row_t cols = raw_values[i];
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            debouncing[i] = DEBOUNCE;
        }

        if (debouncing[i]) {
            if (new_tick) {
                if (--debouncing[i]) {
                output_matrix[i] = matrix_debouncing[i];
                }
            }
        }
    }
    
}

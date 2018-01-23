/*
TMK Algorithm for infinity and newer boards.
*/
#include "matrix.h"
#include "debounce_matrix.h"
#include <util/delay.h>
#include "timer.h"
#ifndef DEBOUNCE
    #define DEBOUNCE 5
#endif

static bool debouncing = false;
static uint16_t debouncing_time;

static matrix_row_t matrix_debouncing[MATRIX_ROWS];

void debounce_matrix_init(void)
{	
    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        matrix_debouncing[i] = 0;
    }
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    uint16_t current_time = timer_read();
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix_row_t cols = raw_values[i];
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            debouncing = true;
            debouncing_time = current_time;
        }
    }
    
    if (debouncing && timer_elapsed(debouncing_time) > DEBOUNCE) {
        for (int i = 0; i < MATRIX_ROWS; i++) {
            output_matrix[i] = matrix_debouncing[i];
        }
        debouncing = false;
    }
    
}

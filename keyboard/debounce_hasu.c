/*
Original TMK algorithm
Scans entire matrix, only updating entire matrix if no changes occured in last 5ms
*/
#include "matrix.h"
#include "debounce_matrix.h"
#include <util/delay.h>

#ifndef DEBOUNCE
    #define DEBOUNCE 5
#endif

static uint8_t debouncing = DEBOUNCE; //debouncing timer in milliseconds.
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
    for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
        matrix_row_t cols = raw_values[i];
        if (matrix_debouncing[i] != cols) {
            matrix_debouncing[i] = cols;
            debouncing = DEBOUNCE;
        }
    }
    
    if (debouncing) {
        if (--debouncing) {
            _delay_ms(1);
        } else {
            for (uint8_t i = 0; i < MATRIX_ROWS; i++) {
                output_matrix[i] = matrix_debouncing[i];
            }
        }
    }

}

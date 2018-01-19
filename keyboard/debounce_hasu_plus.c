/*
Original TMK algorithm
Debounces each key for user configurable press/release.
Per key debouncing rather than whole-matrix debouncing.
*/
#include "matrix.h"
#include "debounce_matrix.h"
#include <util/delay.h>
#include "timer.h"

#ifndef DEBOUNCE_PRESS
    #define DEBOUNCE_PRESS 2
#endif

#ifndef DEBOUNCE_RELEASE
    #define DEBOUNCE_RELEASE 5
#endif

#define MAX_DEBOUNCE 5

#define CHECK_BIT(var,pos) ((var) & (1 <<(pos)))
#define CLEAR_BIT(var,pos) ((var) & ~(1 << (pos)))
#define SET_BIT(var,pos) ((var) | (1 << (pos)))

static bool is_debouncing[MATRIX_ROWS*MATRIX_COLS];
static uint8_t checks_remaining[MATRIX_ROWS*MATRIX_COLS];
static uint8_t last_check[MATRIX_ROWS*MATRIX_COLS];
static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static uint8_t current_time;

void debounce_matrix_init(void)
{	
    for (uint8_t i = 0; i < MATRIX_ROWS; i++)
    {
        matrix_debouncing[i] = 0;
    }
    
    for (uint8_t i = 0; i < MATRIX_ROWS*MATRIX_COLS; i++)
    {
        is_debouncing[i] = false;
        checks_remaining[i] = 0;
        last_check[i] = 0;
    }
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    current_time = timer_read(); //update timer.
    for (uint8_t row_num = 0; row_num < MATRIX_ROWS; row_num++) {
        matrix_row_t cols = raw_values[row_num];
        matrix_row_t result = output_matrix[row_num];
        matrix_row_t existing = matrix_debouncing[row_num];
        for (uint8_t col_num = 0; col_num < MATRIX_COLS; col_num++) {
            bool new_col = CHECK_BIT(cols, col_num);
            bool old_col = CHECK_BIT(existing, col_num);
            uint8_t index = row_num * MATRIX_COLS + col_num;
            if (is_debouncing[index]) {                
                if (current_time != last_check[index]) { //otherwise skip check.
                    if (new_col == old_col) {
                        checks_remaining[index] = checks_remaining[index] - 1;
                        if (checks_remaining[index] == 0) 
                        {
                            if (new_col) {
                                result = SET_BIT(result, col_num);
                            } else {
                                result = CLEAR_BIT(result, col_num);
                            }
                            is_debouncing[index] = false;
                        } else {
                            last_check[index] = current_time;
                        }                        
                    } else { //reset checks because value changed.
                        if (new_col) {                        
                            last_check[index] = current_time;
                            checks_remaining[index] = DEBOUNCE_PRESS;
                        } else {
                            last_check[index] = current_time;
                            checks_remaining[index] = DEBOUNCE_RELEASE;
                        }
                    }
                }
            } else if (new_col != old_col) { //not debouncing. time to add a debouncer
                is_debouncing[index] = true;
                last_check[index] = current_time;
                checks_remaining[index] = new_col ? DEBOUNCE_PRESS : DEBOUNCE_RELEASE;                
            }            
        }
        
        matrix_debouncing[row_num] = cols;        
        output_matrix[row_num] = result;
    }
    
}

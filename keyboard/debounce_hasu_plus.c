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

#define CHECK_BIT(var,pos) ((var) & (1 <<(pos)))
#define CLEAR_BIT(var,pos) ((var) & ~(1 << (pos)))
#define SET_BIT(var,pos) ((var) | (1 << (pos)))

static matrix_row_t matrix_debouncing[MATRIX_ROWS];


static uint8_t current_time;

//bits 1,2,3 - bounces remaining
//bits 6,7,8 - last timestamp with debounce, modulo MAX_DEBOUNCE
typedef uint8_t debounce_t;
#define IS_DEBOUNCING(var) (var)
#define CHECKS_REMAINING(var) (((var) >> 5) & 0b111)
#define MAX_DEBOUNCE 0b111
#define IS_TIME(var, time) (((var) & (MAX_DEBOUNCE)) == time)
static debounce_t debounce_data[MATRIX_ROWS*MATRIX_COLS];



static inline debounce_t set_debounce(uint8_t checks)
{         
    return ((checks << 5) | (current_time % MAX_DEBOUNCE));               
}

void debounce_matrix_init(void)
{	
    for (uint8_t i = 0; i < MATRIX_ROWS; i++)
    {
        matrix_debouncing[i] = 0;
    }
    
    for (uint8_t i = 0; i < MATRIX_ROWS*MATRIX_COLS; i++)
    {
        debounce_data[i] = 0;
    }
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    current_time = timer_read() % MAX_DEBOUNCE; //update timer.
    debounce_t* data = debounce_data;
    matrix_row_t* local_data = matrix_debouncing;
    
    for (uint8_t row_num = 0; row_num < MATRIX_ROWS; row_num++) {
        matrix_row_t cols = *raw_values;
        matrix_row_t result = *output_matrix;
        matrix_row_t existing = *local_data;
        
        for (uint8_t col_num = 0; col_num < MATRIX_COLS; col_num++) {
            bool new_col = CHECK_BIT(cols, col_num);
            bool old_col = CHECK_BIT(existing, col_num);
            
            if (IS_DEBOUNCING(*data)) {                
                if (!IS_TIME(*data, current_time)) { //otherwise skip check.
                    if (new_col == old_col) {
                        uint8_t remaining_checks = CHECKS_REMAINING(*data);
                        if (remaining_checks == 1) 
                        {
                            if (new_col) {
                                result = SET_BIT(result, col_num);
                            } else {
                                result = CLEAR_BIT(result, col_num);
                            }
                            *data = 0;
                        } else {
                            *data = set_debounce(remaining_checks - 1);
                        }                        
                    } else { //reset checks because value changed.
                        *data = set_debounce(new_col ? DEBOUNCE_PRESS : DEBOUNCE_RELEASE);
                    }
                }
            } else if (new_col != old_col) { //not debouncing. time to add a debouncer
                *data = set_debounce(new_col ? DEBOUNCE_PRESS : DEBOUNCE_RELEASE);
            }      
            
            data++;
        }
        
        *local_data = cols;        
        *output_matrix = result;
        
        raw_values++;
        output_matrix++;
        local_data++;
    }
    
}

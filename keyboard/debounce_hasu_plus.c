/*
Debounces each key for user configurable press/release.
Per key debouncing rather than whole-matrix debouncing.
*/
#include "matrix.h"
#include "debounce_matrix.h"
#include "timer.h"

//How many checks to do.
//The first check is instant, the 2nd check may be less than 1ms apart
//All further checks are at least 1ms apart.
#ifndef DEBOUNCE_PRESS
    #define DEBOUNCE_PRESS 2
#endif

#ifndef DEBOUNCE_RELEASE
    #define DEBOUNCE_RELEASE 3
#endif

#define CHECK_BIT(var,pos)          ((var) & (1 <<(pos)))
#define CHECK_BITMASK(var, mask)    ((var) & (mask))
#define SET_BIT(var,pos)            ((var) | (1 << (pos)))
#define CLEAR_BIT(var,pos)          ((var) & ~(1 << (pos)))
#define SET_BITMASK(var, mask)      ((var) | (mask))
#define CLEAR_BITMASK(var, mask)    ((var) & ~(mask))

static matrix_row_t matrix_debouncing[MATRIX_ROWS];

static uint8_t current_time;

typedef uint8_t debounce_t; //checks remaining
static debounce_t debounce_data[MATRIX_ROWS*MATRIX_COLS];
#define IS_DEBOUNCING(var) (var) //(var != 0)

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

//scans non-debounced keys as fast as possible.
//scans debouncing keys only once per ms.
void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    uint8_t new_time = (uint8_t)timer_read(); //update timer.
    bool timer_changed = new_time == current_time;
    current_time = new_time;

    debounce_t* data = debounce_data;
    matrix_row_t* local_data = matrix_debouncing;
    for (uint8_t row_num = 0; row_num < MATRIX_ROWS; row_num++) {
        matrix_row_t cols = *raw_values;
        matrix_row_t result = *output_matrix;
        matrix_row_t existing = *local_data;
        
        matrix_row_t bitmask = 1;
        
        for (uint8_t col_num = 0; col_num < MATRIX_COLS; col_num++) {
            bool new_col = CHECK_BITMASK(cols, bitmask);
            bool old_col = CHECK_BITMASK(existing, bitmask);

            if (IS_DEBOUNCING(*data)) {
                if (timer_changed) { //only check once per millisecond.
                    if (new_col == old_col) {                 
                        (*data)--; //decrement count.
                        if (*data == 0) //compare to one so we don't have to decrement
                        {
                            if (new_col) {
                                result = SET_BITMASK(result, bitmask);
                            } else {
                                result = CLEAR_BITMASK(result, bitmask);
                            }                        
                        }                 
                    } else { //reset checks because value changed.
                        *data = new_col ? DEBOUNCE_PRESS : DEBOUNCE_RELEASE;
                    }            
                }
            } else if (new_col != old_col) { //not debouncing. time to add a debouncer
                *data = new_col ? DEBOUNCE_PRESS : DEBOUNCE_RELEASE;
            }
            data++;
            bitmask <<= 1;
        }
        
        *local_data = cols;
        *output_matrix = result;
        
        raw_values++;
        output_matrix++;
        local_data++;
    }

}

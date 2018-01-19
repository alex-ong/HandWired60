#include "matrix.h"
#include "debounce_matrix.h"
#include "timer.h"

//debounce times in milliseconds. Note that keys are sent on first state change,
//so there is no EMI protection. Actual debounce time is between (value) and (value - 1),
//due to timer resolution.
#ifndef DEBOUNCE_PRESS
    #define DEBOUNCE_PRESS 5
#endif

#ifndef DEBOUNCE_RELEASE
    #define DEBOUNCE_RELEASE 5
#endif

#define BOUNCE_BIT              (0b10000000)
#define MAX_DEBOUNCE            (0b01111111)
#define DEBOUNCE_MODULO_MASK    (0b01111111)
#define IS_BOUNCING(button_data) (button_data) //just have to check for non-zero
#define BOUNCE_EXPIRY(button_data) (button_data & DEBOUNCE_MODULO_MASK)

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define CLEAR_BIT(var,pos) ((var) & ~(1 << (pos)))
#define SET_BIT(var,pos) ((var) | (1 << (pos)))

//first bit is whether we are bouncing, remaining 7 bits is modulo of when we are ready again
typedef uint8_t debounce_button_t; 

static debounce_button_t matrix[MATRIX_ROWS * MATRIX_COLS];
static uint8_t current_time;

void debounce_matrix_init(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++)
    {
        matrix[i] = 0;
    }
}

inline static void set_expiry(debounce_button_t* button, uint8_t timeFromNow)
{
    *button = ((current_time + timeFromNow) % MAX_DEBOUNCE) ^ BOUNCE_BIT;
}

//We assume that this will be called at least once every millisecond!
static void check_debounce_matrix(void) 
{	    
    debounce_button_t* button_data = matrix;
    for (uint8_t i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++)
    {
        if (IS_BOUNCING(*button_data) && current_time == BOUNCE_EXPIRY(*button_data))
        {
            *button_data = 0;
        }
        button_data++;
    }
}

static inline void handle_new_data(matrix_row_t* raw_values, matrix_row_t* output)
{
    for (uint8_t row = 0; row < MATRIX_ROWS; row++)
    {
        matrix_row_t raw_row = *raw_values;
        matrix_row_t existing_row = *output;
        matrix_row_t result_row = existing_row;

        if (raw_row != existing_row) { //quick check for change.
            debounce_button_t* button_data = matrix + row * MATRIX_COLS;
            for (uint8_t col = 0; col < MATRIX_COLS; col++)
            {                
                if (!IS_BOUNCING(*button_data))
                {                    
                    bool existing_value = CHECK_BIT(existing_row, col);
                    bool new_value = CHECK_BIT(raw_row, col);
                    if (existing_value != new_value) { //value changed, lets reflect that immediately
                        if (new_value) 
                        {
                            result_row = SET_BIT(result_row, col); //send press
                            #if DEBOUNCE_PRESS != 0
                            set_expiry(button_data, DEBOUNCE_PRESS);
                            #endif
                        } else {
                            result_row = CLEAR_BIT(result_row, col); //send release
                            #if DEBOUNCE_RELEASE != 0
                            set_expiry(button_data, DEBOUNCE_RELEASE);
                            #endif
                        }
                    }
                }
                button_data++;
            }
            *output = result_row;
        }
        raw_values++;
        output++;
    }
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    current_time = timer_read() % MAX_DEBOUNCE; //update timer.
    check_debounce_matrix(); //first clear all times if appropriate
    handle_new_data(raw_values, output_matrix); //reads raw values, writing some of them to output_matrix
}

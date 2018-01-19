#include "matrix.h"
#include "debounce_matrix.h"
#include "timer.h"

#define BOUNCE_BIT (1 << 7)
#define MAX_DEBOUNCE (1 << 6)
#define DEBOUNCE_MODULO_MASK (0b01111111)
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) ((var) | (1 << (pos)))

//first bit is whether we are bouncing, remaining 7 bits is modulo of when we are ready again
typedef uint8_t debounce_button_t; 

static debounce_button_t matrix[MATRIX_ROWS * MATRIX_COLS];
static matrix_row_t debounced_matrix[MATRIX_ROWS];
static uint8_t current_time;

void debounce_matrix_init(void)
{
    for (int i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++)
    {
        matrix[i] = 0;
    }
    for (int i = 0; i < MATRIX_ROWS; i++)
    {
        debounced_matrix[i] = 0;
    }
}


inline static bool is_bouncing(int index)
{
    //return (matrix[index] & BOUNCE_BIT == BOUNCE_BIT);
    //return (matrix[index] & BOUNCE_BIT);
    return (matrix[index]); //since value is non-zero, this works.
}

inline static uint8_t bounce_expiry(int index)
{
    return (matrix[index] & DEBOUNCE_MODULO_MASK);
}

inline static void set_expiry(int index, uint8_t timeFromNow)
{
    matrix[index] = ((current_time + timeFromNow) % MAX_DEBOUNCE) ^ BOUNCE_BIT;
}

//We assume that this will be called at least once every millisecond!
static void check_debounce_matrix(void) 
{	    
    for (int i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++)
    {
        if (is_bouncing(i) && current_time == bounce_expiry(i))
        {
            matrix[i] = 0;
        }
    }
}

//variables for function.


static inline void handle_new_data(matrix_row_t* raw_values, matrix_row_t* output)
{

    for (int row = 0; row < MATRIX_ROWS; row++)
    {
        matrix_row_t raw_row = raw_values[row];
        matrix_row_t existing_row = output[row];
        matrix_row_t result_row = 0;
        uint8_t matrix_index = row * MATRIX_COLS;

        if (raw_row != existing_row) { //quick check for change.
            for (int col = 0; col < MATRIX_COLS; col++)
            {
                bool existing_value = CHECK_BIT(existing_row, col);
                bool new_value = CHECK_BIT(raw_row, col);
                if (is_bouncing(matrix_index))
                {
                    if (existing_value) result_row = SET_BIT(result_row, col);
                    //else do nothing cause it's already 0
                } 
                else if (existing_value != new_value) 
                {
                    if (new_value) 
                    {
                        result_row = SET_BIT(result_row, col); //send press
                        set_expiry(matrix_index, DEBOUNCE_PRESS); //won't detect release until at least this many ms has passed.
                    } else {
                        set_expiry(matrix_index, DEBOUNCE_RELEASE); //won't detect press until at least this many ms passed.
                    }
                }
                matrix_index++;
            }
        }
        output[row] = result_row;
    }
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{
    current_time = timer_read() % MAX_DEBOUNCE; //update timer.
    check_debounce_matrix(); //first clear all times if appropriate
    handle_new_data(raw_values, output_matrix); //reads raw values, writing some of them to output_matrix
}

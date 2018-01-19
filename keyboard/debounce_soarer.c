#include "matrix.h"
#include "debounce_matrix.h"
#include "timer.h"

//debounces by checking last n scans (up to 8)
//Note that this isn't millisecond based so timing will be based on speed of scanning
//each bit is a history of the buttons status. e.g. 0b00000011 means its been down for 2 scans.
//                                                 past <-- present
typedef uint8_t debounce_button_t; 

static debounce_button_t matrix[MATRIX_ROWS * MATRIX_COLS];

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define KEY_DOWN 0b00000001 //first key down after at least 7 cycles of release
#define KEY_UP 0b11111110 //first key_release after at least 7 cycles of keydown

void debounce_matrix_init(void)
{
    for (uint8_t i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++)
    {
        matrix[i] = 0;
    }
}

void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix)
{    
    debounce_button_t* button_history = matrix;

    for (uint8_t row = 0; row < MATRIX_ROWS; row++)
    {
        matrix_row_t raw_row = *raw_values++;
        matrix_row_t result_row = *output_matrix; //grab original value
        
        for (uint8_t col = 0; col < MATRIX_COLS; col++)
        {
            bool raw_value = CHECK_BIT(raw_row, col);
            
            *button_history = (*button_history << 1) | raw_value; //shift everything along.
            if (*button_history == KEY_DOWN) {
                result_row |= 1 << col;
            } else if (*button_history == KEY_UP) {
                result_row &= ~(1 << col);
            }            
            button_history++;
        }

        *output_matrix  = result_row;
        output_matrix++;
    }
}

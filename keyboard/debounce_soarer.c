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


#define DEBOUNCE_KEY_DOWN 5 //number of readings for before we accept that the key is actually down
#define DEBOUNCE_KEY_UP 5
#define KEY_DOWN_MASK (0b11111111 >> (8 - DEBOUNCE_KEY_DOWN))
#define KEY_UP_MASK (0b11111111 >> (8 - DEBOUNCE_KEY_UP)) //first key_release after at least 7 cycles of keydown
#define IS_KEY_DOWN(var) (((var) & KEY_DOWN_MASK) == KEY_DOWN_MASK)
#define IS_KEY_UP(var) (((~(var)) & KEY_UP_MASK) == KEY_UP_MASK)

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
        matrix_row_t raw_row = *raw_values;
        matrix_row_t result_row = *output_matrix; //grab original value
        
        for (uint8_t col = 0; col < MATRIX_COLS; col++)
        {
            bool raw_value = CHECK_BIT(raw_row, col);
            
            *button_history = (*button_history << 1) | raw_value; //shift everything along.
            if (IS_KEY_DOWN(*button_history)) {
                result_row |= 1 << col;
            } else if (IS_KEY_UP(*button_history)) {
                result_row &= ~(1 << col);
            } //else we don't modify button state, and it stays the same
            button_history++;
        }

        *output_matrix  = result_row;
        //incrememnt to next row of raw/output matrices
        raw_values++;
        output_matrix++;
    }
}

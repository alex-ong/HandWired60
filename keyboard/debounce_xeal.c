#include "matrix.h"
#include "debounce_matrix.h"
#include "timer.h"

#define BOUNCE_BIT (1 << 8)
#define MAX_DEBOUNCE (1 << 7)
#define DEBOUNCE_MODULO_MASK (0x7F)

//first bit is whether we are bouncing, remaining 7 bits is modulo of when we are ready again
typedef uint8_t debounce_button_t; 

static debounce_button_t matrix[MATRIX_ROWS * MATRIX_COLS];
static matrix_row_t debounced_matrix[MATRIX_ROWS];
static uint8_t current_time = 0;

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


inline static bool isBouncing(int index)
{
	//return (matrix[index] & BOUNCE_BIT);
	return (matrix[index]); //since value is non-zero, this works.
}

inline static uint8_t bounce_expiry(int index)
{
	return (matrix[index] & DEBOUNCE_MODULO_MASK);
}

inline static void set_expiry(int index, uint8_t timeFromNow)
{
	matrix[index] = (current_time + timeFromNow) ^ BOUNCE_BIT;
}

//We assume that this will be called at least once every millisecond!
inline void update_debounce_matrix(void) 
{	
	current_time = timer_read() % MAX_DEBOUNCE;
	for (int i = 0; i < MATRIX_ROWS * MATRIX_COLS; i++)
	{
		if (isBouncing(i) && current_time == bounce_expiry(i))
		{
			matrix[i] = 0;
		}
	}
}


/* call this on each row with the raw values */
inline matrix_row_t debounce_matrix_set_row(uint8_t row, matrix_row_t value)
{
	matrix_row_t existing = debounced_matrix[row];
	matrix_row_t result = 0;
	int matrix_index = row * MATRIX_COLS;
	for (int i = 0; i < MATRIX_COLS; i++)
	{		
		bool existingValue = (existing >> i) & 1;
		bool newValue = (value >> i) & 1;
		
		if (isBouncing(matrix_index) && existingValue) { //bouncing. set result to existing value.
			result |= 1 << i;
		} else if (existingValue != newValue) { //not bouncing and state change.
			debounced_matrix[matrix_index] = newValue;			
			if (newValue)  //key pressed down.
			{
				result |= 1 << i;
				set_expiry(matrix_index, DEBOUNCE_PRESS); //won't detect release until at least DEBOUNCE ms have passed
			} else { //key released.
				set_expiry(matrix_index, DEBOUNCE_RELEASE); //won't detect next press until at least DEBOUNCE ms have passed
			}
		}
		matrix_index++;
	}	
	
	debounced_matrix[row] = result;

	return result;
}


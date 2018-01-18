#include "time.h"
#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif
//first bit is whether we are bouncing, remaining 7 bits is modulo of when we are ready again
typedef uint8_t debounce_button_t; 

/* call this to initialize our matrix */
void debounce_matrix_init(void);
/* call this first - refreshes all debouncing timers */
void update_debounce_matrix(void);
/* call this on each row with the raw values. returns debounced values */
matrix_row_t debounce_matrix_set_row(uint8_t row, matrix_row_t value);

#ifdef __cplusplus
}
#endif
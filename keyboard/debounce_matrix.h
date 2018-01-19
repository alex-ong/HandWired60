#include "time.h"
#include "matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

/* call this to initialize our matrix */
void debounce_matrix_init(void);

/* call this every update loop. writes output to finalMatrix */
void update_debounce_matrix(matrix_row_t* raw_values, matrix_row_t* output_matrix);

#ifdef __cplusplus
}
#endif
/* Program that transforms a given initial two-dimensional matrix into a target
  matrix by applying a sequence of matrix manipulations.
  
  Skeleton program written by Artem Polyvyanyy, http://polyvyanyy.com/,
  September 2025, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.
  All included code is (c) Copyright University of Melbourne, 2025.

  Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: Zhuyirui Xu
  Dated:     17 October 2025
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* #DEFINE'S -----------------------------------------------------------------*/
#define SDELIM "==STAGE %d============================\n"   // stage delimiter
#define THEEND "==THE END============================\n"    // end message
#define MTXDIM "%dx%d\n"                // matrix dimensions input format
#define MAX_LINE_LEN 1000
#define INITIAL_CAPACITY 0
#define GROWTH_FACTOR 2
#define MAX_SMALL_DIM 35
#define MAX_SMALL_VAL 9
#define DELIMITER "-------------------------------------"

/* TYPE DEFINITIONS ----------------------------------------------------------*/
// Compressed Sparse Row (CSR) matrix representation
typedef struct {
    int  rows;       // number of rows in this matrix
    int  cols;       // number of columns in this matrix
    int  nnz;        // number of stored non-zeros values in this matrix
    int  cap;        // matrix capacity to hold non-zero values
    int* vals;       // non-zero values in this matrix
    int* cidx;       // column indices of non-zero values, in row-major order
    int* rptr;       // row pointers
} CSRMatrix_t;

/* FUNCTION PROTOTYPES -------------------------------------------------------*/
/* INTERFACE FUNCTIONS FOR WORKING WITH CSR MATRICES -------------------------*/
CSRMatrix_t*  csr_matrix_create(int, int);        // create empty CSR matrix
void          csr_matrix_free(CSRMatrix_t*);      // free input CSR matrix
int           csr_matrix_get(CSRMatrix_t*, int, int); // get matrix element
void          csr_matrix_set(CSRMatrix_t*, int, int, int); // set matrix element
int           csr_matrix_equals(CSRMatrix_t*, CSRMatrix_t*); // check equality
CSRMatrix_t*  csr_matrix_read(int, int);          // read matrix from input
void          csr_matrix_print(CSRMatrix_t*, char*); // print matrix
CSRMatrix_t*  csr_matrix_copy(CSRMatrix_t*);      // copy matrix
int           is_small_matrix(CSRMatrix_t*);      // check if matrix is small
void          resize_if_needed(CSRMatrix_t*);     // resize matrix if needed
int           find_element_index(CSRMatrix_t*, int, int); // find element index

// Operation prototypes
void op_set(CSRMatrix_t*, int, int, int);
void op_swap(CSRMatrix_t*, int, int, int, int);
void op_multiply(CSRMatrix_t*, int);
void op_add(CSRMatrix_t*, int);
void op_copy_row(CSRMatrix_t*, int, int);
void op_copy_col(CSRMatrix_t*, int, int);
void op_swap_row(CSRMatrix_t*, int, int);
void op_swap_col(CSRMatrix_t*, int, int);
void matrix_sort(CSRMatrix_t *mat);

// Solution handling function
void print_solution_and_cleanup(CSRMatrix_t*, CSRMatrix_t*, CSRMatrix_t*, int);
void cleanup_matrices(CSRMatrix_t*, CSRMatrix_t*, CSRMatrix_t*);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int main(void) {
    int stage = 0, rows, cols;
    char line[MAX_LINE_LEN];
    // Stage 0 initialization
    printf(SDELIM, stage++);    // print Stage 0 header
    // Read matrix dimensions
    fgets(line, MAX_LINE_LEN, stdin);
    assert(sscanf(line, MTXDIM, &rows, &cols) == 2);
    // Create initial and target matrices
    CSRMatrix_t* initial = csr_matrix_read(rows, cols);
    CSRMatrix_t* target = csr_matrix_read(rows, cols);
    CSRMatrix_t* current = csr_matrix_copy(initial);
    // Print initial and target matrices
    csr_matrix_print(initial, "Initial matrix");
    printf("%s\n", DELIMITER);
    csr_matrix_print(target, "Target matrix");
    // Check if already solved
    if (csr_matrix_equals(current, target)) {
        print_solution_and_cleanup(initial, target, current, 0);
        return EXIT_SUCCESS;
    }
    int step_count = 0;
    int stage1_printed = 0;
    int stage2_printed = 0;
    // Process operations
    while (fgets(line, MAX_LINE_LEN, stdin)) {
        // Remove newline and skip empty lines
        line[strcspn(line, "\n")] = 0;  
        if (strlen(line) == 0) continue;
        char op_type = line[0];
        // Check for stage 2 operations
        int is_stage2_op = (op_type == 'r' || op_type == 'c' || op_type == 'R' 
                         || op_type == 'C');
        // Print stage headers when needed
        if (!stage1_printed && stage == 1) {
            printf(SDELIM, stage++);  // Print Stage 1 header
            stage1_printed = 1;
        }
        if (is_stage2_op && !stage2_printed) {
            printf(SDELIM, stage++);  // Print Stage 2 header
            stage2_printed = 1;
        }
        // Print instruction
        printf("INSTRUCTION %s\n", line);
        // Parse and execute operation
        if (op_type == 's') {
            int r, c, val;
            sscanf(line + 2, "%d,%d,%d", &r, &c, &val);
            op_set(current, r, c, val);
        } else if (op_type == 'S') {
            int r1, c1, r2, c2;
            sscanf(line + 2, "%d,%d,%d,%d", &r1, &c1, &r2, &c2);
            op_swap(current, r1, c1, r2, c2);
        } else if (op_type == 'm') {
            int val;
            sscanf(line + 2, "%d", &val);
            op_multiply(current, val);
        } else if (op_type == 'a') {
            int val;
            sscanf(line + 2, "%d", &val);
            op_add(current, val);
        } else if (op_type == 'r') {
            int r1, r2;
            sscanf(line + 2, "%d,%d", &r1, &r2);
            op_copy_row(current, r1, r2);
        } else if (op_type == 'c') {
            int c1, c2;
            sscanf(line + 2, "%d,%d", &c1, &c2);
            op_copy_col(current, c1, c2);
        } else if (op_type == 'R') {
            int r1, r2;
            sscanf(line + 2, "%d,%d", &r1, &r2);
            op_swap_row(current, r1, r2);
        } else if (op_type == 'C') {
            int c1, c2;
            sscanf(line + 2, "%d,%d", &c1, &c2);
            op_swap_col(current, c1, c2);
        } else {
            break;
        }
        step_count++;
        // sort
        matrix_sort(current);
        // Print current and target matrices
        csr_matrix_print(current, "Current matrix");
        csr_matrix_print(target, "Target matrix");
        // Check if solved
        if (csr_matrix_equals(current, target)) {
            print_solution_and_cleanup(initial, target, current, step_count);
            return EXIT_SUCCESS;
        }
    }
    // No solution found
    printf(THEEND);
    cleanup_matrices(initial, target, current);
    return EXIT_SUCCESS;
}

void matrix_sort(CSRMatrix_t *mat) {
    if (mat->nnz <= 1) {
        return;
    }

    // Extract all non-zero elements as (row, col, val)
    int *rows_array = malloc(sizeof(int) * mat->nnz);
    int *cols_array = malloc(sizeof(int) * mat->nnz);
    int *vals_array = malloc(sizeof(int) * mat->nnz);
    assert(rows_array && cols_array && vals_array);

    // Extract data from CSR format
    int idx = 0;
    for (int r = 0; r < mat->rows; r++) {
        for (int i = mat->rptr[r]; i < mat->rptr[r + 1]; i++) {
            rows_array[idx] = r;
            cols_array[idx] = mat->cidx[i];
            vals_array[idx] = mat->vals[i];
            idx++;
        }
    }

    // Bubble sort in row-major order
    for (int i = 0; i < mat->nnz - 1; i++) {
        for (int j = i + 1; j < mat->nnz; j++) {
            int swap_needed = 0;

            // First compare rows
            if (rows_array[i] > rows_array[j]) {
                swap_needed = 1;
            }
            // If rows are equal, compare columns
            else if (rows_array[i] == rows_array[j] && 
                     cols_array[i] > cols_array[j]) {
                swap_needed = 1;
            }

            if (swap_needed) {
                // Swap elements across the three arrays
                int temp;

                temp = rows_array[i];
                rows_array[i] = rows_array[j];
                rows_array[j] = temp;

                temp = cols_array[i];
                cols_array[i] = cols_array[j];
                cols_array[j] = temp;

                temp = vals_array[i];
                vals_array[i] = vals_array[j];
                vals_array[j] = temp;
            }
        }
    }

    // Write the sorted data back to CSR format
    // Rebuild rptr, cidx, vals
    for (int i = 0; i <= mat->rows; i++) {
        mat->rptr[i] = 0;
    }

    // Reconstruct CSR structure
    idx = 0;
    for (int r = 0; r < mat->rows; r++) {
        mat->rptr[r] = idx;
        // Collect all elements belonging to row r
        for (int i = 0; i < mat->nnz; i++) {
            if (rows_array[i] == r) {
                mat->cidx[idx] = cols_array[i];
                mat->vals[idx] = vals_array[i];
                idx++;
            }
        }
    }
    mat->rptr[mat->rows] = mat->nnz;

    // Free temporary arrays
    free(rows_array);
    free(cols_array);
    free(vals_array);
}

/* CSR matrix implementation -------------------------------------------------*/

// Create an empty CSR matrix of nrows rows and ncols columns
CSRMatrix_t *csr_matrix_create(int nrows, int ncols) {
    assert(nrows >= 0 && ncols >= 0);
    CSRMatrix_t *A = (CSRMatrix_t*)malloc(sizeof(CSRMatrix_t));
    assert(A != NULL);
    A->rows = nrows;
    A->cols = ncols;
    A->nnz = 0;
    A->cap = INITIAL_CAPACITY;
    A->vals = NULL;
    A->cidx = NULL;
    A->rptr = (int*)malloc((size_t)(A->rows + 1) * sizeof(int));
    assert(A->rptr != NULL);
    for (int i = 0; i <= A->rows; i++) {
        A->rptr[i] = 0;
    }
    return A;
}

// Free input CSR matrix A
void csr_matrix_free(CSRMatrix_t *A) {
    assert(A != NULL);
    free(A->vals);
    free(A->cidx);
    free(A->rptr);
    free(A);
}

// Resize CSR matrix if necessary
void resize_if_needed(CSRMatrix_t *A) {
    if (A->nnz >= A->cap) {
        int new_cap = (A->cap == 0) ? 1 : A->cap * GROWTH_FACTOR;
        A->vals = (int*)realloc(A->vals, sizeof(int) * new_cap);
        A->cidx = (int*)realloc(A->cidx, sizeof(int) * new_cap);
        assert(A->vals != NULL && A->cidx != NULL);
        A->cap = new_cap;
    }
}

// Find index of element at (r,c) in CSR matrix
int find_element_index(CSRMatrix_t *A, int r, int c) {
    for (int i = A->rptr[r]; i < A->rptr[r + 1]; i++) {
        if (A->cidx[i] == c) {
            return i;
        }
    }
    return -1;
}

// Get element value at (r,c)
int csr_matrix_get(CSRMatrix_t *A, int r, int c) {
    assert(r >= 0 && r < A->rows && c >= 0 && c < A->cols);
    int idx = find_element_index(A, r, c);
    return (idx != -1) ? A->vals[idx] : 0;
}

// Set element value at (r,c)
void csr_matrix_set(CSRMatrix_t *A, int r, int c, int val) {
    assert(r >= 0 && r < A->rows && c >= 0 && c < A->cols);
    int idx = find_element_index(A, r, c);
    
    if (idx != -1) {
        if (val == 0) {
            // Remove element
            for (int i = idx; i < A->nnz - 1; i++) {
                A->vals[i] = A->vals[i + 1];
                A->cidx[i] = A->cidx[i + 1];
            }
            A->nnz--;
            
            // Update row pointers
            for (int i = r + 1; i <= A->rows; i++) {
                A->rptr[i]--;
            }
        } else {
            // Update existing element
            A->vals[idx] = val;
        }
    } else if (val != 0) {
        // Add new element
        resize_if_needed(A);
        
        // Shift elements to make space
        int insert_pos = A->rptr[r + 1];
        for (int i = A->nnz; i > insert_pos; i--) {
            A->vals[i] = A->vals[i - 1];
            A->cidx[i] = A->cidx[i - 1];
        }
        
        // Insert new element
        A->vals[insert_pos] = val;
        A->cidx[insert_pos] = c;
        A->nnz++;
        
        // Update row pointers
        for (int i = r + 1; i <= A->rows; i++) {
            A->rptr[i]++;
        }
    }
}

// Check if two matrices are equal 
int csr_matrix_equals(CSRMatrix_t *A, CSRMatrix_t *B) {
    if (A->rows != B->rows || A->cols != B->cols) {
        return 0;
    }
    
    if (A->nnz != B->nnz) {
        return 0;
    }
    
    for (int i = 0; i < A->nnz; i++) {
        int r = 0;
        while (r < A->rows && A->rptr[r + 1] <= i) {
            r++;
        }
        int c = A->cidx[i];
        int valA = A->vals[i];
        
        int valB = csr_matrix_get(B, r, c);
        
        if (valA != valB) {
            return 0;
        }
    }
    
    for (int i = 0; i < B->nnz; i++) {
        int r = 0;
        while (r < B->rows && B->rptr[r + 1] <= i) {
            r++;
        }
        int c = B->cidx[i];
        int valB = B->vals[i];
        
        int valA = csr_matrix_get(A, r, c);
        
        if (valA != valB) {
            return 0;
        }
    }
    
    return 1;
}

// Read matrix from input
CSRMatrix_t* csr_matrix_read(int rows, int cols) {
    CSRMatrix_t *A = csr_matrix_create(rows, cols);
    char line[MAX_LINE_LEN];
    
    while (fgets(line, MAX_LINE_LEN, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;
        if (line[0] == '#') break;
        
        int r, c, val;
        if (sscanf(line, "%d,%d,%d", &r, &c, &val) == 3) {
            csr_matrix_set(A, r, c, val);
        }
    }
    
    return A;
}

// Check if matrix is small
int is_small_matrix(CSRMatrix_t *A) {
    if (A->rows > MAX_SMALL_DIM || A->cols > MAX_SMALL_DIM) return 0;
    
    for (int i = 0; i < A->nnz; i++) {
        if (A->vals[i] < 0 || A->vals[i] > MAX_SMALL_VAL) return 0;
    }
    return 1;
}

// Print matrix
void csr_matrix_print(CSRMatrix_t *A, char *title) {
    printf("%s: %dx%d, nnz=%d\n", title, A->rows, A->cols, A->nnz);
    
    if (is_small_matrix(A)) {
        // Print small matrix as grid
        for (int r = 0; r < A->rows; r++) {
            printf("[");
            for (int c = 0; c < A->cols; c++) {
                int val = csr_matrix_get(A, r, c);
                printf("%c", val == 0 ? ' ' : '0' + val);
            }
            printf("]\n");
        }
    } else {
        // Print large matrix as non-zero elements
        for (int r = 0; r < A->rows; r++) {
            for (int i = A->rptr[r]; i < A->rptr[r + 1]; i++) {
                printf("(%d,%d)=%d\n", r, A->cidx[i], A->vals[i]);
            }
        }
    }
}

// Copy matrix
CSRMatrix_t* csr_matrix_copy(CSRMatrix_t *A) {
    CSRMatrix_t *copy = csr_matrix_create(A->rows, A->cols);
    
    // Allocate memory for values and column indices
    if (A->nnz > 0) {
        copy->cap = A->cap;
        copy->vals = (int*)malloc(sizeof(int) * copy->cap);
        copy->cidx = (int*)malloc(sizeof(int) * copy->cap);
        assert(copy->vals != NULL && copy->cidx != NULL);
        
        // Copy values and column indices
        memcpy(copy->vals, A->vals, sizeof(int) * A->nnz);
        memcpy(copy->cidx, A->cidx, sizeof(int) * A->nnz);
        copy->nnz = A->nnz;
    }
    
    // Copy row pointers
    memcpy(copy->rptr, A->rptr, sizeof(int) * (A->rows + 1));
    
    return copy;
}

/* Operations implementation -------------------------------------------------*/

void op_set(CSRMatrix_t *A, int r, int c, int val) {
    csr_matrix_set(A, r, c, val);
}

void op_swap(CSRMatrix_t *A, int r1, int c1, int r2, int c2) {
    int val1 = csr_matrix_get(A, r1, c1);
    int val2 = csr_matrix_get(A, r2, c2);
    csr_matrix_set(A, r1, c1, val2);
    csr_matrix_set(A, r2, c2, val1);
}

void op_multiply(CSRMatrix_t *A, int val) {
    for (int i = 0; i < A->nnz; i++) {
        A->vals[i] *= val;
    }
}

void op_add(CSRMatrix_t *A, int val) {
    int i = 0;
    while (i < A->nnz) {
        A->vals[i] += val;
        if (A->vals[i] == 0) {
            // Remove element that became zero
            int r = 0;
            while (r < A->rows && A->rptr[r + 1] <= i) r++;
            
            for (int j = i; j < A->nnz - 1; j++) {
                A->vals[j] = A->vals[j + 1];
                A->cidx[j] = A->cidx[j + 1];
            }
            A->nnz--;
            
            for (int j = r + 1; j <= A->rows; j++) {
                A->rptr[j]--;
            }
        } else {
            i++;
        }
    }
}

void op_copy_row(CSRMatrix_t *A, int r1, int r2) {
    // Clear destination row
    for (int c = 0; c < A->cols; c++) {
        csr_matrix_set(A, r2, c, 0);
    }
    
    // Copy from source row
    for (int c = 0; c < A->cols; c++) {
        int val = csr_matrix_get(A, r1, c);
        if (val != 0) {
            csr_matrix_set(A, r2, c, val);
        }
    }
}

void op_copy_col(CSRMatrix_t *A, int c1, int c2) {
    // Clear destination column
    for (int r = 0; r < A->rows; r++) {
        csr_matrix_set(A, r, c2, 0);
    }
    
    // Copy from source column
    for (int r = 0; r < A->rows; r++) {
        int val = csr_matrix_get(A, r, c1);
        if (val != 0) {
            csr_matrix_set(A, r, c2, val);
        }
    }
}

void op_swap_row(CSRMatrix_t *A, int r1, int r2) {
    if (r1 == r2) return;
    
    // Create temporary copies of both rows
    CSRMatrix_t *temp = csr_matrix_create(1, A->cols);
    for (int c = 0; c < A->cols; c++) {
        csr_matrix_set(temp, 0, c, csr_matrix_get(A, r1, c));
    }
    
    // Copy row r2 to row r1
    op_copy_row(A, r2, r1);
    
    // Copy temporary row to row r2
    for (int c = 0; c < A->cols; c++) {
        csr_matrix_set(A, r2, c, csr_matrix_get(temp, 0, c));
    }
    
    csr_matrix_free(temp);
}

void op_swap_col(CSRMatrix_t *A, int c1, int c2) {
    if (c1 == c2) return;
    
    // Swap column indices in all rows
    for (int r = 0; r < A->rows; r++) {
        for (int i = A->rptr[r]; i < A->rptr[r + 1]; i++) {
            if (A->cidx[i] == c1) {
                A->cidx[i] = c2;
            } else if (A->cidx[i] == c2) {
                A->cidx[i] = c1;
            }
        }
    }
}

// Print solution message and cleanup matrices
void print_solution_and_cleanup(CSRMatrix_t* initial, CSRMatrix_t* target, 
                                CSRMatrix_t* current, int step_count) {
    printf("%s\n", DELIMITER);
    printf("TA-DAA!!! SOLVED IN %d STEP(S)!\n", step_count);
    printf(THEEND);
    csr_matrix_free(initial);
    csr_matrix_free(target);
    csr_matrix_free(current);
}

// Cleanup matrices without printing solution message
void cleanup_matrices(CSRMatrix_t* initial, CSRMatrix_t* target, 
                      CSRMatrix_t* current) {
    csr_matrix_free(initial);
    csr_matrix_free(target);
    csr_matrix_free(current);
}

//algorithms are fun！
/* THE END -------------------------------------------------------------------*/

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
#define DELIMITER "-------------------------------------"
#define INITIAL_CAPACITY 0
#define GROWTH_FACTOR 2
#define END_HEADER "======================================="

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
int           csr_matrix_get(CSRMatrix_t*, int, int);
void          csr_matrix_set(CSRMatrix_t*, int, int, int);
int           csr_matrix_equals(CSRMatrix_t*, CSRMatrix_t*);
CSRMatrix_t*  csr_matrix_read(int, int);
void          csr_matrix_print(CSRMatrix_t*, char*);
CSRMatrix_t*  csr_matrix_copy(CSRMatrix_t*);

// Helper functions
int find_matrix_index(CSRMatrix_t*, int, int);
void resize_if_needed(CSRMatrix_t*);
void remove_element(CSRMatrix_t*, int);

// Operation functions
void op_set(CSRMatrix_t*, int, int, int);
void op_swap(CSRMatrix_t*, int, int, int, int);
void op_multiply(CSRMatrix_t*, int);
void op_add(CSRMatrix_t*, int);
void op_copy_row(CSRMatrix_t*, int, int);
void op_copy_col(CSRMatrix_t*, int, int);
void op_swap_row(CSRMatrix_t*, int, int);
void op_swap_col(CSRMatrix_t*, int, int);

/* WHERE IT ALL HAPPENS ------------------------------------------------------*/
int main(void) {
    int stage = 0, rows, cols;
    char line[MAX_LINE_LEN];

    printf(SDELIM, stage++);                      // print Stage 0 header
    assert(scanf(MTXDIM, &rows, &cols) == 2);    // assert matrix dimensions
    
    CSRMatrix_t* initial = csr_matrix_read(rows, cols);
    CSRMatrix_t* target = csr_matrix_read(rows, cols);
    CSRMatrix_t* current = csr_matrix_copy(initial);

    csr_matrix_print(initial, "Initial matrix");
    printf("%s\n", DELIMITER);
    csr_matrix_print(target, "Target matrix");

    int step_count = 0;
    int stage1_printed = 0;

    while (fgets(line, MAX_LINE_LEN, stdin)) {
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0) {
            continue;
        }

        char op_type = line[0];

        int is_stage2_op = (op_type == 'r' || op_type == 'c' ||
                            op_type == 'R' || op_type == 'C');
        
        if (is_stage2_op && stage == 1) {
            printf(SDELIM, stage++);
        } else if (!stage1_printed && stage == 1) {
            printf(SDELIM, stage++);
            stage1_printed = 1;
        }

        printf("INSTRUCTION %s\n", line);

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

        csr_matrix_print(current, "Current matrix");
        csr_matrix_print(target, "Target matrix");

        if (csr_matrix_equals(current, target)) {
            printf("%s\n", DELIMITER);
            printf("TA-DAA!!! SOLVED IN %d STEP(S)!\n", step_count);
            printf(THEEND);

            csr_matrix_free(initial);
            csr_matrix_free(target);
            csr_matrix_free(current);
            return EXIT_SUCCESS;
        }
    }
    
    printf(THEEND);                               // print "THE END" message
    csr_matrix_free(initial);                     // free initial matrix
    csr_matrix_free(target);                      // free target matrix
    csr_matrix_free(current);                     // free current matrix
    return EXIT_SUCCESS;                          // algorithms are fun!!!
}

// Create an empty CSR  matrix of nrows rows and ncols columns
CSRMatrix_t *csr_matrix_create(int nrows, int ncols) {
    assert(nrows >= 0 && ncols >= 0);   // check matrix dimensions
    // allocate memory for this matrix
    CSRMatrix_t *A = (CSRMatrix_t*)malloc(sizeof(CSRMatrix_t));
    assert(A!=NULL);            // check if memory was allocated
    A->rows = nrows;            // set number of rows in the matrix
    A->cols = ncols;            // set number of columns in the matrix
    A->nnz  = 0;                // initialize with no non-zero values
    A->cap  = 0;                // initialize capacity to no non-zero values
    A->vals = NULL;             // no values to store...
    A->cidx = NULL;             // so there is no need to store column indices
    // allocate array to store row pointers
    A->rptr = (int*)malloc((size_t)(A->rows+1)*sizeof(int));
    assert(A->rptr!=NULL);
    for (int i = 0; i <= A->rows; i++) {    // no values, so initialize ...
        A->rptr[i] = 0;                     // ... all row pointers to zeros
    }
    return A;
}

// Free input CSR matrix A
void csr_matrix_free(CSRMatrix_t *A) {
    assert(A!=NULL);
    free(A->vals);      // free matrix values
    free(A->cidx);      // free column indices
    free(A->rptr);      // free row pointers
    free(A);            // free matrix
}

int csr_matrix_get(CSRMatrix_t *mat, int r, int c) {
    assert(r >= 0 && r < mat->rows);
    assert(c >= 0 && c < mat->cols);
    
    // Find the value in CSR format
    int start = mat->rptr[r];
    int end = mat->rptr[r + 1];
    
    for (int i = start; i < end; i++) {
        if (mat->cidx[i] == c) {
            return mat->vals[i];
        }
    }
    return 0;
}

void csr_matrix_set(CSRMatrix_t *mat, int r, int c, int val) {
    assert(r >= 0 && r < mat->rows);
    assert(c >= 0 && c < mat->cols);
    
    int start = mat->rptr[r];
    int end = mat->rptr[r + 1];
    
    // Find existing element using binary search for efficiency
    int pos = -1;
    int left = start;
    int right = end - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (mat->cidx[mid] == c) {
            pos = mid;
            break;
        } else if (mat->cidx[mid] < c) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    
    if (pos >= 0) {
        if (val == 0) {
            // Remove element
            for (int i = pos; i < mat->nnz - 1; i++) {
                mat->vals[i] = mat->vals[i + 1];
                mat->cidx[i] = mat->cidx[i + 1];
            }
            mat->nnz--;
            // Update row pointers
            for (int i = r + 1; i <= mat->rows; i++) {
                mat->rptr[i]--;
            }
        } else {
            mat->vals[pos] = val;
        }
    } else {
        if (val != 0) {
            // Insert new element
            resize_if_needed(mat);
            
            // Insert at the correct position to maintain column order
            int insert_pos = left;
            
            // Shift elements to make room
            for (int i = mat->nnz; i > insert_pos; i--) {
                mat->vals[i] = mat->vals[i - 1];
                mat->cidx[i] = mat->cidx[i - 1];
            }
            
            mat->vals[insert_pos] = val;
            mat->cidx[insert_pos] = c;
            mat->nnz++;
            
            // Update row pointers
            for (int i = r + 1; i <= mat->rows; i++) {
                mat->rptr[i]++;
            }
        }
    }
}

int csr_matrix_equals(CSRMatrix_t *m1, CSRMatrix_t *m2) {
    if (m1->rows != m2->rows || m1->cols != m2->cols) {
        return 0;
    }

    for (int r = 0; r < m1->rows; r++) {
        for (int c = 0; c < m1->cols; c++) {
            if (csr_matrix_get(m1, r, c) != csr_matrix_get(m2, r, c)) {
                return 0;
            }
        }
    }

    return 1;
}

CSRMatrix_t* csr_matrix_read(int nrows, int ncols) {
    char line[1000];
    CSRMatrix_t *mat = csr_matrix_create(nrows, ncols);

    while (fgets(line, 1000, stdin)) {
        if (line[0] == '#') {
            break;
        }

        int r, c, val;
        if (sscanf(line, "%d,%d,%d", &r, &c, &val) == 3) {
            csr_matrix_set(mat, r, c, val);
        }
    }

    return mat;
}

void csr_matrix_print(CSRMatrix_t *mat, char *title) {
    printf("%s: %dx%d, nnz=%d\n", title, mat->rows, mat->cols, mat->nnz);

    // Print non-zero elements in coordinate format
    for (int r = 0; r < mat->rows; r++) {
        int start = mat->rptr[r];
        int end = mat->rptr[r + 1];
        for (int i = start; i < end; i++) {
            printf("(%d,%d)=%d\n", r, mat->cidx[i], mat->vals[i]);
        }
    }
}

CSRMatrix_t* csr_matrix_copy(CSRMatrix_t *mat) {
    CSRMatrix_t *copy = csr_matrix_create(mat->rows, mat->cols);
    for (int r = 0; r < mat->rows; r++) {
        int start = mat->rptr[r];
        int end = mat->rptr[r + 1];
        for (int i = start; i < end; i++) {
            csr_matrix_set(copy, r, mat->cidx[i], mat->vals[i]);
        }
    }
    return copy;
}

void resize_if_needed(CSRMatrix_t *mat) {
    if (mat->nnz >= mat->cap) {
        if (mat->cap == 0) {
            mat->cap = 1;
        } else {
            mat->cap *= GROWTH_FACTOR;
        }
        mat->vals = (int*)realloc(mat->vals, sizeof(int) * mat->cap);
        mat->cidx = (int*)realloc(mat->cidx, sizeof(int) * mat->cap);
        assert(mat->vals != NULL && mat->cidx != NULL);
    }
}

void op_set(CSRMatrix_t *mat, int r, int c, int val) {
    csr_matrix_set(mat, r, c, val);
}

void op_swap(CSRMatrix_t *mat, int r1, int c1, int r2, int c2) {
    int val1 = csr_matrix_get(mat, r1, c1);
    int val2 = csr_matrix_get(mat, r2, c2);
    csr_matrix_set(mat, r1, c1, val2);
    csr_matrix_set(mat, r2, c2, val1);
}

void op_multiply(CSRMatrix_t *mat, int val) {
    for (int i = 0; i < mat->nnz; i++) {
        mat->vals[i] *= val;
    }
}

void op_add(CSRMatrix_t *mat, int val) {
    int i = 0;
    while (i < mat->nnz) {
        mat->vals[i] += val;
        if (mat->vals[i] == 0) {
            // Find which row this element belongs to
            int row = 0;
            while (mat->rptr[row + 1] <= i) {
                row++;
            }
            csr_matrix_set(mat, row, mat->cidx[i], 0);
        } else {
            i++;
        }
    }
}

void op_copy_row(CSRMatrix_t *mat, int r1, int r2) {
    // Clear target row first
    int start2 = mat->rptr[r2];
    int end2 = mat->rptr[r2 + 1];
    int count2 = end2 - start2;
    
    // Remove elements from target row
    for (int i = 0; i < count2; i++) {
        csr_matrix_set(mat, r2, mat->cidx[start2], 0);
    }
    
    // Copy elements from source row
    int start1 = mat->rptr[r1];
    int end1 = mat->rptr[r1 + 1];
    for (int i = start1; i < end1; i++) {
        csr_matrix_set(mat, r2, mat->cidx[i], mat->vals[i]);
    }
}

void op_copy_col(CSRMatrix_t *mat, int c1, int c2) {
    // Clear target column first
    for (int r = 0; r < mat->rows; r++) {
        csr_matrix_set(mat, r, c2, 0);
    }
    
    // Copy elements from source column
    for (int r = 0; r < mat->rows; r++) {
        int start = mat->rptr[r];
        int end = mat->rptr[r + 1];
        for (int i = start; i < end; i++) {
            if (mat->cidx[i] == c1) {
                csr_matrix_set(mat, r, c2, mat->vals[i]);
            }
        }
    }
}

void op_swap_row(CSRMatrix_t *mat, int r1, int r2) {
    // Swap row pointers
    int temp_start1 = mat->rptr[r1];
    int temp_end1 = mat->rptr[r1 + 1];
    int temp_start2 = mat->rptr[r2];
    int temp_end2 = mat->rptr[r2 + 1];
    
    // Adjust row pointers
    mat->rptr[r1] = temp_start2;
    mat->rptr[r1 + 1] = temp_end2;
    mat->rptr[r2] = temp_start1;
    mat->rptr[r2 + 1] = temp_end1;
}

void op_swap_col(CSRMatrix_t *mat, int c1, int c2) {
    for (int i = 0; i < mat->nnz; i++) {
        if (mat->cidx[i] == c1) {
            mat->cidx[i] = c2;
        } else if (mat->cidx[i] == c2) {
            mat->cidx[i] = c1;
        }
    }
}
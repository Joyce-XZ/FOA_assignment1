#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_LINE_LEN 1000
#define STAGE_0_HEADER "==STAGE 0============================"
#define STAGE_1_HEADER "==STAGE 1============================"
#define STAGE_2_HEADER "==STAGE 2============================"
#define END_HEADER "========================================"
#define DELIMITER "-------------------------------------"
#define INITIAL_CAPACITY 0
#define GROWTH_FACTOR 2

typedef struct {
    int *rows;
    int *cols;
    int *vals;
    int nnz;
    int nrows;
    int ncols;
    int capacity;
}matrix_t;

matrix_t* matrix_create(int nrows, int ncols);
void matrix_free(matrix_t *mat);
int matrix_get(matrix_t *mat, int r, int c);
void matrix_set(matrix_t *mat, int r, int c, int val);
int matrix_equals(matrix_t *m1, matrix_t *m2);

matrix_t* matrix_read(int nrows, int ncols);
void matrix_print(matrix_t *mat, char *title);
matrix_t* matrix_copy(matrix_t *mat);

int find_matrix_index(matrix_t *mat, int r, int c);
void resize_if_needed(matrix_t *mat);
void remove_element(matrix_t *mat, int index);

void op_set(matrix_t *mat, int r, int c, int val);
void op_swap(matrix_t *mat, int r1, int c1, int r2, int c2);
void op_multiply(matrix_t *mat, int val);
void op_add(matrix_t *mat, int val);
void op_copy_row(matrix_t *mat, int r1, int r2);
void op_copy_col(matrix_t *mat, int c1, int c2);
void op_swap_row(matrix_t *mat, int r1, int r2);
void op_swap_col(matrix_t *mat, int c1, int c2);


int main(int argc, char *argv[]){
    int nrows, ncols;
    char line[MAX_LINE_LEN];

    fgets(line, MAX_LINE_LEN, stdin);
    sscanf(line, "%dx%d", &nrows, &ncols);

    matrix_t *initial = matrix_read(nrows, ncols);
    matrix_t *target = matrix_read(nrows, ncols);
    matrix_t *current = matrix_copy(initial);

    printf("%s\n", STAGE_0_HEADER);
    matrix_print(initial, "Initial matrix");
    printf("%s\n", DELIMITER);
    matrix_print(target, "Target matrix");

    int stage = 1;
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
            stage = 2;
            printf("%s\n", STAGE_2_HEADER);
        } else if (!stage1_printed && stage == 1) {
            printf("%s\n", STAGE_1_HEADER);
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

        matrix_print(current, "Current matrix");
        matrix_print(target, "Target matrix");

        if (matrix_equals(current, target)) {
            printf("%s\n", DELIMITER);
            printf("TA-DAA!!! SOLVED IN %d STEP(S)!\n", step_count);
            printf("%s\n", END_HEADER);

            matrix_free(initial);
            matrix_free(target);
            matrix_free(current);
            return 0;
        }
    }
}

matrix_t* matrix_create(int nrows, int ncols) {
    matrix_t *mat = (matrix_t*)malloc(sizeof(matrix_t));
    assert(mat != NULL);

    mat->nrows = nrows;
    mat->ncols = ncols;
    mat->nnz = 0;
    mat->capacity = INITIAL_CAPACITY;

    mat->rows = (int*)malloc(sizeof(int) * mat->capacity);
    mat->cols = (int*)malloc(sizeof(int) * mat->capacity);
    mat->vals = (int*)malloc(sizeof(int) * mat->capacity);

    assert(mat->rows != NULL && mat->cols != NULL && mat->vals != NULL);

    return mat;
}

matrix_t* matrix_copy(matrix_t *mat) {
    matrix_t *copy = matrix_create(mat->nrows, mat->ncols);
    copy->nnz = mat->nnz;
    copy->capacity = mat->capacity;
    
    copy->rows = (int*)malloc(sizeof(int) * mat->capacity);
    copy->cols = (int*)malloc(sizeof(int) * mat->capacity);
    copy->vals = (int*)malloc(sizeof(int) * mat->capacity);
    
    assert(copy->rows != NULL && copy->cols != NULL && copy->vals != NULL);
    
    for (int i = 0; i < mat->nnz; i++) {
        copy->rows[i] = mat->rows[i];
        copy->cols[i] = mat->cols[i];
        copy->vals[i] = mat->vals[i];
    }
    
    return copy;
}

void matrix_free(matrix_t *mat) {
    if (mat) {
        free(mat->rows);
        free(mat->cols);
        free(mat->vals);
        free(mat);
    }
}

int matrix_get(matrix_t *mat, int r, int c) {
    assert(r >= 0 && r < mat->nrows);
    assert(c >= 0 && c < mat->ncols);
    int idx = find_matrix_index(mat, r, c);
    return (idx >= 0) ? mat->vals[idx] : 0;
}

void matrix_set(matrix_t *mat, int r, int c, int val) {
    assert(r >= 0 && r < mat->nrows);
    assert(c >= 0 && c < mat->ncols);
    int idx = find_matrix_index(mat, r, c);

    if (idx >= 0) {
        if (val == 0) {
            remove_element(mat, idx);
        } else {
            mat->vals[idx] = val;
        }
    } else {
        if (val != 0) {
            resize_if_needed(mat);
            mat->rows[mat->nnz] = r;
            mat->cols[mat->nnz] = c;
            mat->vals[mat->nnz] = val;
            mat->nnz++;
        }
    }
}

int find_matrix_index(matrix_t *mat, int r, int c) {
    for (int i = 0; i < mat->nnz; i++) {
        if (mat->rows[i] ==r && mat->cols[i] == c) {
            return i;
        }
    }
    return -1;
}

int matrix_equals(matrix_t *m1, matrix_t *m2) {
    if (m1->nrows != m2->nrows || m1->ncols != m2->ncols) {
        return 0;
    }

    for (int r = 0; r < m1->nrows; r++) {
        for (int c = 0; c < m1->ncols; c++) {
            if (matrix_get(m1, r, c) != matrix_get(m2, r, c)) {
                return 0;
            }
        }
    }

    return 1;
}

matrix_t* matrix_read(int nrows, int ncols) {
    char line[1000];
    matrix_t *mat = matrix_create(nrows, ncols);

    while (fgets(line, 1000, stdin)) {
        if (line[0] == '#') {
            break;
        }

        int r, c, val;
        if (sscanf(line, "%d,%d,%d", &r, &c, &val) == 3) {
            matrix_set(mat, r, c, val);
        }
    }

    return mat;
}

void matrix_print(matrix_t *mat, char *title) {
    printf("%s: %dx%d, nnz=%d\n", title, mat->nrows, mat->ncols, mat->nnz);

    for (int r = 0; r < mat->nrows; r++) {
        printf("[");
        for (int c = 0; c < mat->ncols; c++) {
            int val = matrix_get(mat, r, c);
            if (val == 0) {
                printf(" ");
            } else {
                printf("%d", val);
            }
        }
        printf(" ]\n");
    }
}

void resize_if_needed(matrix_t *mat) {
    if (mat->nnz >= mat->capacity) {
        mat->capacity *= GROWTH_FACTOR;
        mat->rows = (int*)realloc(mat->rows, sizeof(int) * mat->capacity);
        mat->cols = (int*)realloc(mat->cols, sizeof(int) * mat->capacity);
        mat->vals = (int*)realloc(mat->vals, sizeof(int) * mat->capacity);
        assert(mat->rows != NULL && mat->cols != NULL && mat->vals != NULL);
    }
}

void remove_element(matrix_t *mat, int index) {
    for (int i = index; i < mat->nnz - 1; i++) {
        mat->rows[i] = mat->rows[i + 1];
        mat->cols[i] = mat->cols[i + 1];
        mat->vals[i] = mat->vals[i + 1];
    }
    mat->nnz--;
}

void op_set(matrix_t *mat, int r, int c, int val) {
    matrix_set(mat, r, c, val);
}

void op_swap(matrix_t *mat, int r1, int c1, int r2, int c2) {
    int val1 = matrix_get(mat, r1, c1);
    int val2 = matrix_get(mat, r2, c2);
    matrix_set(mat, r1, c1, val2);
    matrix_set(mat, r2, c2, val1);
}

void op_multiply(matrix_t *mat, int val) {
    for (int i = 0; i < mat->nnz; i++) {
        mat->vals[i] *= val;
    }
}

void op_add(matrix_t *mat, int val) {
    for (int i = 0; i < mat->nnz; i++) {
        mat->vals[i] += val;
        if (mat->vals[i] == 0) {
            remove_element(mat, i);
        }
    }
}

void op_copy_row(matrix_t *mat, int r1, int r2) {
    int i = 0;
    while (i < mat->nnz) {
        if (mat->rows[i] == r2) {
            remove_element(mat, i);
        } else {
            i++;
        }
    }

    for (int c = 0; c < mat->ncols; c++) {
        int val = matrix_get(mat, r1, c);
        if (val != 0) {
            matrix_set(mat, r2, c, val);
        }
    }
}

void op_copy_col(matrix_t *mat, int c1, int c2) {
    int i = 0;
    while (i < mat->nnz) {
        if (mat->cols[i] == c2) {
            remove_element(mat, i);
        } else {
            i++;
        }
    }

    for (int r = 0; r < mat->nrows; r++) {
        int val = matrix_get(mat, r, c1);
        if (val != 0) {
            matrix_set(mat, r, c2, val);
        }
    }
}

void op_swap_row(matrix_t *mat, int r1, int r2) {
    for (int i = 0; i < mat->nnz; i++) {
        if (mat->rows[i] == r1) {
            mat->rows[i] = r2;
        } else if (mat->rows[i] == r2) {
            mat->rows[i] = r1;
        }
    }
}

void op_swap_col(matrix_t *mat, int c1, int c2) {
    for (int i = 0; i < mat->nnz; i++) {
        if (mat->cols[i] == c1) {
            mat->cols[i] = c2;
        } else if (mat->cols[i] == c2) {
            mat->cols[i] = c1;
        }
    }
}
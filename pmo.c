////////////////////////////////Libraries///////////////////////////////////////
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <ctype.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////GLOBAL VARIABLES/STRUCTS////////////////////////////

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////FUNCTIONS///////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////MAIN FUNCTION//////////////////////////////////
typedef struct {
    char name;
    int rows;
    int cols;
    int** data;
} Matrix;

int readMatrix(Matrix* matrix) {
    char c;
    matrix->data = (int**)malloc(matrix->rows * sizeof(int*));
    if (matrix->data == NULL) {
        printf("Memory allocation failed for matrix data.\n");
        return -1;
    }
    for (int i = 0; i < matrix->rows; i++) {
        matrix->data[i] = (int*)malloc(matrix->cols * sizeof(int));
        if (matrix->data[i] == NULL) {
            printf("Memory allocation failed for matrix data[%d].\n", i);
            return -1;
        }
    }

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            if (scanf("%d", &matrix->data[i][j]) != 1) {
                printf("Failed to read element at row %d, column %d.\n", i + 1, j + 1);
                return -1;
            }
        }
        while ((c = getchar()) != '\n' && c != EOF) {
            // Skip the remaining characters on the current line
        }
    }
    return 0;
}

void freeMatrix(Matrix* matrix) {
    if (matrix->data != NULL) {
        for (int i = 0; i < matrix->rows; i++) {
            free(matrix->data[i]);
        }
        free(matrix->data);
    }
}

void matrixAddition(Matrix* matrix1, Matrix* matrix2, Matrix* result) {
    // Determine the dimensions of the result matrix
    int resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
    int resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;

    // Allocate memory for the result matrix
    result->data = (int**)malloc(resultRows * sizeof(int*));
    if (result->data == NULL) {
        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
        return;
    }
    for (int i = 0; i < resultRows; i++) {
        result->data[i] = (int*)malloc(resultCols * sizeof(int));
        if (result->data[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
            return;
        }
    }

    // Perform matrix addition
    for (int i = 0; i < resultRows; i++) {
        for (int j = 0; j < resultCols; j++) {
            int value1 = (i < matrix1->rows && j < matrix1->cols) ? matrix1->data[i][j] : 0;
            int value2 = (i < matrix2->rows && j < matrix2->cols) ? matrix2->data[i][j] : 0;
            result->data[i][j] = value1 + value2;
        }
    }

    // Update the dimensions of the result matrix
    result->rows = resultRows;
    result->cols = resultCols;
}

void matrixSubtraction(Matrix* matrix1, Matrix* matrix2, Matrix* result) {
    // Perform matrix addition and store the result in the 'result' Matrix struct
    
}

void matrixMultiplication(Matrix* matrix1, Matrix* matrix2, Matrix* result) {
    // Perform matrix multiplication and store the result in the 'result' Matrix struct
    
}

void expressionInterpretation(char expression[50], Matrix* matrices) {
    // Perform interpretation of the expression using the provided matrices
}

void printMatrix(Matrix* matrix) {
    printf("Matrix %c:\n", matrix->name);
    printf("n: %d\n", matrix->rows);
          printf("m: %d\n", matrix->cols);
        for (int j = 0; j < matrix->rows; j++) {
            for (int k = 0; k < matrix->cols; k++) {
                printf("%d ", matrix->data[j][k]);
            }
            printf("\n");
}
}

int main() {
    char expression[50];
    int n, m;
    int numMatrices = 0;
    Matrix* matrices;

    // Read the expression and matrix dimensions
    if (scanf("%[^\n]%*c", expression) != 1) {
        printf("Failed to read the expression.\n");
        return 1;
    }

    // Count the number of English letters in the expression
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalpha(expression[i])) {
            numMatrices++;
        }
    }

    // Allocate memory for the matrices
    matrices = (Matrix*)malloc(numMatrices * sizeof(Matrix));
    if (matrices == NULL) {
        printf("Memory allocation failed for matrices.\n");
        return 1;
    }

   // Read and assign values to the matrices
    int matrixIndex = 0;
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalpha(expression[i])) {
            if (scanf("%d %d", &n, &m) != 2) {
                printf("Failed to read n and m.\n");
                return 1;
            }
            while (getchar() != '\n') {
                // Skip the remaining characters on the current line
            }
            matrices[matrixIndex].rows = n;
            matrices[matrixIndex].cols = m;
            matrices[matrixIndex].name = matrixIndex + 'A';
            printf("matrix row: %d\n", matrices[matrixIndex].rows);
            printf("matrix column: %d\n", matrices[matrixIndex].cols);
            printf("Enter the elements for matrix %c:\n", expression[i]);
            if (readMatrix(&matrices[matrixIndex]) == -1) {
                // Clean up previously allocated matrices
                for (int j = 0; j < matrixIndex; j++) {
                    freeMatrix(&matrices[j]);
                }
                free(matrices);
                return 1;
            }

            matrixIndex++;
        }
    }

    // Output the expression, n, m, and the matrices
    printf("Expression: %s\n", expression);
    

    for (int i = 0; i < numMatrices; i++) {
        printMatrix(&matrices[i]);
    }

    // Free the allocated memory
    for (int i = 0; i < numMatrices; i++) {
        freeMatrix(&matrices[i]);
    }
    free(matrices);

    return 0;
}
////////////////////////////////////////////////////////////////////////////////

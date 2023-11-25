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
    // Perform matrix addition and store the result in the 'result' Matrix struct
}

void matrixMultiplication(Matrix* matrix1, Matrix* matrix2, Matrix* result) {
    // Perform matrix multiplication and store the result in the 'result' Matrix struct
}

void expressionInterpretation(char expression[50], Matrix* matrices) {
    // Perform interpretation of the expression using the provided matrices
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
         printf("n: %d\n", matrices[i].rows);
          printf("m: %d\n", matrices[i].cols);
        printf("Matrix %c:\n", expression[i]);
        for (int j = 0; j < matrices[i].rows; j++) {
            for (int k = 0; k < matrices[i].cols; k++) {
                printf("%d ", matrices[i].data[j][k]);
            }
            printf("\n");
        }
        printf("\n");
    }

    // Free the allocated memory
    for (int i = 0; i < numMatrices; i++) {
        freeMatrix(&matrices[i]);
    }
    free(matrices);

    return 0;
}
////////////////////////////////////////////////////////////////////////////////

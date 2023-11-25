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
            result->data[i][j] = value1 - value2;
        }
    }

    // Update the dimensions of the result matrix
    result->rows = resultRows;
    result->cols = resultCols;
    
}

void matrixMultiplication(Matrix* matrix1, Matrix* matrix2, Matrix* result) {
    // Define the dimensions of the result matrix
    int resultRows = matrix1->rows;
    int resultCols = matrix2->cols;

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

    // Perform matrix multiplication
    for (int i = 0; i < resultRows; i++) {
        for (int j = 0; j < resultCols; j++) {
            int sum = 0;
            for (int k = 0; k < matrix1->cols; k++) {
                int value1 = (i < matrix1->rows && k < matrix1->cols) ? matrix1->data[i][k] : 0;
                int value2 = (k < matrix2->rows && j < matrix2->cols) ? matrix2->data[k][j] : 0;
                sum += value1 * value2;
            }
            result->data[i][j] = sum;
        }
    }

    // Update the dimensions of the result matrix
    result->rows = resultRows;
    result->cols = resultCols;
}


int expressionInterpretation(char expression[50], Matrix* matrices) {
int count=0;
    char temparr[10]={"KLMNOPQRST"}; // matrices[10]= K... 11->L
    int tempcount=0;
    

    for (int i = 0; i < 50; i++) {  //count the total number of the expression
        if (expression[i] != '\0') {
//            if((expression[i] != '*')&&(expression[i] != '+')&&(expression[i] != '-'))
//                expression[i]=count/2;
            count++;
        } else {
            break;
        }
    }
    //printf("\n%d",count);
    //printf("\n%s",expression);
    for(int i=1;i<count;i+=2){
        if (expression[i]=='*'){
            matrixMultiplication(&matrices[expression[i-1]-'A'],&matrices[expression[i+1]-'A'],&matrices[temparr[tempcount]-'A']);
                    //comment    matrixMultiplication(expression[i-1],expression[i+1],temparr[tempcount]);

                    //comment   temparr[tempcount]=expression[i-1]*expression[i+1]
            expression[i-1]=temparr[tempcount];
            tempcount++;
            for(int j=i;j<count-i;j++)  //shift the remain element;
                expression[j]=expression[j+2];
            //printf("\n%s",expression);
            i-=2;
        }
    }
    for(int i=1;i<count;i+=2){
        if (expression[i]=='+'){
            matrixAddition(&matrices[expression[i-1]-'A'],&matrices[expression[i+1]-'A'],&matrices[temparr[tempcount]-'A']);
            //temparr[tempcount]=expression[i-1]+expression[i+1]
            expression[i-1]=temparr[tempcount];
            tempcount++;
            for(int j=i;j<count-i;j++)  //shift the remain element;
                expression[j]=expression[j+2];
            //printf("\n%s",expression);
            i-=2;
        }
        else if (expression[i]=='-'){
            matrixSubtraction(&matrices[expression[i-1]-'A'],&matrices[expression[i+1]-'A'],&matrices[temparr[tempcount]-'A']);
            //temparr[tempcount]=expression[i-1]-expression[i+1]
            expression[i-1]=temparr[tempcount];
            tempcount++;
            for(int j=i;j<count-i;j++)  //shift the remain element;
                expression[j]=expression[j+2];
        //printf("\n%s",expression);
        i-=2;
        }
    }
  return (tempcount + 9);
}

void initialMatrix(Matrix* matrix) {


}


void printMatrix(Matrix* matrix) {
    //printf("Matrix %c:\n", matrix->name);
    printf("%d",matrix->rows);
    printf("  %d\n", matrix->cols);
    for (int j = 0; j < matrix->rows; j++) {
      for (int k = 0; k < matrix->cols; k++) {
        printf("%d	", matrix->data[j][k]);
      }
      printf("\n");
    }
}

int main() {
    char expression[50];
    int n, m;
    int numMatrices = 20;
    Matrix* matrices;

    // Read the expression and matrix dimensions
    if (scanf("%[^\n]%*c", expression) != 1) {
        printf("Failed to read the expression.\n");
        return 1;
    }

    // Count the number of English letters in the expression
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalpha(expression[i])) {
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
//            printf("matrix row: %d\n", matrices[matrixIndex].rows);
//            printf("matrix column: %d\n", matrices[matrixIndex].cols);
//            printf("Enter the elements for matrix %c:\n", expression[i]);
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

//////////////////////////////CALL FUNCTION HERE/////////////////////////////////////////
  int temp; 
//   //Example of addition
//   temp = 2;
//    matrices[temp].name = 'X';
//    //matrix1, matrix2, result
//    matrixAddition(&matrices[0], &matrices[1], &matrices[temp]);
//   
   
//   //Example of multiplication
//   temp = 2;
//    matrices[temp].name = 'X';
//    //matrix1, matrix2, result
//    matrixMultiplication(&matrices[0], &matrices[1], &matrices[temp]);


     temp = expressionInterpretation(expression,matrices);

//////////////////////////////OUTPUT//////////////////////////////////////////

   



//Output the expression, n, m, and the matrices
//    printf("Expression: %s\n", expression);
//    
//    printf("Num Matrix: %d\n", numMatrices);
//    for (int i = 0; i < numMatrices; i++) {
//        printMatrix(&matrices[i]);
//    }
//    printMatrix(&matrices[temp]);
    printMatrix(&matrices[temp]);
    // Free the allocated memory
    for (int i = 0; i < numMatrices; i++) {
        freeMatrix(&matrices[i]);
    }
    free(matrices);

    return 0;
}
////////////////////////////////////////////////////////////////////////////////

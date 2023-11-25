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
typedef struct {
    char name;
    int rows;
    int cols;
    int** data;
} Matrix;

// Define a Node structure for the linked list
typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

typedef struct {
    Matrix* matrix1;
    Matrix* matrix2;
    Matrix* result;
} MatrixArgs;

#define NUM_THREADS 10
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////FUNCTIONS///////////////////////////////////////
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
void* matrixAddition(void* arg) {

    MatrixArgs* args = (MatrixArgs*)arg;

    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    // Determine the dimensions of the result matrix
    int resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
    int resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;

    // Allocate memory for the result matrix
    result->data = (int**)malloc(resultRows * sizeof(int*));
    if (result->data == NULL) {
        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
        pthread_exit(NULL);
    }
    for (int i = 0; i < resultRows; i++) {
        result->data[i] = (int*)malloc(resultCols * sizeof(int));
        if (result->data[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
            pthread_exit(NULL);;
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
    pthread_exit(NULL);
}


void* matrixSubtraction(void* arg) {

    MatrixArgs* args = (MatrixArgs*)arg;

    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    // Determine the dimensions of the result matrix
    int resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
    int resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;

    // Allocate memory for the result matrix
    result->data = (int**)malloc(resultRows * sizeof(int*));
    if (result->data == NULL) {
        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
        pthread_exit(NULL);
    }
    for (int i = 0; i < resultRows; i++) {
        result->data[i] = (int*)malloc(resultCols * sizeof(int));
        if (result->data[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
            pthread_exit(NULL);
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
    pthread_exit(NULL);
}

void* matrixMultiplication(void* arg) {

    MatrixArgs* args = (MatrixArgs*)arg;

    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    
    // Define the dimensions of the result matrix
    int resultRows = matrix1->rows;
    int resultCols = matrix2->cols;

    // Allocate memory for the result matrix
    result->data = (int**)malloc(resultRows * sizeof(int*));
    if (result->data == NULL) {
        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
        pthread_exit(NULL);
    }
    for (int i = 0; i < resultRows; i++) {
        result->data[i] = (int*)malloc(resultCols * sizeof(int));
        if (result->data[i] == NULL) {
            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
            pthread_exit(NULL);
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
    pthread_exit(NULL);
}
// Function to create a new Node with the given data
Node* createNode(char data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
void printLinkedList(Node* head) {
    Node* current = head;

    printf("Linked List: ");
    while (current != NULL) {
        printf("%c ", current->data);
        current = current->right;
    }
    printf("\n");
}
int expressionInterpretation(char expression[50], Matrix* matrices) {
    int count = 0;
    char temparr[10] = {"KLMNOPQRST"}; // matrices[10]= K... 11->L
    int tempcount = 0;
    int threadcount;
    int rc;
    pthread_t threads[NUM_THREADS];
    MatrixArgs args[NUM_THREADS];
    // Create a linked list to store the expression
    Node* head = createNode(expression[0]);
    Node* current = head;
    Node* previous = current;
    Node* next = current->right;
    Node* tempnode = current->left;
    while(expression[count] != '\0'){// Count the total number of elements in the expression
        if (count >= 1) {
            current->right = createNode(expression[count]);
            current->right->left = current;
            current = current->right;
        }
        count++;
    }

    //printLinkedList(head);

    
    //Clear threadcount
    threadcount = 0;
    //start from head+1
    current = head->right;
    //calculate multiplcation first
    while (current != NULL) {
        previous = current->left;
        next = current->right;
        if (current->data =='*'){

            //multplication
            args[threadcount].matrix1 = &matrices[previous->data -'A'];
            args[threadcount].matrix2 = &matrices[next->data -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            rc = pthread_create(&threads[threadcount], NULL, matrixMultiplication, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            //pthread_join(threads[threadcount], NULL);
            threadcount++;

            
            //creat a new node for a new matrix character
            tempnode = createNode(temparr[tempcount]); 
            tempnode->right = next->right;
            if(next->right != NULL){
            next->right->left = tempnode;
            }
            tempnode->left = previous->left;
            if(previous->left != NULL){
            previous->left->right = tempnode;
            }else{
            head = tempnode;
            }
            free(current);
            free(previous);
            free(next);
            current = tempnode;
            tempcount++;
            for(int i =0;i<2;i++){
                if (current->right !=NULL)
                    current = current->right;
            }
            count -=2;
            //printLinkedList(head);
        }
        current = current->right;
    }
    
    
    // Wait for the threads to finish
    for (int t = 0; t < threadcount; t++) {
        rc = pthread_join(threads[t], NULL);
        if (rc) {
            printf("Error joining thread %d\n", t);
            return -1;
        }
    }
    
    
    
    //Clear threadcount
    threadcount = 0;
    //start from head+1
    current = head->right;
    //calculate multiplcation first
    while (current != NULL) {
        previous = current->left;
        next = current->right;
        if (current->data =='*'){

            //multplication
            args[threadcount].matrix1 = &matrices[previous->data -'A'];
            args[threadcount].matrix2 = &matrices[next->data -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            rc = pthread_create(&threads[threadcount], NULL, matrixMultiplication, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            pthread_join(threads[threadcount], NULL);
            threadcount++;
            
            //creat a new node for a new matrix character
            tempnode = createNode(temparr[tempcount]); 
            tempnode->right = next->right;
            if(next->right != NULL){
            next->right->left = tempnode;
            }
            tempnode->left = previous->left;
            if(previous->left != NULL){
            previous->left->right = tempnode;
            }else{
            head = tempnode;
            }
            free(current);
            free(previous);
            free(next);
            current = tempnode;
            tempcount++;
            
            count -=2;
            //printLinkedList(head);
        }
        current = current->right;
    }
    

    //Clear threadcount
    threadcount = 0;    
    //start from head+1
    current = head->right;
    //calculate Addition and Subtraction
    while (current != NULL) {
        previous = current->left;
        next = current->right;
        if ((current->data =='+') || (current->data =='-')){
            //Addition        
            if (current->data =='+'){
            args[threadcount].matrix1 = &matrices[previous->data -'A'];
            args[threadcount].matrix2 = &matrices[next->data -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            rc = pthread_create(&threads[threadcount], NULL, matrixAddition, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            pthread_join(threads[threadcount], NULL);
            threadcount++;
            }else{//Subtraction
            args[threadcount].matrix1 = &matrices[previous->data -'A'];
            args[threadcount].matrix2 = &matrices[next->data -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            rc = pthread_create(&threads[threadcount], NULL, matrixSubtraction, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            pthread_join(threads[threadcount], NULL);
            threadcount++;
            }

            //creat a new node for a new matrix character
            tempnode = createNode(temparr[tempcount]); 
            tempnode->right = next->right;
            if(next->right != NULL){
            next->right->left = tempnode;
            }
            tempnode->left = previous->left;
            if(previous->left != NULL){
            previous->left->right = tempnode;
            }else{
            head = tempnode;
            }
            free(current);
            free(previous);
            free(next);
            current = tempnode;
            tempcount++;
            
            count -=2;
            //printLinkedList(head);
        }
        current = current->right;
    }
    
    // Clean up the linked list and free memory
    current = head;
    while (current != NULL) {
        tempnode = current;
        current = current->right;
        free(tempnode);
    }
    
  return (tempcount + 9);
  
  
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
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////MAIN FUNCTION//////////////////////////////////

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
  int temp = expressionInterpretation(expression,matrices);
  if (temp == -1) {
    printf("Error");
    return 0;
  }
//////////////////////////////OUTPUT//////////////////////////////////////////

    printMatrix(&matrices[temp]);
    // Free the allocated memory
    for (int i = 0; i < numMatrices; i++) {
        freeMatrix(&matrices[i]);
    }
    free(matrices);

    return 0;
}
////////////////////////////////////////////////////////////////////////////////

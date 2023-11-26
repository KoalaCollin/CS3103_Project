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
    char pthreadID;
    int subPthreadNum;
    char operatorCh;
} MatrixArgs;

typedef struct {
    Matrix* matrix1;
    Matrix* matrix2;
    Matrix* result;
    int i;
    int j;
} PthreadArgs;

#define NUM_THREADS 50
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


void* pthreadAddition(void* arg) {
    PthreadArgs* args = (PthreadArgs*)arg;
    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    int startRow = args->i;
    int endRow = args->j;
    
        // Perform matrix addition
    for (int m = startRow; m < endRow; m++) {
        for (int n = 0; n < result->cols; n++) {
            int value1 = (m < matrix1->rows && n < matrix1->cols) ? matrix1->data[m][n] : 0;
            int value2 = (m < matrix2->rows && n < matrix2->cols) ? matrix2->data[m][n] : 0;
            result->data[m][n] = value1 + value2;
        }
    }
    
    pthread_exit(NULL);
}

void* pthreadSubtraction(void* arg) {
    PthreadArgs* args = (PthreadArgs*)arg;
    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    int startRow = args->i;
    int endRow = args->j;
    
        // Perform matrix subtraction
    for (int m = startRow; m < endRow; m++) {
        for (int n = 0; n < result->cols; n++) {
            int value1 = (m < matrix1->rows && n < matrix1->cols) ? matrix1->data[m][n] : 0;
            int value2 = (m < matrix2->rows && n < matrix2->cols) ? matrix2->data[m][n] : 0;
            result->data[m][n] = value1 - value2;
        }
    }
    
    pthread_exit(NULL);
}


void* pthreadMultiplication(void* arg) {
    PthreadArgs* args = (PthreadArgs*)arg;
    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    int startRow = args->i;
    int endRow = args->j;
    
    for (int m = startRow; m < endRow; m++) {
        for (int n = 0; n < result->cols; n++) {
            int sum = 0;
            for (int k = 0; k < matrix1->cols; k++) {
                int value1 = (m < matrix1->rows && k < matrix1->cols) ? matrix1->data[m][k] : 0;
                int value2 = (k < matrix2->rows && n < matrix2->cols) ? matrix2->data[k][n] : 0;
                sum += value1 * value2;
            }
            result->data[m][n] = sum;
        }
    }
    pthread_exit(NULL);
}


void* matrixCalculation(void* arg) {
    int rc;
    MatrixArgs* args = (MatrixArgs*)arg;
    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    char operatorCh = args->operatorCh;
    char id = args->pthreadID;
    int subPthreadNum = args->subPthreadNum;
    PthreadArgs subArgs[subPthreadNum];
    pthread_t threads[subPthreadNum];
    int resultRows;
    int resultCols;
    if(operatorCh == '*'){
      // Define the dimensions of the result matrix
      resultRows = matrix1->rows;
      resultCols = matrix2->cols;
    }else {// For + and -
      // Determine the dimensions of the result matrix
      resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
      resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;
    }
    
    // Update the dimensions of the result matrix
    result->rows = resultRows;
    result->cols = resultCols;
    result->name = id;
    
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

    
    // Perform matrix Calculation
    int subPthreadCount = 0;
    int rowPerPthread = resultRows / subPthreadNum;
    if ((rowPerPthread > 0)&&(subPthreadNum > 1)){
    // Create and execute three threads
      for (int i = 1; i < subPthreadNum; i++) {
          subArgs[subPthreadCount].matrix1 = matrix1;
          subArgs[subPthreadCount].matrix2 = matrix2;
          subArgs[subPthreadCount].result = result;
          subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
          subArgs[subPthreadCount].j = subPthreadCount * rowPerPthread + rowPerPthread;
          
          if(operatorCh == '*'){
            rc = pthread_create(&threads[subPthreadCount], NULL, pthreadMultiplication, (void*)&subArgs[subPthreadCount]);
          }else if(operatorCh == '+'){
            rc = pthread_create(&threads[subPthreadCount], NULL, pthreadAddition, (void*)&subArgs[subPthreadCount]);       
          }else{
            rc = pthread_create(&threads[subPthreadCount], NULL, pthreadSubtraction, (void*)&subArgs[subPthreadCount]);   
          }
          if (rc) {
              printf("Error creating thread %d\n", subPthreadCount);
              pthread_exit(NULL);
          } 
          subPthreadCount ++;
      }
    }
    subArgs[subPthreadCount].matrix1 = matrix1;
    subArgs[subPthreadCount].matrix2 = matrix2;
    subArgs[subPthreadCount].result = result;
    subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
    subArgs[subPthreadCount].j = resultRows;
    if(operatorCh == '*'){
      rc = pthread_create(&threads[subPthreadCount], NULL, pthreadMultiplication, (void*)&subArgs[subPthreadCount]);
    }else if(operatorCh == '+'){
      rc = pthread_create(&threads[subPthreadCount], NULL, pthreadAddition, (void*)&subArgs[subPthreadCount]);        }else{
      rc = pthread_create(&threads[subPthreadCount], NULL, pthreadSubtraction, (void*)&subArgs[subPthreadCount]);     }
    if (rc) {
      printf("Error creating thread %d\n", subPthreadCount);
      pthread_exit(NULL);
    } 
    subPthreadCount ++;
    
    
    // Wait for all threads to complete
    for (int i = 0; i < subPthreadNum; i++) {
        pthread_join(threads[i], NULL);
    } 

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
// Print linkedlist
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

    //For pthread
    int tempcount = 0;
    int threadcount;
    int subPthreadSize;
    int rc;
    pthread_t threads[20];
    MatrixArgs args[20];
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
    for(int i=0;i<2;i++){
    
        //Set number of sub-phread that a phread can generate
        //Fair distribution. Update per loop
        //int(count / 2 +1): Num of letter; NUM_THREADS / 2: 2 letter with one operater
        subPthreadSize = NUM_THREADS / (2 * (int)(count / 2 +1));
        subPthreadSize = (subPthreadSize > 1) ? subPthreadSize : 1;
        //Clear threadcount
        threadcount = 0;
        
        //start from head+1
        current = head->right;
        //calculate multiplcation first
        while (current != NULL) {
            previous = current->left;
            next = current->right;
            if (current->data =='*'){
            
                //multplication with sub-pthread
                args[threadcount].matrix1 = &matrices[previous->data -'A'];
                args[threadcount].matrix2 = &matrices[next->data -'A'];
                args[threadcount].result = &matrices[temparr[tempcount]-'A'];
                args[threadcount].pthreadID = temparr[tempcount];
                args[threadcount].subPthreadNum = subPthreadSize;
                args[threadcount].operatorCh = current->data;
                rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
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
    }
    
    
    //Set number of sub-phread that a phread can generate
    //Fair distribution. Update per loop
    //int(count / 2 +1): Num of letter; NUM_THREADS / 2: 2 letter with one operater
    subPthreadSize = NUM_THREADS / (2 * (int)(count / 2 +1));
    subPthreadSize = (subPthreadSize > 1) ? subPthreadSize : 1;
    //Clear threadcount
    threadcount = 0;

    //start from head+1
    current = head->right;
    //calculate multiplcation first
    while (current != NULL) {
        previous = current->left;
        next = current->right;
        if (current->data =='*'){
            //multplication with sub-pthread
            args[threadcount].matrix1 = &matrices[previous->data -'A'];
            args[threadcount].matrix2 = &matrices[next->data -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].pthreadID = threadcount * subPthreadSize;
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = current->data;
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
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
            
            count -=2;
            //printLinkedList(head);
        }
        current = current->right;
    }
    

    //Set number of sub-phread that a phread can generate
    //Fair distribution. Update per loop
    //int(count / 2 +1): Num of letter; NUM_THREADS / 2: 2 letter with one operater
    subPthreadSize = NUM_THREADS / (2 * (int)(count / 2 +1));
    subPthreadSize = (subPthreadSize > 1) ? subPthreadSize : 1;
    //Clear threadcount
    threadcount = 0;
  
    //start from head+1
    current = head->right;
    //calculate Addition and Subtraction
    while (current != NULL) {
        previous = current->left;
        next = current->right;
        if ((current->data =='+') || (current->data =='-')){
        /////////////////////////////
            args[threadcount].matrix1 = &matrices[previous->data -'A'];
            args[threadcount].matrix2 = &matrices[next->data -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].pthreadID = threadcount * subPthreadSize;
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = current->data;
            //Addition        
            if (current->data =='+'){
              rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
                if (rc) {
                  printf("Error creating thread %d\n", threadcount);
                  return -1;
                }
              pthread_join(threads[threadcount], NULL);
              threadcount++;
            }else{
            //Subtraction
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
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

//void* matrixAddition(void* arg) {
//int rc;
//    MatrixArgs* args = (MatrixArgs*)arg;
//    Matrix* matrix1 = args->matrix1;
//    Matrix* matrix2 = args->matrix2;
//    Matrix* result = args->result;
//    char id = args->pthreadID;
//    int subPthreadNum = args->subPthreadNum;
//    PthreadArgs subArgs[subPthreadNum];
//    pthread_t threads[subPthreadNum];
//    
//    // Determine the dimensions of the result matrix
//    int resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
//    int resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;
//
//    // Allocate memory for the result matrix
//    result->data = (int**)malloc(resultRows * sizeof(int*));
//    if (result->data == NULL) {
//        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
//        pthread_exit(NULL);
//    }
//    for (int i = 0; i < resultRows; i++) {
//        result->data[i] = (int*)malloc(resultCols * sizeof(int));
//        if (result->data[i] == NULL) {
//            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
//            pthread_exit(NULL);
//        }
//    }
//
//    // Update the dimensions of the result matrix
//    result->rows = resultRows;
//    result->cols = resultCols;
//    result->name = id;
//
//
//    // Perform matrix subtraction
//    int subPthreadCount = 0;
//    int rowPerPthread = resultRows / subPthreadNum;
//    if ((rowPerPthread > 0)&&(subPthreadNum > 1)){
//    // Create and execute three threads
//      //printf("Create sub-pthread %c + %c: %d\n",matrix1->name,matrix2->name, subPthreadCount);
//      for (int i = 1; i < subPthreadNum; i++) {
//          subArgs[subPthreadCount].matrix1 = matrix1;
//          subArgs[subPthreadCount].matrix2 = matrix2;
//          subArgs[subPthreadCount].result = result;
//          subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
//          subArgs[subPthreadCount].j = subPthreadCount * rowPerPthread + rowPerPthread;
//          //printf("Create sub-pthread %c + %c (i=%d, j=%d) : %d\n",matrix1->name,matrix2->name,subArgs[subPthreadCount].i,subArgs[subPthreadCount].j, subPthreadCount);
//          rc = pthread_create(&threads[subPthreadCount], NULL, pthreadAddition, (void*)&subArgs[subPthreadCount]);
//          if (rc) {
//              printf("Error creating thread %d\n", subPthreadCount);
//              pthread_exit(NULL);
//          } 
//          subPthreadCount ++;
//      }
//    }
//    
//    subArgs[subPthreadCount].matrix1 = matrix1;
//    subArgs[subPthreadCount].matrix2 = matrix2;
//    subArgs[subPthreadCount].result = result;
//    subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
//    subArgs[subPthreadCount].j = resultRows;
//    //printf("Create sub-pthread %c + %c (i=%d, j=%d) : %d\n",matrix1->name,matrix2->name,subArgs[subPthreadCount].i,subArgs[subPthreadCount].j, subPthreadCount);
//    rc = pthread_create(&threads[subPthreadCount], NULL, pthreadAddition, (void*)&subArgs[subPthreadCount]);
//    if (rc) {
//      printf("Error creating thread %d\n", subPthreadCount);
//      pthread_exit(NULL);
//    } 
//    subPthreadCount ++;
//    
//    
//    // Wait for all threads to complete
//    for (int i = 0; i < subPthreadNum; i++) {
//        pthread_join(threads[i], NULL);
//    } 
//
//    pthread_exit(NULL);
//}
//
//void* matrixSubtraction(void* arg) {
//    int rc;
//    MatrixArgs* args = (MatrixArgs*)arg;
//    Matrix* matrix1 = args->matrix1;
//    Matrix* matrix2 = args->matrix2;
//    Matrix* result = args->result;
//    char id = args->pthreadID;
//    int subPthreadNum = args->subPthreadNum;
//    PthreadArgs subArgs[subPthreadNum];
//    pthread_t threads[subPthreadNum];
//    
//    // Determine the dimensions of the result matrix
//    int resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
//    int resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;
//
//    // Allocate memory for the result matrix
//    result->data = (int**)malloc(resultRows * sizeof(int*));
//    if (result->data == NULL) {
//        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
//        pthread_exit(NULL);
//    }
//    for (int i = 0; i < resultRows; i++) {
//        result->data[i] = (int*)malloc(resultCols * sizeof(int));
//        if (result->data[i] == NULL) {
//            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
//            pthread_exit(NULL);
//        }
//    }
//
//    // Update the dimensions of the result matrix
//    result->rows = resultRows;
//    result->cols = resultCols;
//    result->name = id;
//
//
//    // Perform matrix subtraction
//    int subPthreadCount = 0;
//    int rowPerPthread = resultRows / subPthreadNum;
//    if ((rowPerPthread > 0)&&(subPthreadNum > 1)){
//    // Create and execute three threads
//      //printf("Create sub-pthread %c - %c: %d\n",matrix1->name,matrix2->name, subPthreadCount);
//      for (int i = 1; i < subPthreadNum; i++) {
//          subArgs[subPthreadCount].matrix1 = matrix1;
//          subArgs[subPthreadCount].matrix2 = matrix2;
//          subArgs[subPthreadCount].result = result;
//          subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
//          subArgs[subPthreadCount].j = subPthreadCount * rowPerPthread + rowPerPthread;
//          //printf("Create sub-pthread %c - %c (i=%d, j=%d) : %d\n",matrix1->name,matrix2->name,subArgs[subPthreadCount].i,subArgs[subPthreadCount].j, subPthreadCount);
//          rc = pthread_create(&threads[subPthreadCount], NULL, pthreadSubtraction, (void*)&subArgs[subPthreadCount]);
//          if (rc) {
//              printf("Error creating thread %d\n", subPthreadCount);
//              pthread_exit(NULL);
//          } 
//          subPthreadCount ++;
//      }
//    }
//    
//    subArgs[subPthreadCount].matrix1 = matrix1;
//    subArgs[subPthreadCount].matrix2 = matrix2;
//    subArgs[subPthreadCount].result = result;
//    subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
//    subArgs[subPthreadCount].j = resultRows;
//    //printf("Create sub-pthread %c - %c (i=%d, j=%d) : %d\n",matrix1->name,matrix2->name,subArgs[subPthreadCount].i,subArgs[subPthreadCount].j, subPthreadCount);
//    rc = pthread_create(&threads[subPthreadCount], NULL, pthreadSubtraction, (void*)&subArgs[subPthreadCount]);
//    if (rc) {
//      printf("Error creating thread %d\n", subPthreadCount);
//      pthread_exit(NULL);
//    } 
//    subPthreadCount ++;
//    
//    
//    // Wait for all threads to complete
//    for (int i = 0; i < subPthreadNum; i++) {
//        pthread_join(threads[i], NULL);
//    } 
//
//    pthread_exit(NULL);
//}
//
//
//void* matrixMultiplication(void* arg) {
//    int rc;
//    MatrixArgs* args = (MatrixArgs*)arg;
//    Matrix* matrix1 = args->matrix1;
//    Matrix* matrix2 = args->matrix2;
//    Matrix* result = args->result;
//    char id = args->pthreadID;
//    int subPthreadNum = args->subPthreadNum;
//    PthreadArgs subArgs[subPthreadNum];
//    pthread_t threads[subPthreadNum];
//    
//    // Define the dimensions of the result matrix
//    int resultRows = matrix1->rows;
//    int resultCols = matrix2->cols;
//  
//    // Allocate memory for the result matrix
//    result->data = (int**)malloc(resultRows * sizeof(int*));
//    if (result->data == NULL) {
//        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
//        pthread_exit(NULL);
//    }
//    for (int i = 0; i < resultRows; i++) {
//        result->data[i] = (int*)malloc(resultCols * sizeof(int));
//        if (result->data[i] == NULL) {
//            fprintf(stderr, "Memory allocation failed for result matrix data[%d].\n", i);
//            pthread_exit(NULL);
//        }
//    }
//    // Update the dimensions of the result matrix
//    result->rows = resultRows;
//    result->cols = resultCols;
//    result->name = id;
//    
//    // Perform matrix multiplication
//    int subPthreadCount = 0;
//    int rowPerPthread = resultRows / subPthreadNum;
//    if ((rowPerPthread > 0)&&(subPthreadNum > 1)){
//    // Create and execute three threads
//      //printf("Create sub-pthread %c * %c: %d\n",matrix1->name,matrix2->name, subPthreadCount);
//      for (int i = 1; i < subPthreadNum; i++) {
//          subArgs[subPthreadCount].matrix1 = matrix1;
//          subArgs[subPthreadCount].matrix2 = matrix2;
//          subArgs[subPthreadCount].result = result;
//          subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
//          subArgs[subPthreadCount].j = subPthreadCount * rowPerPthread + rowPerPthread;
//          //printf("Create sub-pthread %c * %c (i=%d, j=%d) : %d\n",matrix1->name,matrix2->name,subArgs[subPthreadCount].i,subArgs[subPthreadCount].j, subPthreadCount);
//          rc = pthread_create(&threads[subPthreadCount], NULL, pthreadMultiplication, (void*)&subArgs[subPthreadCount]);
//          if (rc) {
//              printf("Error creating thread %d\n", subPthreadCount);
//              pthread_exit(NULL);
//          } 
//          subPthreadCount ++;
//      }
//    }
//    
//    subArgs[subPthreadCount].matrix1 = matrix1;
//    subArgs[subPthreadCount].matrix2 = matrix2;
//    subArgs[subPthreadCount].result = result;
//    subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
//    subArgs[subPthreadCount].j = resultRows;
//    //printf("Create sub-pthread %c * %c (i=%d, j=%d) : %d\n",matrix1->name,matrix2->name,subArgs[subPthreadCount].i,subArgs[subPthreadCount].j, subPthreadCount);
//    rc = pthread_create(&threads[subPthreadCount], NULL, pthreadMultiplication, (void*)&subArgs[subPthreadCount]);
//    if (rc) {
//      printf("Error creating thread %d\n", subPthreadCount);
//      pthread_exit(NULL);
//    } 
//    subPthreadCount ++;
//    
//    
//    // Wait for all threads to complete
//    for (int i = 0; i < subPthreadNum; i++) {
//        pthread_join(threads[i], NULL);
//    } 
//
//    pthread_exit(NULL);
//}

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
//For testing
#include<time.h>
////////////////////////////////////////////////////////////////////////////////

////////////////////////////GLOBAL VARIABLES/STRUCTS////////////////////////////
typedef struct {
    char name;
    int rows;
    int cols;
    int* data;
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
    char resultID;
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

#define NUM_THREADS 45
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////FUNCTIONS///////////////////////////////////////
int readMatrix(Matrix* matrix) {
    char c;
    matrix->data = (int*)malloc(matrix->rows * matrix->cols * sizeof(int));
    if (matrix->data == NULL) {
        printf("Memory allocation failed for matrix data.\n");
        return -1;
    }

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            if (scanf("%d", &matrix->data[i * matrix->cols + j]) != 1) {
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
//        for (int i = 0; i < matrix->rows; i++) {
//            free(matrix->data[i]);
//        }
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
    int resultCol = result->cols;
    int resultRow = result->rows;
    int m1Row = matrix1->rows;
    int m1Col = matrix1->cols;
    int m2Row = matrix2->rows;
    int m2Col = matrix2->cols;
        
        // Perform matrix addition
    for (int m = startRow; m < endRow; m++) {
        for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            result->data[m* resultCol + n] = value1 + value2;
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
    int resultCol = result->cols;
    int resultRow = result->rows;
    int m1Row = matrix1->rows;
    int m1Col = matrix1->cols;
    int m2Row = matrix2->rows;
    int m2Col = matrix2->cols;
        
        // Perform matrix subtraction
    for (int m = startRow; m < endRow; m++) {
        for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            result->data[m* resultCol + n] = value1 - value2;
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
    int resultCol = result->cols;
    int resultRow = result->rows;
    int m1Row = matrix1->rows;
    int m1Col = matrix1->cols;
    int m2Row = matrix2->rows;
    int m2Col = matrix2->cols;
    
    for (int m = startRow; m < endRow; m++) {
        for (int k = 0; k < matrix1->cols; k++){
            int value1 = (m < m1Row && k < m1Col) ? matrix1->data[m * m1Col + k] : 0;
            for (int n = 0; n < resultCol; n++)  {
                int value2 = (k < m2Row && n < m2Col) ? matrix2->data[k * m2Col + n] : 0;
                result->data[m * resultCol + n] += value1 * value2;
            }
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
    char id = args->resultID;
    int subPthreadNum = args->subPthreadNum;
    int resultRows;
    int resultCols;
    
//    clock_t start, end;
//    double cpu_time_used;
//    //start = clock(); // Start the timer
//    for(int redo = 0; redo < 3; redo ++){
//    start = clock(); // Start the timer
    
    if(operatorCh == '*'){
      // Define the dimensions of the result matrix
      resultRows = matrix1->rows;
      resultCols = matrix2->cols;
      subPthreadNum = 5;
    }else {// For + and -
      // Determine the dimensions of the result matrix
      resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
      resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;
      subPthreadNum = 1;
    }
    
    // Update the dimensions of the result matrix
    result->rows = resultRows;
    result->cols = resultCols;
    result->name = id;
    
    // Allocate memory for the result matrix
    result->data = (int*)malloc(resultRows * resultCols * sizeof(int*));
    if (result->data == NULL) {
        fprintf(stderr, "Memory allocation failed for result matrix data.\n");
        pthread_exit(NULL);
    }
    
    if(subPthreadNum ==0){
      subPthreadNum = 1;
    }
    
    
    PthreadArgs subArgs[subPthreadNum];
    pthread_t threads[subPthreadNum];    
    
    
    /////////////////////////////////////CALL THREADS/////////////////////////////////////////////////
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
    /////////////////////////////////////CALL THREADS/////////////////////////////////////////////////
    
    // Wait for all threads to complete
    for (int i = 0; i < subPthreadNum; i++) {
        pthread_join(threads[i], NULL);
    } 
    

//    //redo }
//    end = clock(); // Stop the timer
//    cpu_time_used += ((double) (end - start)) / CLOCKS_PER_SEC;
//    }
//    //end = clock(); // Stop the timer
//    //cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    cpu_time_used = cpu_time_used / 3;
//    printf("Execution Time: %d Thread  %c[%d][%d] = %c[%d][%d] %c %c[%d][%d]  %f seconds\n",subPthreadNum,result->name, result->rows,result->cols,matrix1->name, matrix1->rows,matrix1->cols,operatorCh,matrix2->name, matrix2->rows,matrix2->cols,cpu_time_used);

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

int Operation_logic(char expression[50], Matrix* matrices)
{
    //For pthread
    int threadcount;
    int subPthreadSize;
    int rc;
    pthread_t threads[20];
    MatrixArgs args[20];
    
    int count=0;
    int ptr;    // this is current in list node(should be)
    int multicount=0; // (useless) count the number of * 
    int firstL=1;    //boolean for don't do in first loop
    int lockk=0;
    //printf("Hello World");
    //char expression[50]={"A*B+C+D*E-F+G"}; //already have
    //char expression[50]={"A*B*C"}; //temp
    char temparr[50]={"KLMNOPQRST"};    //temparr
    char waitlist[50];
    int waitloc[10];
    int AtHead,AtTail;
    int Lmulti=0;
    int waitcount=0;
    int tempcount=0;
    while(expression[count] !='\0'){
        if (expression[count]=='*')
            multicount++;
        count++;
    }
    
    //printf("%s",expression);
    //printf("\n%d *%d\n",count,multicount);
    //Set number of sub-phread that a phread can generate
    //Fair distribution. Update per loop
    //int(count / 2 +1): Num of letter; NUM_THREADS / 2: 2 letter with one operater
//    subPthreadSize = NUM_THREADS / (2 * (int)(count / 2 +1));
//    subPthreadSize = (subPthreadSize > 1) ? subPthreadSize : 1;
    //Clear threadcount
    threadcount = 0;
    ptr=1;
    while(ptr<count){
        Lmulti=0;  //0 dont lock multi
        if (expression[ptr]=='*'){
            Lmulti=1; //locked
            if(firstL==1)
                lockk=1;
            // Wait for the threads to finish
            for (int t = 0; t < threadcount; t++) {
              rc = pthread_join(threads[t], NULL);
                if (rc) {
                  printf("Error joining thread %d\n", t);
                  return -1;
                }
            }             
            
            
//    clock_t start, end;
//    double cpu_time_used;
//    start = clock(); // Start the timer
//    for(int redo = 0; redo < 3; redo ++){
//    start = clock(); // Start the timer    

 
    
            //Clear threadcount
            threadcount = 0;                
            args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
            args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = '*';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
//            //thread num +1
//            threadcount++; 
            pthread_join(threads[0], NULL);     
            
//    end = clock(); // Stop the timer
//    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    cpu_time_used = cpu_time_used / 3;
//    printf("Execution Time: %d Thread  %c[%d][%d] = %c[%d][%d] %c %c[%d][%d]  %f seconds\n",args[threadcount].subPthreadNum,args[threadcount].result->name, args[threadcount].result->rows,args[threadcount].result->cols,args[threadcount].matrix1->name, args[threadcount].matrix1->rows,args[threadcount].matrix1->cols,args[threadcount].operatorCh,args[threadcount].matrix2->name, args[threadcount].matrix2->rows,args[threadcount].matrix2->cols,cpu_time_used);             
            
            
            
            
            
            //Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            expression[ptr-1]=temparr[tempcount++];
            for(int i=ptr;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            count-=2;
            //printf("\n%s",expression);
            if(count==1){
              break;
            }
        }
        
        if (ptr==1&&(lockk==0)){  //read head //
            if (expression[ptr]!='*'){
                waitlist[waitcount++]='+';
                waitloc[waitcount/2]=0;
                waitlist[waitcount++]=expression[0];
                ptr+=2;
                //printf("  waitlist %s",waitlist);
            
                //if ((expression[ptr]!='*')&&(expression[ptr+2]!='*')){
                if ((expression[ptr]!='*')){
                    waitlist[waitcount++]=expression[1];
                    waitloc[waitcount/2]=2;
                    waitlist[waitcount++]=expression[2];
                    //printf("  waitlist %s",waitlist);
                }
            }   
        }
        
        if ((expression[ptr]=='*')&&(Lmulti==0)){
            if(firstL==1)
                lockk=1;
            // Wait for the threads to finish
            for (int t = 0; t < threadcount; t++) {
              rc = pthread_join(threads[t], NULL);
                if (rc) {
                  printf("Error joining thread %d\n", t);
                  return -1;
                }
            } 
                     
//    clock_t start, end;
//    double cpu_time_used;
//    start = clock(); // Start the timer
//    for(int redo = 0; redo < 3; redo ++){
//    start = clock(); // Start the timer  
                          
            //Clear threadcount
            threadcount = 0;                
            args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
            args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = '*';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            
//            //thread num +1
//            threadcount++; 
            pthread_join(threads[0], NULL);
                      
                     
//    end = clock(); // Stop the timer
//    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    cpu_time_used = cpu_time_used / 3;
//    printf("Execution Time: %d Thread  %c[%d][%d] = %c[%d][%d] %c %c[%d][%d]  %f seconds\n",args[threadcount].subPthreadNum,args[threadcount].result->name, args[threadcount].result->rows,args[threadcount].result->cols,args[threadcount].matrix1->name, args[threadcount].matrix1->rows,args[threadcount].matrix1->cols,args[threadcount].operatorCh,args[threadcount].matrix2->name, args[threadcount].matrix2->rows,args[threadcount].matrix2->cols,cpu_time_used);                        
                      
                      
            //Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            expression[ptr-1]=temparr[tempcount++];
            for(int i=ptr;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            count-=2;
            //printf("\n%s",expression);
            if(count==1){            
                break;
            }
        }
        
        if (waitcount==4){ //++ or   -- == -(+)
            if(waitlist[1]==waitlist[3]){
                waitcount=0;
                //printf("\nClear\n");
                continue;
            }
            if(waitlist[0]==waitlist[2]){
            
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = '+';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            //thread num +1
            threadcount++;            
            
                //Padd(waitlist[1],waitlist[3],temparr[tempcount]);
                
            }
            else{         //+- or -+ == -(-)
            
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = '-';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            //thread num +1
            threadcount++;  
            
                //Psub(waitlist[1],waitlist[3],temparr[tempcount]);
            }
             //printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]);
            if(waitloc[0]<waitloc[1]){
                AtHead=waitloc[0];
                AtTail=waitloc[1];
            }
            else{
                AtHead=waitloc[1];
                AtTail=waitloc[0];
            }
            expression[AtHead]=temparr[tempcount++];
            for(int i=AtTail-1;i<count+1;i++){
                    expression[i]=expression[i+2];
            }
            
            waitcount=0;
            //printf("after waitlist:\n%s",expression);
            count-=2;
            ptr-=2;
            if(count==1){
                break;}
        }
        
        if ((expression[ptr]=='+')||(expression[ptr]=='-')){
            if (expression[ptr+2]!='*'){
                waitlist[waitcount++]=expression[ptr];
                waitloc[waitcount/2]=(ptr+1);
                waitlist[waitcount++]=expression[ptr+1];
                //ptr+=2;
                //printf(" ptr:%d waitlist %s loc0:%d  1:%d",ptr,waitlist,waitloc[0],waitloc[1]);
            }
        }
        if (waitcount==4){ //++ or   -- == -(+)
            if(waitlist[1]==waitlist[3]){
                waitcount=0;
                printf("\nClear\n");
                continue;
            }
            if(waitlist[0]==waitlist[2]){

            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = '+';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            //thread num +1
            threadcount++;  
 
                //Padd(waitlist[1],waitlist[3],temparr[tempcount]);
            }
            else{         //+- or -+ == -(-)
            
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = subPthreadSize;
            args[threadcount].operatorCh = '-';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            //thread num +1
            threadcount++;  
             
                //Psub(waitlist[1],waitlist[3],temparr[tempcount]);
            }
             //printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]);
            if(waitloc[0]<waitloc[1]){
                AtHead=waitloc[0];
                AtTail=waitloc[1];
            }
            else{
                AtHead=waitloc[1];
                AtTail=waitloc[0];
            }
            expression[AtHead]=temparr[tempcount++];
            for(int i=AtTail-1;i<count+1;i++){
                    expression[i]=expression[i+2];
            }
            
            waitcount=0;
            //printf("after waitlist:\n%s\n",expression);
            count-=2;
            ptr-=2;
            if(count==1){
                break;}
        }
        
        ptr+=2;
        if(count==1){
            break;}
        if(firstL==1){
            firstL=0;
            lockk=0;
        }
        if(ptr>=count){ //if(current ==NULL)
            ptr=1;      //current= head->right;
                        //joinPthread here 
                        
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
            //Set number of sub-phread that a phread can generate
            //Fair distribution. Update per loop
            //int(count / 2 +1): Num of letter; NUM_THREADS / 2: 2 letter with one operater
//            subPthreadSize = NUM_THREADS / (2 * (int)(count / 2 +1));
//            subPthreadSize = (subPthreadSize > 1) ? subPthreadSize : 1;
  
            firstL=1;
            waitcount=0;
            //printf("\n\nWaiting all pthread\n\n");
        }
        
        //printf("\n\n%s ,%d  !%d",expression,count,ptr);

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
    return (expression[0] - 'A');
}

void printMatrix(Matrix* matrix) {
    clock_t start, end;
    double cpu_time_used;
    start = clock(); // Start the timer
//    for(int redo = 0; redo < 3; redo ++){
//    start = clock(); // Start the timer                        
  
                      
    //printf("Matrix %c:\n", matrix->name);
    printf("%d",matrix->rows);
    printf("  %d\n", matrix->cols);
    for (int j = 0; j < matrix->rows; j++) {
      for (int k = 0; k < matrix->cols; k++) {
        printf("%d	", matrix->data[j * matrix->cols + k]);
      }
      printf("\n");
    }
    end = clock(); // Stop the timer
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    cpu_time_used = cpu_time_used / 3;
    printf("Execution Time of Printing Maxtrix: %f seconds\n", cpu_time_used); 
}
                 
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////MAIN FUNCTION//////////////////////////////////

int main() {
    char expression[50];
    int n, m;
    int numMatrices = 20;
    Matrix* matrices;
    
//    clock_t start, end;
//    double cpu_time_used;
//    start = clock(); // Start the timer
//    
    // Read the expression and matrix dimensions
    if (scanf("%[^\n]%*c", expression) != 1) {
        printf("Failed to read the expression.\n");
        return 1;
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
            }
            matrices[matrixIndex].rows = n;
            matrices[matrixIndex].cols = m;
            matrices[matrixIndex].name = matrixIndex + 'A';
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

//    end = clock(); // Stop the timer
//    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    printf("Execution Time of Reading Maxtrix: %f seconds\n", cpu_time_used);

//////////////////////////////CALL FUNCTION HERE/////////////////////////////////////////
  //int temp = expressionInterpretation(expression,matrices);
  int temp = Operation_logic(expression,matrices);
  //printf("Output: %int\n  in char: %c",temp,temp+'A');
  if (temp == -1) {
    printf("Error");
    return 0;
  }
//////////////////////////////OUTPUT//////////////////////////////////////////
    printMatrix(&matrices[temp]);


//    start = clock(); // Start the timer

    //Free the allocated memory
    for (int i = 0; i < numMatrices; i++) {
        freeMatrix(&matrices[i]);
    }
    free(matrices);

//    end = clock(); // Stop the timer
//    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
//    printf("Execution Time of Clean Maxtrix: %f seconds\n", cpu_time_used);


    return 0;
}
////////////////////////////////////////////////////////////////////////////////

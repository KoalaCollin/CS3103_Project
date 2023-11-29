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
#include <sys/time.h>
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
    int printOut;
} MatrixArgs;

typedef struct {
    Matrix* matrix1;
    Matrix* matrix2;
    Matrix* result;
    char operatorCh;
    int i;
    int j;
    int printOut;
} PthreadArgs;
//
// Global variable
//int runningThreads = 0; 
//int waitingThreads = 0;
int lastCalculation = 0;

//// Mutex lock
//pthread_mutex_t runninglock; 
//pthread_mutex_t waitinglock;


#define NUM_THREADS 5
#define Multiple_SUB 5
#define NonMultiple_SUB 3
#define TESTMODE 0
#define block_size 20
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


void* pthreadNonMultiplication(void* arg) {
    struct timeval start, end;
    double cpu_time_used;
    gettimeofday(&start, NULL);
    PthreadArgs* args = (PthreadArgs*)arg;
    Matrix* matrix1 = args->matrix1;
    Matrix* matrix2 = args->matrix2;
    Matrix* result = args->result;
    char operatorCh = args->operatorCh;
    int startRow = args->i;
    int endRow = args->j;
    int resultCol = result->cols;
    //int resultRow = result->rows;
    int m1Row = matrix1->rows;
    int m1Col = matrix1->cols;
    int m2Row = matrix2->rows;
    int m2Col = matrix2->cols; 
    int printOut = args->printOut;     
     
    if(TESTMODE){
    if(printOut == 1){
      printf("%d",result->rows);
      printf("  %d\n", result->cols);
      // Perform matrix addition
      if(operatorCh == '+'){
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
              int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
              int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
              printf("%d  ", value1 + value2);
          }
          printf("\n"); 
        }
      }else{
        // Perform matrix subtraction
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            printf("%d  ", value1 - value2);
          } 
        printf("\n");
        }
      }    
    }else{
      // Perform matrix addition
      if(operatorCh == '+'){
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
              int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
              int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
              result->data[m* resultCol + n] = value1 + value2;
          }
        }
      }else{
        // Perform matrix subtraction
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            result->data[m* resultCol + n] = value1 - value2;
          } 
        }
      }
    }    
    
    
    }else{
    if(printOut == 1){
      printf("%d",result->rows);
      printf("  %d\n", result->cols);
      // Perform matrix addition
      if(operatorCh == '+'){
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
              int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
              int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
              printf("%d	", value1 + value2);
          }
          printf("\n"); 
        }
      }else{
        // Perform matrix subtraction
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            printf("%d	", value1 - value2);
          } 
        printf("\n");
        }
      }    
    }else{
      // Perform matrix addition
      if(operatorCh == '+'){
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
              int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
              int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
              result->data[m* resultCol + n] = value1 + value2;
          }
        }
      }else{
        // Perform matrix subtraction
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            result->data[m* resultCol + n] = value1 - value2;
          } 
        }
      }
    }
    }
    if(TESTMODE){ 
    if(printOut == 1){
         printf("\n %c Print Out Here! \n",result->name);
    } 
    gettimeofday(&end, NULL);
    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution Time: %d Thread  %c = %c %c %c  %f seconds\n",startRow,result->name,matrix1->name,operatorCh,matrix2->name,cpu_time_used);
    }    
    
    pthread_exit(NULL);
}


void* pthreadBlockMultiplication(void* arg) {
    struct timeval start, end;
    double cpu_time_used;
    gettimeofday(&start, NULL);
    

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

    //int rows_block = (m1Row + block_size - 1) / block_size;
    //int cols_block = (m2Col + block_size - 1) / block_size;

    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < m2Col; j++) {
            for (int k = 0; k < m1Col; k++) {
                for (int ii = i * block_size; ii < (i + 1) * block_size && ii < m1Row; ii++) {
                    for (int jj = j * block_size; jj < (j + 1) * block_size && jj < m2Col; jj++) {
                        for (int kk = k * block_size; kk < (k + 1) * block_size && kk < m1Col; kk++) {
                        result->data[ii * resultCol + jj] += matrix1->data[ii * m1Col + kk] * matrix2->data[kk * m2Col + jj];
                        }
                    }
                }
            }
        }
    }
    
    if(TESTMODE){ 
    
    gettimeofday(&end, NULL);
    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution Time: %d Thread  %c = %c %c %c  %f seconds\n",startRow,result->name,matrix1->name,'*',matrix2->name,cpu_time_used);
    }
    pthread_exit(NULL);
}


void* pthreadMultiplication(void* arg) {
    struct timeval start, end;
    double cpu_time_used;
    gettimeofday(&start, NULL);
    

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
    //int temparray[(endRow - startRow)*resultCol];
       
    for (int m = startRow; m < endRow; m++) {
        for (int k = 0; k < matrix1->cols; k++){
            int value1 = (m < m1Row && k < m1Col) ? matrix1->data[m * m1Col + k] : 0;
            for (int n = 0; n < resultCol; n++)  {
                int value2 = (k < m2Row && n < m2Col) ? matrix2->data[k * m2Col + n] : 0;
                result->data[m * resultCol + n] += value1 * value2;
            }
        }
    }

    if(TESTMODE){ 
    
    gettimeofday(&end, NULL);
    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution Time: %d Thread  %c = %c %c %c  %f seconds\n",startRow,result->name,matrix1->name,'*',matrix2->name,cpu_time_used);
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
    int printOut = args->printOut;

    struct timeval start, end;
    double cpu_time_used;
    if(TESTMODE){
    gettimeofday(&start, NULL);
    }
    
    if(operatorCh == '*'){
      // Define the dimensions of the result matrix
      resultRows = matrix1->rows;
      resultCols = matrix2->cols;
      subPthreadNum = Multiple_SUB;
    }else {// For + and -
      // Determine the dimensions of the result matrix
      resultRows = (matrix1->rows > matrix2->rows) ? matrix1->rows : matrix2->rows;
      resultCols = (matrix1->cols > matrix2->cols) ? matrix1->cols : matrix2->cols;
      if(printOut == 1){
        subPthreadNum = 1;
      }else{
        subPthreadNum = NonMultiple_SUB;
      }
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
    int rowPerPthread;
    int subPthreadCount = 0;
    
    if(operatorCh == '*'){
      int rows_C = (matrix1->rows + block_size - 1) / block_size;
      int cols_C = (matrix2->cols + block_size - 1) / block_size;
      rowPerPthread = rows_C / subPthreadNum;
    }else{
      rowPerPthread = resultRows / subPthreadNum;
    }
    if ((rowPerPthread > 0)&&(subPthreadNum > 1)){
    // Create and execute three threads
      for (int i = 1; i < subPthreadNum; i++) {
          subArgs[subPthreadCount].matrix1 = matrix1;
          subArgs[subPthreadCount].matrix2 = matrix2;
          subArgs[subPthreadCount].result = result;
          subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
          subArgs[subPthreadCount].j = subPthreadCount * rowPerPthread + rowPerPthread;
          subArgs[subPthreadCount].operatorCh = operatorCh;
          
          if(operatorCh == '*'){
            rc = pthread_create(&threads[subPthreadCount], NULL, pthreadBlockMultiplication, (void*)&subArgs[subPthreadCount]);
          }else{
            rc = pthread_create(&threads[subPthreadCount], NULL, pthreadNonMultiplication, (void*)&subArgs[subPthreadCount]);   
          }
          if (rc) {
              printf("Error creating thread %d\n", subPthreadCount);
              pthread_exit(NULL);
          } 
          subPthreadCount ++;
      }
    }
    
    if(printOut == 1){
        subArgs[0].printOut = printOut;
      } 
    subArgs[subPthreadCount].matrix1 = matrix1;
    subArgs[subPthreadCount].matrix2 = matrix2;
    subArgs[subPthreadCount].result = result;
    subArgs[subPthreadCount].i = subPthreadCount * rowPerPthread;
    subArgs[subPthreadCount].j = resultRows;
    subArgs[subPthreadCount].operatorCh = operatorCh;
    if(operatorCh == '*'){
      rc = pthread_create(&threads[subPthreadCount], NULL, pthreadBlockMultiplication, (void*)&subArgs[subPthreadCount]);
    }else{
      rc = pthread_create(&threads[subPthreadCount], NULL, pthreadNonMultiplication, (void*)&subArgs[subPthreadCount]);     
    }
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
  //    }else{
  //    ///////////2d//////////////////////////////////////////////////////////////
  //    subArgs[subPthreadCount].matrix1 = matrix1;
  //    subArgs[subPthreadCount].matrix2 = matrix2;
  //    subArgs[subPthreadCount].result = result;     
  //    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    if(TESTMODE){
      gettimeofday(&end, NULL);

      cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
      printf("Execution Time: %d Thread  %c[%d][%d] = %c[%d][%d] %c %c[%d][%d]  %f seconds\n",subPthreadNum,result->name, result->rows,result->cols,matrix1->name, matrix1->rows,matrix1->cols,operatorCh,matrix2->name, matrix2->rows,matrix2->cols,cpu_time_used);
    
    }
    
    //release memory
    //free(matrix1->data);
    //free(matrix2->data);
    
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

int Operation_logic(char expression[50], Matrix* matrices){
    //For pthread
    int threadcount;
    //int subPthreadSize;
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
            //Clear threadcount
            threadcount = 0;             
   
            args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
            args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = 1;
            args[threadcount].operatorCh = '*';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
  //            //thread num +1
            threadcount++; 
  //            pthread_join(threads[0], NULL);     
            
            
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
  //              Clear threadcount
              threadcount = 0;      
                          
               
            args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
            args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = 1;
            args[threadcount].operatorCh = '*';
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
            }
            
  //            //thread num +1
 //           threadcount++; 
            pthread_join(threads[0], NULL);
                      
                      
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
            ////ready to print in non_multiplication
            if(count==3){
              lastCalculation = 1;
              args[threadcount].printOut = 1;
              if (TESTMODE){
                printf("\nREADY TO PRINT!\n");
              }
            }            
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum =1;
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
            ////ready to print in non_multiplication
            if(count==3){
              lastCalculation = 1;
              args[threadcount].printOut = 1;
              if (TESTMODE){
                printf("\nREADY TO PRINT!\n");
              }              
            }            
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum =1;
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
            ////ready to print in non_multiplication
            if(count==3){
              lastCalculation = 1;
              args[threadcount].printOut = 1;
              if (TESTMODE){
                printf("\nREADY TO PRINT!\n");
              }              
            }
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = 1;
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
            ////ready to print in non_multiplication
            if(count==3){
              lastCalculation = 1;
              args[threadcount].printOut = 1;
              if (TESTMODE){
                printf("\nREADY TO PRINT!\n");
              }
            }            
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].subPthreadNum = 1;
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
    if(TESTMODE){
    struct timeval start, end;
    double cpu_time_used;
    gettimeofday(&start, NULL);                 
    printf("Matrix %c:\n", matrix->name);
    printf("%d",matrix->rows);
    printf("  %d\n", matrix->cols);
    for (int j = 0; j < matrix->rows; j++) {
      for (int k = 0; k < matrix->cols; k++) {
          printf("%d", matrix->data[j * matrix->cols + k]);
      }
    }
    printf("\n");
    printf("\n");
    gettimeofday(&end, NULL);
    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution Time of Printing Maxtrix: %f seconds\n", cpu_time_used); 
    
    }else{              
    //printf("Matrix %c:\n", matrix->name);
    printf("%d",matrix->rows);
    printf("  %d\n", matrix->cols);
    for (int j = 0; j < matrix->rows; j++) {
      for (int k = 0; k < matrix->cols; k++) {
        printf("%d	", matrix->data[j * matrix->cols + k]);
      }
      printf("\n");
    }
    }
 }                
////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////MAIN FUNCTION//////////////////////////////////

int main() {
    char expression[50];
    int n, m;
    int numMatrices = 20;
    Matrix* matrices;
    struct timeval start, end;
    double cpu_time_used;
    gettimeofday(&start, NULL); 
    
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
    if(TESTMODE){
      gettimeofday(&end, NULL);
      cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
      printf("Execution Time of Reading Maxtrix: %f seconds\n", cpu_time_used);
    }
  //////////////////////////////CALL FUNCTION HERE/////////////////////////////////////////
  //int temp = expressionInterpretation(expression,matrices);
    int temp = Operation_logic(expression,matrices);
  //printf("Output: %int\n  in char: %c",temp,temp+'A');
    if (temp == -1) {
    printf("Error");
    return 0;
    }
  //////////////////////////////OUTPUT//////////////////////////////////////////
    //Use this function only if the last one is not non multplication
    if(lastCalculation == 0){
    printMatrix(&matrices[temp]);
    }


  //    start = clock(); // Start the timer

    //Free the allocated memory
  //    for (int i = 0; i < numMatrices; i++) {
  //        freeMatrix(&matrices[i]);
  //    }
  //    free(matrices);

    if(TESTMODE){
    gettimeofday(&end, NULL);
    cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
  //    end = clock(); // Stop the timer
  //    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Total Execution Time: %f seconds\n", cpu_time_used);
  }

    return 0;
}
////////////////////////////////////////////////////////////////////////////////

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
    int standby;
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
int runningThreads = 0; 
//int waitingThreads = 0;
int lastCalculation = 0;

//// Mutex lock
pthread_mutex_t runninglock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t waitinglock;
pthread_cond_t runningcond = PTHREAD_COND_INITIALIZER;

#define MAX_THREADS 5
#define Multiple_SUB 5
#define NonMultiple_SUB 3
#define TESTMODE 0
#define block_size 25
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////FUNCTIONS///////////////////////////////////////
void creatMatrix(Matrix* matrices, int matrixIndex) {
  int n,m;
  if (scanf("%d %d", &n, &m) != 2) {
      printf("Failed to read n and m.\n");
      return -1;
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
      //free(matrices);
      return -1;
    }
    matrices[matrixIndex].standby = 1;
    pthread_cond_signal(&runningcond);
}

int readMatrix(Matrix* matrix) {
    char c;
    matrix->data = (int*)malloc(matrix->rows * matrix->cols * sizeof(int));
    if (matrix->data == NULL) {
        printf("Memory allocation failed for matrix data.\n");
        return -1;
    }

    // for (int i = 0; i < matrix->rows; i++) {
    //     for (int j = 0; j < matrix->cols; j++) {
    //         if (scanf("%d", &matrix->data[i * matrix->cols + j]) != 1) {
    //             printf("Failed to read element at row %d, column %d.\n", i + 1, j + 1);
    //             return -1;
    //         }
    //     }
    //     while ((c = getchar()) != '\n' && c != EOF) {
    //         // Skip the remaining characters on the current line
    //     }
    // }

    char line[sizeof(int) * matrix->cols];

    for (int i = 0; i < matrix->rows; i++) {
      if (fgets(line, sizeof(line), stdin) == NULL) {
          printf("Failed to read line %d from the input.\n", i + 1);
          return -1;
      } 
      char *token = strtok(line, "	");
      for (int j = 0; j < matrix->cols; j++) {
      if (token == NULL) {
          printf("Failed to read element at row %d, column %d.\n", i + 1, j + 1);
          return -1;
      }

      matrix->data[i * matrix->cols + j] = atoi(token);
      token = strtok(NULL, "	");
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
      lastCalculation = 1;
      // Perform matrix addition
      if(operatorCh == '+'){
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
              int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
              int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
              //printf("%d  ", value1 + value2);
          }
          //printf("\n"); 
        }
      }else{
        // Perform matrix subtraction
        for (int m = startRow; m < endRow; m++) {
          for (int n = 0; n < resultCol; n++) {
            int value1 = (m < m1Row && n < m1Col) ? matrix1->data[m * m1Col + n] : 0;
            int value2 = (m < m2Row && n < m2Col) ? matrix2->data[m * m2Col + n] : 0;
            //printf("%d  ", value1 - value2);
          } 
        //printf("\n");
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
      lastCalculation = 1;
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
    //end subthread  
    pthread_mutex_lock(&runninglock);
    runningThreads--;
    pthread_cond_signal(&runningcond);
    pthread_mutex_unlock(&runninglock);
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
    //end subthread  
    pthread_mutex_lock(&runninglock);
    runningThreads--;
    pthread_cond_signal(&runningcond);
    pthread_mutex_unlock(&runninglock);
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

    //wait if matrices is not stand by
    pthread_mutex_lock(&runninglock);
    while ((matrix1->standby != 1)||(matrix2->standby !=1)) {
        pthread_cond_wait(&runningcond, &runninglock);
    }
    pthread_mutex_unlock(&runninglock);
    if(TESTMODE){
      printf("matrix %c %c Done, start calculation.",matrix1->name,matrix2->name);
    }
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
          //wait for create
          pthread_mutex_lock(&runninglock);
          while (runningThreads >= MAX_THREADS) {
            pthread_cond_wait(&runningcond, &runninglock);
          }
          runningThreads ++;
          if(TESTMODE){
          printf("thread:%d\n",runningThreads);
          }
          pthread_mutex_unlock(&runninglock);

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

    //wait for create
    pthread_mutex_lock(&runninglock);
    while (runningThreads >= MAX_THREADS) {
      pthread_cond_wait(&runningcond, &runninglock);
    }
    runningThreads ++;
    pthread_mutex_unlock(&runninglock);

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
    result->standby = 1;
    pthread_cond_signal(&runningcond);
    pthread_exit(NULL);
    
}

int Operation_logic(char expression[50], Matrix* matrices,int needle){
    //For pthread
    int threadcount;
    //int subPthreadSize;
    int rc;
    pthread_t threads[20];
    MatrixArgs args[20];
    
    int count=0;
    int ptr=1;       //pointer point to the operator in expression
    int lockfirst=0; //boolean for don't use first element twice
    char temparr[50]={"KLMNOPQRST"};    //array that store temporary matrix
    char waitlist[50];
    int waitloc[10];  //store the location of the elements in waitlist
    char operatorr;
    int waitcount=0; //count how many elements are in waitlist
    int tempcount=needle; //the number of used temporary matrix
    while(expression[count] !='\0'){
        count++;
    }
    //Clear threadcount
    threadcount = 0;
    ptr=1;
    while(count>1){
        lockfirst=0;
        if(TESTMODE){
        printf("\n\n%s ,%dleft  ptr%d",expression,count,ptr);
        }
        if(ptr==1){
            if (expression[ptr]=='*'){
                lockfirst=1;   //do not allow second call in this loop(need to wait the result of excute)
                //Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
                  args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
                  args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
                  args[threadcount].result = &matrices[temparr[tempcount]-'A'];
                  args[threadcount].resultID = temparr[tempcount];
                  args[threadcount].operatorCh = '*';
                  rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
                  if (rc) {
                    printf("Error creating thread %d\n", threadcount);
                    return -1;
                  }
                ////thread num +1
                  threadcount++; 

                expression[ptr-1]=temparr[tempcount++];  //change the expression e.g.(A*B+C->K+C)
                for(int i=ptr;i<count+1;i++){
                    expression[i]=expression[i+2];
                }
                count-=2;
                if(ptr>count)
                    ptr=1;
            }
            
            if((expression[ptr+2]!='*')&&(lockfirst==0)){
                lockfirst=0;
                //Pprogram_Konw(expression[ptr-1],expression[ptr+1],temparr[tempcount],expression[ptr]);
                if(count == 3){
                  args[threadcount].printOut = 1;
                }
                args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
                args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
                args[threadcount].result = &matrices[temparr[tempcount]-'A'];
                args[threadcount].resultID = temparr[tempcount];
                args[threadcount].operatorCh = expression[ptr];
                rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
                if (rc) {
                  printf("Error creating thread %d\n", threadcount);
                  return -1;
                }
                ////thread num +1
                threadcount++; 

                expression[ptr-1]=temparr[tempcount++];
                for(int i=ptr;i<count+1;i++){
                    expression[i]=expression[i+2];
                }
                count-=2;
            }
            else if(lockfirst==0) {  //put first element into waitlist
                waitlist[waitcount++]='+';
                waitloc[waitcount/2]=0;
                waitlist[waitcount++]=expression[0];
                //printf("head is add into waitlist %s",waitlist);
                ptr+=2;
            }
            
        }
        //Not point to the first operator
        //base on operator to do calculation
        if ((lockfirst==0)&&(expression[ptr]=='*')){
          //Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
          args[threadcount].matrix1 = &matrices[expression[ptr-1] -'A'];
          args[threadcount].matrix2 = &matrices[expression[ptr+1] -'A'];
          args[threadcount].result = &matrices[temparr[tempcount]-'A'];
          args[threadcount].resultID = temparr[tempcount];
          args[threadcount].operatorCh = '*';
          rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
          if (rc) {
            printf("Error creating thread %d\n", threadcount);
            return -1;
          }
          ////thread num +1
          threadcount++; 

          expression[ptr-1]=temparr[tempcount++];
          for(int i=ptr;i<count+1;i++){
              expression[i]=expression[i+2];
          }
          count-=2;
        }
        //add next element into waitlist
        if((expression[ptr]!='\0')&&(expression[ptr]!='*')){
            if((expression[ptr+2]!='*')){
                waitlist[waitcount++]=expression[ptr];
                waitloc[waitcount/2]=ptr+1;
                waitlist[waitcount++]=expression[ptr+1];
                //printf("element %c is add into waitlist %s ",expression[ptr+1],waitlist);
            }
        }
        //if waitlist is ready to excute(have 2 elements)
        if (waitcount==4){ //++ or   -- == -(+)
            //printf("\nexcute waitlist\n");
            if(waitlist[0]==waitlist[2]){
                //Padd(waitlist[1],waitlist[3],temparr[tempcount]);
                operatorr='+';
            }
            else{         //+- or -+ == -(-)
                //Psub(waitlist[1],waitlist[3],temparr[tempcount]);
                operatorr='-';
            }
            //Pprogram_Konw(waitlist[1],waitlist[3],temparr[tempcount],operatorr);
            if(count == 3){
              args[threadcount].printOut = 1;
            }
            args[threadcount].matrix1 = &matrices[waitlist[1] -'A'];
            args[threadcount].matrix2 = &matrices[waitlist[3] -'A'];
            args[threadcount].result = &matrices[temparr[tempcount]-'A'];
            args[threadcount].resultID = temparr[tempcount];
            args[threadcount].operatorCh = operatorr;
            rc = pthread_create(&threads[threadcount], NULL, matrixCalculation, (void*)&args[threadcount]);
            if (rc) {
              printf("Error creating thread %d\n", threadcount);
              return -1;
            }
            ////thread num +1
            threadcount++; 

            //printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]); 
            expression[waitloc[0]]=temparr[tempcount++];
            for(int i=waitloc[1]-1;i<count+1;i++){
                    expression[i]=expression[i+2];
            }
            waitcount=0;
            //printf("after waitlist:\n%s\n",expression);
            count-=2;
            if(count==1)
                break;
            continue;
        }
        
        ptr+=2;
        if (ptr>count){
            ptr=1;
            waitcount=0;
            //printf("\n\nTo the end of expression, Waiting all pthread\n\n");
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
        }
        
    }
    //printf("\n\nBefor left, Wait all pthread\n\n");

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
    return (expression[0]-'A');
}

void printMatrix(Matrix* matrix) {
    //wait if matrices is not stand by
    pthread_mutex_lock(&runninglock);
    while (matrix->standby !=1) {
        pthread_cond_wait(&runningcond, &runninglock);
    }
    pthread_mutex_unlock(&runninglock);
    if(TESTMODE){
    struct timeval start, end;
    double cpu_time_used;
    gettimeofday(&start, NULL);                 
    printf("Matrix %c:\n", matrix->name);
    printf("%d",matrix->rows);
    printf("  %d\n", matrix->cols);
    for (int j = 0; j < matrix->rows; j++) {
      for (int k = 0; k < matrix->cols; k++) {
          //printf("%d", matrix->data[j * matrix->cols + k]);
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
    int needle=0;
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
    pthread_t threads[5];
    int skip = 0;
    runningThreads += 1;
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalpha(expression[i])) {
            creatMatrix(matrices, matrixIndex);
            matrixIndex++;
        //parallel do the multplication if possible
        if((skip <= 0)&&(matrixIndex >= 2)&&(expression[(matrixIndex-1)*2-1]=='*')&&(expression[(matrixIndex-1)*2+1]!='/0')){
          if(TESTMODE){
            printf("%c * %c",expression[(matrixIndex-1)*2-2],expression[(matrixIndex-1)*2]);
          }
          if (expression[(matrixIndex-1)*2-1]=='*'){
            MatrixArgs args1;
            args1.matrix1 = &matrices[expression[(matrixIndex-1)*2-2] -'A'];
            args1.matrix2 = &matrices[expression[(matrixIndex-1)*2] -'A'];
            args1.result = &matrices[10 + needle];
            args1.resultID = 'K' + needle;
            args1.subPthreadNum = 1;
            args1.operatorCh = '*';
            pthread_create(&threads[needle], NULL, matrixCalculation, (void*)&args1);
            //Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            //expression[ptr-1]=temparr[tempcount++];
            expression[(matrixIndex-1)*2-2]='K' + needle;
            expression[(matrixIndex-1)*2-1]='0';
            expression[(matrixIndex-1)*2]='0';
            needle++;
            skip = 2;
          }
        }
        skip --;

        }
    //wait
          
    }
    //end readthread  
    pthread_mutex_lock(&runninglock);
    runningThreads -=1;
    pthread_cond_signal(&runningcond);
    pthread_mutex_unlock(&runninglock);

          if (needle != 0){
            //printf("enter first *");
            for(int j=1;j<21;j++){
              if(expression[j]=='0'){
                for(int k = j; k <21;k++){
                  expression[k]=expression[k+1];
                }
                j--;
              }
            }
    }
    if(TESTMODE){
      gettimeofday(&end, NULL);
      cpu_time_used = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
      printf("Execution Time of Reading Maxtrix: %f seconds\n", cpu_time_used);
    }
  //////////////////////////////CALL FUNCTION HERE/////////////////////////////////////////
  //int temp = expressionInterpretation(expression,matrices);
    int temp = Operation_logic(expression,matrices,needle);
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
    pthread_mutex_destroy(&runninglock);
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

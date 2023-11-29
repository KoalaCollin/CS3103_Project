    // Allocate memory for the matrices
    matrices = (Matrix*)malloc(numMatrices * sizeof(Matrix));
    if (matrices == NULL) {
        printf("Memory allocation failed for matrices.\n");
        return 1;
    }
   // Read and assign values to the matrices
    int matrixIndex = 0;
    int do
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isalpha(expression[i])) {
            creatMatrix(matrices, matrixIndex);
            matrixIndex++;
        }
        if(matrixIndex==2){
          if (expression[ptr]=='*'){
            //Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            //expression[ptr-1]=temparr[tempcount++];
            expression[0]='K'
            for(int i=1;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            int needle=1;
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

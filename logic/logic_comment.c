#include <stdio.h>
void Pmulti(char a, char b, char c){ //a*b=c
    //printf(" %d=(%d*%d) ",c-'A',a-'A',b-'A');
    printf(" %c=(%c*%c) ",c,a,b);
}
void Padd(char a, char b, char c){ //a+b=c
    //printf(" %d=(%d+%d) ",c-'A',a-'A',b-'A');
    printf(" %c=(%c+%c) ",c,a,b);
}
void Psub(char a, char b, char c){ //a-b=c
    //printf(" %d=(%d-%d) ",c-'A',a-'A',b-'A');
    printf(" %c=(%c-%c) ",c,a,b);
}

void Pprogram_Konw(char a, char b, char c, char operatorr){
    //printf(" %d=(%d%c%d) ",c-'A',a-'A',b-'A',operatorr);
    printf(" %c=(%c%c%c) ",c,a,operatorr,b);
}


//This function is use to determine call which matrix function 
//and input the correct arguments for those function
//finally it will output the index that store the answer
char Operation_logic(char expression[50],int needle){
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
    while(count>1){
        lockfirst=0;
        printf("\n\n%s ,%dleft  ptr%d",expression,count,ptr);
        if(ptr==1){
            if (expression[ptr]=='*'){
                lockfirst=1;   //do not allow second call in this loop(need to wait the result of excute)
                Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
                expression[ptr-1]=temparr[tempcount++];  //change the expression e.g.(A*B+C->K+C)
                for(int i=ptr;i<count+1;i++){
                    expression[i]=expression[i+2];
                }
                count-=2;
                if(ptr>count)
                    ptr=1;
            }
            else{
                if(expression[ptr+2]!='*'){
                    lockfirst=0;
                    Pprogram_Konw(expression[ptr-1],expression[ptr+1],temparr[tempcount],expression[ptr]);
                    expression[ptr-1]=temparr[tempcount++];
                    for(int i=ptr;i<count+1;i++){
                        expression[i]=expression[i+2];
                    }
                    count-=2;
                }
                else{  //put first element into waitlist
                    waitlist[waitcount++]='+';
                    waitloc[waitcount/2]=0;
                    waitlist[waitcount++]=expression[0];
                    printf("head is add into waitlist %s",waitlist);
                    ptr+=2;
                }
            }
        }
        //Not point to the first operator
        //base on operator to do calculation
        if ((lockfirst==0)&&(expression[ptr]=='*')){
            Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            expression[ptr-1]=temparr[tempcount++];
            for(int i=ptr;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            count-=2;
        }
        //add next element into waitlist
        if((lockfirst==0)&&(expression[ptr]!='\0')&&(expression[ptr]!='*')){
            if((expression[ptr+2]!='*')){
                waitlist[waitcount++]=expression[ptr];
                waitloc[waitcount/2]=ptr+1;
                waitlist[waitcount++]=expression[ptr+1];
                printf("element %c is add into waitlist %s ",expression[ptr+1],waitlist);
            }
        }
        //if waitlist is ready to excute(have 2 elements)
        if (waitcount==4){ //++ or   -- == -(+)
            printf("\nexcute waitlist\n");
            if(waitlist[0]==waitlist[2]){
                //Padd(waitlist[1],waitlist[3],temparr[tempcount]);
                operatorr='+';
            }
            else{         //+- or -+ == -(-)
                //Psub(waitlist[1],waitlist[3],temparr[tempcount]);
                operatorr='-';
            }
            Pprogram_Konw(waitlist[1],waitlist[3],temparr[tempcount],operatorr);
            printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]); 
            expression[waitloc[0]]=temparr[tempcount++];
            for(int i=waitloc[1]-1;i<count+1;i++){
                    expression[i]=expression[i+2];
            }
            waitcount=0;
            printf("after waitlist:\n%s\n",expression);
            count-=2;
            if(count==1)
                break;
            continue;
        }
        
        ptr+=2;
        if (ptr>count){
            ptr=1;
            waitcount=0;
            printf("\n\nTo the end of expression, Waiting all pthread\n\n");
        }
        
    }
    printf("\n\nBefor left, Wait all pthread\n\n");
    return (expression[0]);
}


int main()
{
    //char expression[50]={"A*B+C+D*E-F+G"}; 
    
    //char expression[50]={"A+B"}; //task1
    //char expression[50]={"A*B"}; //task2
    //char expression[50]={"A-B"}; //task3
    //char expression[50]={"A+B-C+D"}; //task4
    //char expression[50]={"A*B*C*D"}; //task5
    char expression[50]={"A+B*C+D"}; //task6
    //char expression[50]={"A+B+C*D*E"}; //task7
    //char expression[50]={"A+B+C*D*E+F-G-H*I*J"}; //task8
    //char expression[50]={"A*B*C*D*E*F*G*H*I*J"}; //temp
    //char expression[50]={"A*B+C*D-E*F+G*H-I*J"}; //task9
    //char expression[50]={"A*B*C*D+E+F+G+H+I+J"}; //task10
    int needle=0;
    char finalll=Operation_logic(expression,needle);
    //printf("\nThe output is %c",Operation_logic(expression));
    printf("\nThe output is %c",finalll);
}

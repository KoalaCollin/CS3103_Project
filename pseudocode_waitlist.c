/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
void Pmulti(char a, char b, char c){ //a*b=c
    printf(" %d=(%d*%d) ",c-'A',a-'A',b-'A');
}
void Padd(char a, char b, char c){ //a+b=c
    printf(" %d=(%d+%d) ",c-'A',a-'A',b-'A');
}
void Psub(char a, char b, char c){ //a-b=c
    printf(" %d=(%d-%d) ",c-'A',a-'A',b-'A');
}

int Operation_logic(char expression[50])
{
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
    
    printf("%s",expression);
    printf("\n%d *%d\n",count,multicount);
    
    ptr=1;
    while(ptr<count){
        Lmulti=0;  //0 dont lock multi
        if (expression[ptr]=='*'){
            Lmulti=1; //locked
            if(firstL==1)
                lockk=1;
            Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            expression[ptr-1]=temparr[tempcount++];
            for(int i=ptr;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            count-=2;
            printf("\n%s",expression);
            if(count==1)
                break;
        }
        
        if (ptr==1&&(lockk==0)){  //read head //
            if (expression[ptr]!='*'){
                waitlist[waitcount++]='+';
                waitloc[waitcount/2]=0;
                waitlist[waitcount++]=expression[0];
                ptr+=2;
                printf("  waitlist %s",waitlist);
            
                //if ((expression[ptr]!='*')&&(expression[ptr+2]!='*')){
                if ((expression[ptr]!='*')){
                    waitlist[waitcount++]=expression[1];
                    waitloc[waitcount/2]=2;
                    waitlist[waitcount++]=expression[2];
                    printf("  waitlist %s",waitlist);
                }
            }   
        }
        
        if ((expression[ptr]=='*')&&(Lmulti==0)){
            if(firstL==1)
                lockk=1;
            Pmulti(expression[ptr-1],expression[ptr+1],temparr[tempcount]);
            expression[ptr-1]=temparr[tempcount++];
            for(int i=ptr;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            count-=2;
            printf("\n%s",expression);
            if(count==1)
                break;
        }
        
        if (waitcount==4){ //++ or   -- == -(+)
            if(waitlist[1]==waitlist[3]){
                waitcount=0;
                printf("\nClear\n");
                continue;
            }
            if(waitlist[0]==waitlist[2]){
                Padd(waitlist[1],waitlist[3],temparr[tempcount]);
            }
            else{         //+- or -+ == -(-)
                Psub(waitlist[1],waitlist[3],temparr[tempcount]);
            }
             printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]);
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
            printf("after waitlist:\n%s",expression);
            count-=2;
            ptr-=2;
            if(count==1)
                break;
        }
        
        if ((expression[ptr]=='+')||(expression[ptr]=='-')){
            if (expression[ptr+2]!='*'){
                waitlist[waitcount++]=expression[ptr];
                waitloc[waitcount/2]=(ptr+1);
                waitlist[waitcount++]=expression[ptr+1];
                //ptr+=2;
                printf(" ptr:%d waitlist %s loc0:%d  1:%d",ptr,waitlist,waitloc[0],waitloc[1]);
            }
        }
        if (waitcount==4){ //++ or   -- == -(+)
            if(waitlist[1]==waitlist[3]){
                waitcount=0;
                printf("\nClear\n");
                continue;
            }
            if(waitlist[0]==waitlist[2]){
                Padd(waitlist[1],waitlist[3],temparr[tempcount]);
            }
            else{         //+- or -+ == -(-)
                Psub(waitlist[1],waitlist[3],temparr[tempcount]);
            }
             printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]);
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
            printf("after waitlist:\n%s\n",expression);
            count-=2;
            ptr-=2;
            if(count==1)
                break;
        }
        
        ptr+=2;
        if(count==1)
            break;
        if(firstL==1){
            firstL=0;
            lockk=0;
        }
        if(ptr>=count){ //if(current ==NULL)
            ptr=1;      //current= head->right;
                        //joinPthread here //
            firstL=1;
            //lockk=0;
            waitcount=0;
            //continue;
            printf("\n\nWaiting all pthread\n\n");
        }
        
        printf("\n\n%s ,%d  !%d",expression,count,ptr);

    }
    return (expression[0]);
}



int main(){
    //char expression[50]={"A*B+C+D*E-F+G"}; //already have
    
    //char expression[50]={"A+B"}; //task1
    //char expression[50]={"A*B"}; //task2
    //char expression[50]={"A-B"}; //task3
    //char expression[50]={"A+B-C+D"}; //task4
    //char expression[50]={"A*B*C*D"}; //task5
    //char expression[50]={"A+B*C+D"}; //task6
    //char expression[50]={"A+B+C*D*E"}; //task7
    //char expression[50]={"A+B+C*D*E+F-G-H*I*J"}; //task8
    //char expression[50]={"A*B*C*D*E*F*G*H*I*J"}; //temp
    //char expression[50]={"A*B+C*D-E*F+G*H-I*J"}; //task9
    char expression[50]={"A*B*C*D+E+F+G+H+I+J"}; //task10
    
    char finalll=Operation_logic(expression);
    //printf("\nThe output is %c",Operation_logic(expression));
    printf("\nThe output is %c",finalll);

}



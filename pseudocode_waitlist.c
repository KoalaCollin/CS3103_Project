/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, OCaml, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
void Pmulti(char a, char b, char c){ //a*b=c
    printf(" %c=(%c*%c) ",c,a,b);
}
void Padd(char a, char b, char c){ //a+b=c
    printf(" %c=(%c+%c) ",c,a,b);
}
void Psub(char a, char b, char c){ //a-b=c
    printf(" %c=(%c-%c) ",c,a,b);
}

int main()
{
    int count=0;
    int ptr;    // this is current in list node(should be)
    int multicount=0;
    int firstL=1;
    //printf("Hello World");
    char expression[50]={"A*B+C+D*E-F+G"};
    //char expression[50]={"A*B*C*D*e+f"};
    char temp[50]={"KLMNOPQRST"};
    char waitlist[50];
    int waitloc[10];
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
        if (expression[ptr]=='*'){
            Pmulti(expression[ptr-1],expression[ptr+1],temp[tempcount]);
            expression[ptr-1]=temp[tempcount++];
            for(int i=ptr;i<count+1;i++){
                expression[i]=expression[i+2];
            }
            count-=2;
            if (expression[ptr+2]=='*')
                ptr+=2;
            //ptr+=2;
            printf("\n%s",expression);
        }
        
        if (ptr==1&&(firstL==0)){  //read head
            if (expression[ptr]!='*'){
                waitlist[waitcount++]='+';
                waitloc[waitcount/2]=0;
                waitlist[waitcount++]=expression[0];
                ptr+=2;
                printf("  waitlist %s",waitlist);
            }
            if (expression[ptr]=='\0'){
                waitlist[waitcount++]=expression[ptr-2];
                waitloc[waitcount/2]=ptr-1;
                waitlist[waitcount++]=expression[ptr-1];
                ptr+=2;
                printf("  waitlist %s",waitlist);
            }
        }
        else if ((expression[ptr]=='+')||(expression[ptr]=='-')){
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
                waitcount==0;
                continue;
            }
            if(waitlist[0]==waitlist[2]){
                Padd(waitlist[1],waitlist[3],temp[tempcount]);
            }
            else{         //+- or -+ == -(-)
                Psub(waitlist[1],waitlist[3],temp[tempcount]);
            }
             printf("  waitloc 0:%d, 1:%d",waitloc[0],waitloc[1]);
            if(waitloc[0]<waitloc[1]){
                expression[waitloc[0]]=temp[tempcount++];
                for(int i=waitloc[1]-1;i<count+1;i++){
                    expression[i]=expression[i+2];
                }
            }
            else{
                expression[waitloc[1]]=temp[tempcount++];
                for(int i=waitloc[0]-1;i<count+1;i++){
                    expression[i]=expression[i+2];
                }
            }
            waitcount=0;
            printf("after waitlist:\n%s",expression);
            count-=2;
            ptr-=2;
        }
        ptr+=2;
        
        
        if(ptr>=count){ //if(current ==NULL)
            ptr=1;      //current= head->right;
                        //joinPthread here //
        }
        
        printf("\n\n%s ,%d  !%d",expression,count,ptr);
        if(firstL==1)
            firstL=0;
    }
    return 0;
}

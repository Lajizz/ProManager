#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(void){
    while(1){
        char a = getchar();
        if(a == 'r'){
            char buf[100];
            ssize_t ret = read(0,buf,30);
            printf("read: %s\n",buf);
        }else{
            printf("No such instruction\n");
        }
        getchar();
        
    }
}
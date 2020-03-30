#include <stdio.h>

int main(int argc, char* argv[]){
    int x = 0;
    while(x < 1000000){
        printf("%d", x);
        x++;
    }

    return 0;

}
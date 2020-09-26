#include "apue.h"
#include "apue_err.h"

int main(void)
{
    int c;

    printf("Hello world from process ID %d\n", getpid());

    exit(0); 
}

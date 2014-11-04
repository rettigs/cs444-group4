#include<stdio.h>
#define asm __asm__ __volatile__

int main (int argc, char*argv[])
{

int result = 0;

asm("rdrand %0" : "=r" (result));
printf("Random Number: %d\n", result);
return 0;

}


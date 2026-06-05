#include <stdio.h>
#include <stdlib.h>

int cubo(void);
int I;

void main(void)
{
    int CUB;
    for (I = 1; I <= 10; I++)
    {
        CUB = cubo();
        printf("\nE; cubo de %d es: %d", I, CUB);
    }
}

int cubo(void)
{
    return (I*I*I);
}

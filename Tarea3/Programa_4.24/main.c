#include <stdio.h>

int F1(void);
int F2(void);
int F3(void);
int F4(void);
int k = 5;

void main(void)
{
    int I;
    for (I = 1; I <= 4; I++)
    {
        printf("\n\nEl resultado de la funcion F1 es: %d", F1());
        printf("\nEl resultado de la funcion F2 es: %d", F2());
        printf("\nEl resultado de la funcion F3 es: %d", F3());
        printf("\nEl resultado de la funcion F4 es: %d", F4());
    }
}
int F1(void)
{
    k =+ k;
    return (k);
}
int F2(void)
{
    int k = 3;
    k++;
    return (k);
}
int F3(void)
{
    static int k = 6;
    k += 3;
    return (k);
}
int F4(void)
{
    int k_local = 4;
    k = k + k_local;
    return (k);
}

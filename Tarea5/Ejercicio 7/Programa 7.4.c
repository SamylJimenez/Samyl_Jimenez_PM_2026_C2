#include <stdio.h>

void main(void)
{
    char *cad0;
    cad0 = "Argentina";
    puts(cad0);

    cad0 = "Brasil";
    puts(cad0);

    char cad1[50];
    printf("\Ingrese un texto para cad1: ");
    gets(cad1);
    puts(cad1);

    char cad2[20] = "Mexico";
    printf("\nCadena cad2 inicial: ");
    puts(cad2);

    printf("Ingrese un nuevo texto para cad2: ");
    gets(cad2);
    printf("Cadena cad2 final: ");
    puts(cad2);

}

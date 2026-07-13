#include <stdio.h>

void main(void)
{
    char p1;
    FILE *ar;
    ar = fopen("Programa 9.1.txt", "w");
    if (ar != NULL)
    {
        printf("\nEscriba: ");
        while ((p1=getchar()) != '\n')
            fputc(p1, ar);

        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo");
}

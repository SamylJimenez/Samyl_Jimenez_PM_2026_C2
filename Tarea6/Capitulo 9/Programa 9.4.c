#include <stdio.h>

void main(void)
{
    char cad[50];
    FILE *ap;
    if ((ap = fopen ("Programa 9.3.txt", "r")) != NULL)

    {
        while (fgets(cad, sizeof(cad), ap) != NULL)
        {
            printf("%s", cad);
        }
        fclose(ap);
    }
    else
    {
        printf("No se puede abrir el archivo\n");
    }
}

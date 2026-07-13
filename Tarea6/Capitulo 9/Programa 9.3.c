#include <stdio.h>

void main(void)
{
    char cad[50];
    int res;
    FILE *ar;
    if ((ar = fopen("Programa 9.3.txt", "w")) != NULL)
    {
        printf("\nDesea ingresar una cadena de caracteres? Si-1 No-0: ");
        scanf("%d", &res);
        while (getchar() != '\n');
        while (res)
        {
            printf("Ingrese la cadena: ");
            fgets(cad, sizeof(cad), stdin);
            fputs(cad, ar);

            printf("\nDesea ingresar otra cadena de caracteres? Si-1 No-0:");
            scanf("%d", &res);


        }
        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo");
}

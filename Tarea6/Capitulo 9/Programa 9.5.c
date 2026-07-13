#include <stdio.h>

void main(void)
{
    int i, j, n, mat;
    float cal;
    FILE *ar;
    printf("\nIngrese el numero de alumnos: ");
    scanf("%d", &n);

    if((ar = fopen("Programa 9.5.txt", "w")) != NULL)
    {
        fprintf(ar,"%d", n);

        for (i=0; i<n; i++)
        {
            printf("\nIngrese la matricula del alumno %d: ", i+1);
            scanf("%d", &mat);
            fprintf(ar, "\n%d ", mat);

            for(j=0; j<5; j++)
            {
                printf("\nCalificacion %d: ", j+1);
                scanf("%f", &cal);
                fprintf(ar, "%.2f ", cal);

            }
        }
        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo");
}

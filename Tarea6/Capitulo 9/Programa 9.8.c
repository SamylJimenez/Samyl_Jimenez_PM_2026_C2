#include <stdio.h>

typedef struct
{
    int matricula;
    char nombre[20];
    int carrera;
    float promedio;
} alumno;

void escribe(FILE *);

void main(void)
{
    FILE *ar;
    if ((ar = fopen ("Programa 9.8.dat", "w")) != NULL)
        escribe(ar);
    else
        printf("\nEl archivo no se puede abrir");
    fclose(ar);
}

void escribe(FILE *ap)
{
    alumno alu;
    int i = 0, r;
    printf("\nDesea ingresar informacion sobre alumnos? (Si-1 No-0): ");
    scanf("%d", &r);
    getchar();
    while(r)
    {
        i++;
        printf("Matricula del alumno %d: ", i);
        scanf("%d", &alu.matricula);
        getchar();
        printf("Nombre del alumno %d: ", i);
        gets(alu.nombre);
        printf("Carrera del alumno %d: ", i);
        scanf("%d", &alu.carrera);
        getchar();
        printf("Promedio del alumno %d: ", i);
        scanf("%f", &alu.promedio);
        getchar();

        fwrite(&alu, sizeof(alumno), 1, ap);

        printf("\nDesea ingresar informacion sobre mas alumnos? (Si-1 No-0): ");
        scanf("%d", &r);
        getchar();
    }
}

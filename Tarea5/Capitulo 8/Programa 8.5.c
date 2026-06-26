#include <stdio.h>
#include <string.h>

union datos
{
    char celular[15];
    char correo[20];
};

typedef struct
{
    int matricula;
    char nombre[20];
    char carrera[20];
    float promedio;
    union datos personales;
} alumno;

void Lectura(alumno *a);

void main(void)
{
    alumno a1 = {120, "Maria", "Contabilidad", 8.9, "5-158-40-50"}, a2, a3;
    printf("\nAlumno 2\n");
    printf("Ingrese la matricula: ");
    scanf("%d", &a2.matricula);
    getchar();
    printf("Ingrese el nombre: ");
    gets(a2.nombre);
    printf("Ingrese la carrera: ");
    gets(a2.carrera);
    printf("Ingrese el promedio: ");
    scanf("%f", &a2.promedio);
    getchar();
    printf("Ingrese el correo electronico: ");
    gets(a2.personales.correo);

    printf("\nAlumno 3");
    Lectura(&a3);

    printf("\nDatos del alumno 1\n");

    printf("%d\n", a1.matricula);
    puts(a1.nombre);
    puts(a1.carrera);
    printf("%.2f\n", a1.promedio);
    puts(a1.personales.celular);
    puts(a1.personales.correo);

    strcpy(a1.personales.correo, "hgimenez@hotmail.com");

    puts(a1.personales.celular);
    puts(a1.personales.correo);

    printf("\nDatos del alumno 2\n");
    printf("%d\n", a2.matricula);
    puts(a2.nombre);
    puts(a2.carrera);
    printf("%.2f\n", a2.promedio);
    puts(a2.personales.celular);
    puts(a2.personales.correo);

    printf("Ingrese el telefono celular del alumno 2: ");
    gets(a2.personales.celular);

    puts(a2.personales.celular);
    puts(a2.personales.correo);

    printf("\nDatos del alumno 3\n");
    printf("%d\n", a3.matricula);
    puts(a3.nombre);
    puts(a3.carrera);
    printf("%.2f\n", a3.promedio);
    puts(a3.personales.celular);
    puts(a3.personales.correo);
}
    void Lectura(alumno *a)

    {
        printf("\nIngreso la matricula: ");
        scanf("%d", &(*a).matricula);
        getchar();
        printf("Ingrese el nombre: ");
        gets(a->nombre);
        printf("Ingrese la carrera: ");
        gets((*a).carrera);
        printf("Ingrese el promedio: ");
        scanf("%f", &a->promedio);
        getchar();
        printf("Ingrese el telefono celular: ");
        gets(a->personales.celular);
    }

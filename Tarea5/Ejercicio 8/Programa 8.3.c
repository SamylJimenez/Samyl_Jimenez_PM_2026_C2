#include <stdio.h>
#include <string.h>

typedef struct
{
    char calle[20];
    int numero;
    int cp;
    char localidad[20];
}   domicilio;

struct empleado

{
    char nombre[20];
    char departamento[20];
    float sueldo;
    domicilio direccion;

};
void Lectura(struct empleado *a);

void main(void)

{
    struct empleado e0= {"Arturo", "Compras", 15500.75, "San Jeronimo", 120, 3490, "Toluca"};

    struct empleado e1, e2, e3, e4;

    printf("\nIngrese el nombre del empleado 1: ");
    scanf("%s", e1.nombre);
    getchar();

    printf("Ingrese el departamento de la empresa: ");
    gets(e1.departamento);

    printf("Ingrese el sueldo del empleado 1: ");
    scanf("%f", &e1.sueldo);
    getchar();

    printf("Ingrese la direccion del empleado 1: ");
    printf("\n\tCalle: ");
    gets(e1.direccion.calle);

    printf("\tNumero: ");
    scanf("%d", &e1.direccion.numero);
    getchar();

    printf("\tCodigo Postal: ");
    scanf("%d", &e1.direccion.cp);
    getchar();

    printf("\tLocalidad: ");
    gets(e1.direccion.localidad);

    printf("\nIngrese el nombre del empleado 3: ");
    scanf("%s", e3.nombre);
    getchar();

    printf("Ingrese el departamento de la empresa: ");
    gets(e3.departamento);

    printf("Ingrese el sueldo del empleado 3: ");
    scanf("%f", &e3.sueldo);
    getchar();

    printf("Ingrese la direccion del empleado 3: ");
    printf("\n\tCalle: ");
    gets(e3.direccion.calle);

    printf("\tNumero: ");
    scanf("%d", &e3.direccion.numero);
    getchar();

    printf("\tCodigo Postal: ");
    scanf("%d", &e3.direccion.cp);
    getchar();

    printf("\tLocalidad: ");
    gets(e3.direccion.localidad);

    Lectura(&e2);
    Lectura(&e4);

    printf("\nDatos del empleado 1\n");
    printf("%s\t%s\t%.2f\t%s\t%d\t%d\t%s", e1.nombre, e1.departamento, e1.sueldo, e1.direccion.calle, e1.direccion.numero, e1.direccion.cp, e1.direccion.localidad);

    printf("\nDatos del empleado 2\n");
    printf("%s\t%s\t%.2f\t%s\t%d\t%d\t%s", e2.nombre, e2.departamento, e2.sueldo, e2.direccion.calle, e2.direccion.numero, e2.direccion.cp, e2.direccion.localidad);

    printf("\nDatos del empleado 3\n");
    printf("%s\t%s\t%.2f\t%s\t%d\t%d\t%s", e3.nombre, e3.departamento, e3.sueldo, e3.direccion.calle, e3.direccion.numero, e3.direccion.cp, e3.direccion.localidad);

    printf("\nDatos del empleado 4\n");
    printf("%s\t%s\t%.2f\t%s\t%d\t%d\t%s", e4.nombre, e4.departamento, e4.sueldo, e4.direccion.calle, e4.direccion.numero, e4.direccion.cp, e4.direccion.localidad);

}

void Lectura(struct empleado *a)
{

    printf("\nIngrese el nombre del empleado: ");
    gets(a->nombre);

    printf("Ingrese el departamento de la empresa: ");
    gets(a->departamento);

    printf("Ingrese el sueldo del empleado: ");
    scanf("%f", &a->sueldo);
    getchar();

    printf("Ingrese la direccion del empleado");
    printf("\n\tCalle: ");
    gets(a->direccion.calle);

    printf("\tNumero: ");
    scanf("%d", &a->direccion.numero);
    getchar();

    printf("\tCodigo Postal: ");
    scanf("%d", &a->direccion.cp);
    getchar();

    printf("\tLocalidad: ");
    gets(a->direccion.localidad);

}

#include <stdio.h>
#include <string.h>

typedef struct
{
    char zona[20];
    char calle[20];
    char colo[20];
} ubicacion;

typedef struct
{
    char clave[5];
    float scu;
    float ste;
    char car[50];
    ubicacion ubi;

    float precio;
    char dispo;
} propiedades;

void Lectura(propiedades *, int);
void F1(propiedades *, int);
void F2(propiedades *, int);

void main(void)
{
    propiedades PROPIE[100];
    int TAM;
    do
    {
        printf("Ingrese el numero de propiedades: ");
        scanf("%d", &TAM);
    }
    while (TAM > 100 || TAM < 1);
    Lectura(PROPIE, TAM);
    F1(PROPIE, TAM);
    F2(PROPIE, TAM);
}

void Lectura(propiedades A[], int T)
{
    int I;
    for (I=0; I<T; I++)
    {
        printf("\n\tIngrese datos de la propiedad %d", I + 1);
        printf("\nClave: ");
        getchar();
        gets(A[I].clave);
        printf("Superficie cubierta: ");
        scanf("%f", &A[I].scu);
        printf("Superficie terreno: ");
        scanf("%f", &A[I].ste);
        getchar();
        printf("Caracteristicas: ");
        gets(A[I].car);
        printf("\tZona: ");
        gets(A[I].ubi.zona);
        printf("\tCalle: ");
        gets(A[I].ubi.calle);
        printf("\tColonia: ");
        gets(A[I].ubi.colo);
        printf("Precio: ");
        scanf("%f", &A[I].precio);
        getchar();
        printf("Disponibilidad (Venta-V Renta-R): ");
        scanf("%c", &A[I].dispo);
        getchar();
    }
}
void F1(propiedades A[], int T)
{
    int I;
    printf("\n\t\tListado de Propiedades para Venta en Miraflores");
    for (I=0; I<T; I++)
        if ((A[I].dispo == 'V' || A[I].dispo == 'v') && (strcmp (A[I].ubi.zona, "Miraflores") == 0))
            if ((A[I].precio >= 450000) && (A[I].precio <= 650000))
    {
        printf("\nClave de la propiedad: ");
        puts(A[I].clave);
        printf("\nSuperficie cubierta: %f", A[I].scu);
        printf("\nSuperficie terreno: %f", A[I].ste);
        printf("\nCaracteristicas: ");
        puts(A[I].car);
        printf("Calle: ");
        puts(A[I].ubi.calle);
        printf("Colonia: ");
        puts(A[I].ubi.colo);
        printf("Precio: %.2f\n", A[I].precio);
    }
}

void F2(propiedades A[], int T)
{
    int I;
    float li, ls;
    char zon[20];
    printf("\n\t\tListado de Propiedades para Renta");
    printf("\nIngrese zona geografica: ");
    getchar();
    gets(zon);
    printf("Ingrese el limite inferior del precio: ");
    scanf("%f", &li);
    printf("Ingrese el limite superior del precio: ");
    scanf("%f", &ls);
    for (I=0; I<T; I++)
        if ((A[I].dispo == 'R' || A[I].dispo == 'r') && (strcmp (A[I].ubi.zona, zon) == 0))
            if ((A[I].precio >= li) && (A[I].precio <= ls))
    {
        printf("\nClave de la propiedad: ");
        puts(A[I].clave);
        printf("\nSuperficie cubierta: %.2f", A[I].scu);
        printf("\nSuperficie terreno: %.2f", A[I].ste);
        printf("\nCaracteristicas: ");
        puts(A[I].car);
        printf("Calle: ");
        puts(A[I].ubi.calle);
        printf("Colonia: ");
        puts(A[I].ubi.colo);
        printf("Precio: %.2f", A[I].precio);
    }
}

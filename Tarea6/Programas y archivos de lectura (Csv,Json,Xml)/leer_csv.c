/* ============================================================
   leer_csv.c
   Lee un archivo CSV con alumnos (con línea de encabezado),
   cuenta cuántas líneas de datos hay, reserva un array dinámico
   de ese tamaño y carga los datos para imprimirlos.

   Formato esperado (alumnos.csv):
   Nombre,Apellido,Promedio,Materia
   Juan,Perez,8.50,Matematica
   ...
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define MAX_LINEA 512

typedef struct {
    char nombre[MAX_STR];
    char apellido[MAX_STR];
    float promedio;
    char materia[MAX_STR];
} Alumno;

/* Cuenta cuántas líneas de datos (sin contar el encabezado) tiene el archivo */
int contar_alumnos(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        printf("No se pudo abrir el archivo %s\n", ruta);
        return -1;
    }

    char linea[MAX_LINEA];
    int contador = 0;

    /* Descartar la línea de encabezado */
    if (fgets(linea, MAX_LINEA, f) == NULL) {
        fclose(f);
        return 0;
    }

    while (fgets(linea, MAX_LINEA, f) != NULL) {
        /* Ignorar líneas vacías al final del archivo */
        int es_vacia = 1;
        for (int i = 0; linea[i] != '\0'; i++) {
            if (linea[i] != '\n' && linea[i] != '\r' && linea[i] != ' ') {
                es_vacia = 0;
                break;
            }
        }
        if (!es_vacia) contador++;
    }

    fclose(f);
    return contador;
}

/* Carga los n alumnos del archivo CSV en el array ya reservado */
void cargar_alumnos(const char *ruta, Alumno *lista, int n) {
    FILE *f = fopen(ruta, "r");
    if (!f) return;

    char linea[MAX_LINEA];
    fgets(linea, MAX_LINEA, f); /* saltear encabezado */

    int i = 0;
    while (i < n && fgets(linea, MAX_LINEA, f) != NULL) {
        /* Quitar salto de línea */
        linea[strcspn(linea, "\r\n")] = '\0';
        if (strlen(linea) == 0) continue;

        char *token;
        token = strtok(linea, ",");
        strncpy(lista[i].nombre, token ? token : "", MAX_STR - 1);

        token = strtok(NULL, ",");
        strncpy(lista[i].apellido, token ? token : "", MAX_STR - 1);

        token = strtok(NULL, ",");
        lista[i].promedio = token ? (float) atof(token) : 0.0f;

        token = strtok(NULL, ",");
        strncpy(lista[i].materia, token ? token : "", MAX_STR - 1);

        i++;
    }

    fclose(f);
}

void imprimir_alumnos(Alumno *lista, int n) {
    printf("\nSe encontraron %d alumnos en el archivo CSV:\n", n);
    printf("--------------------------------------------------------------\n");
    printf("%-15s %-15s %-10s %-15s\n", "Nombre", "Apellido", "Promedio", "Materia");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("%-15s %-15s %-10.2f %-15s\n",
               lista[i].nombre, lista[i].apellido,
               lista[i].promedio, lista[i].materia);
    }
    printf("--------------------------------------------------------------\n");
}

int main(int argc, char *argv[]) {
    const char *ruta = (argc > 1) ? argv[1] : "alumnos.csv";

    /* 1) Primero se cuenta la cantidad de elementos */
    int n = contar_alumnos(ruta);
    if (n < 0) return 1;
    printf("Cantidad de elementos detectados: %d\n", n);

    if (n == 0) {
        printf("No se encontraron alumnos en el archivo.\n");
        return 0;
    }

    /* 2) Recién ahora se reserva la memoria dinámica del tamaño justo */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (!alumnos) {
        printf("Error al reservar memoria para los alumnos\n");
        return 1;
    }

    /* 3) Se cargan los datos y se imprimen */
    cargar_alumnos(ruta, alumnos, n);
    imprimir_alumnos(alumnos, n);

    free(alumnos);
    return 0;
}

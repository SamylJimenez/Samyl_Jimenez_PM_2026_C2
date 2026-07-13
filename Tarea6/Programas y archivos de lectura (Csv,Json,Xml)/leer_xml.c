/* ============================================================
   leer_xml.c
   Lee un archivo XML con alumnos, cuenta cuántos elementos
   <Alumno> hay, reserva un array dinámico de ese tamaño y
   carga los datos en memoria para luego imprimirlos.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100

typedef struct {
    char nombre[MAX_STR];
    char apellido[MAX_STR];
    float promedio;
    char materia[MAX_STR];
} Alumno;

/* Lee todo el contenido de un archivo de texto en un buffer */
char *leer_archivo_completo(const char *ruta, long *tam_out) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        printf("No se pudo abrir el archivo %s\n", ruta);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long tam = ftell(f);
    rewind(f);

    char *buffer = (char *) malloc((tam + 1) * sizeof(char));
    if (!buffer) {
        printf("Error al reservar memoria para el buffer\n");
        fclose(f);
        return NULL;
    }

    size_t leidos = fread(buffer, 1, tam, f);
    buffer[leidos] = '\0';

    fclose(f);
    if (tam_out) *tam_out = leidos;
    return buffer;
}

/* Cuenta cuántas veces aparece la etiqueta <Alumno> (apertura exacta) */
int contar_alumnos(const char *buffer) {
    int contador = 0;
    const char *p = buffer;
    while ((p = strstr(p, "<Alumno>")) != NULL) {
        contador++;
        p += strlen("<Alumno>");
    }
    return contador;
}

/* Extrae el contenido de una etiqueta simple dentro de un bloque de texto */
void extraer_tag(const char *bloque, const char *tag, char *destino) {
    char apertura[40], cierre[40];
    snprintf(apertura, sizeof(apertura), "<%s>", tag);
    snprintf(cierre, sizeof(cierre), "</%s>", tag);

    const char *inicio = strstr(bloque, apertura);
    if (!inicio) { destino[0] = '\0'; return; }
    inicio += strlen(apertura);

    const char *fin = strstr(inicio, cierre);
    if (!fin) { destino[0] = '\0'; return; }

    long len = fin - inicio;
    if (len >= MAX_STR) len = MAX_STR - 1;
    strncpy(destino, inicio, len);
    destino[len] = '\0';
}

/* Carga los n alumnos del buffer en el array ya reservado */
void cargar_alumnos(const char *buffer, Alumno *lista, int n) {
    const char *p = buffer;
    char promedio_str[MAX_STR];

    for (int i = 0; i < n; i++) {
        p = strstr(p, "<Alumno>");
        const char *fin_bloque = strstr(p, "</Alumno>");

        long len_bloque = fin_bloque - p;
        char *bloque = (char *) malloc((len_bloque + 1) * sizeof(char));
        strncpy(bloque, p, len_bloque);
        bloque[len_bloque] = '\0';

        extraer_tag(bloque, "Nombre", lista[i].nombre);
        extraer_tag(bloque, "Apellido", lista[i].apellido);
        extraer_tag(bloque, "Promedio", promedio_str);
        lista[i].promedio = atof(promedio_str);
        extraer_tag(bloque, "Materia", lista[i].materia);

        free(bloque);
        p = fin_bloque + strlen("</Alumno>");
    }
}

void imprimir_alumnos(Alumno *lista, int n) {
    printf("\nSe encontraron %d alumnos en el archivo XML:\n", n);
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
    const char *ruta = (argc > 1) ? argv[1] : "alumnos.xml";

    long tam;
    char *buffer = leer_archivo_completo(ruta, &tam);
    if (!buffer) return 1;

    /* 1) Primero se cuenta la cantidad de elementos */
    int n = contar_alumnos(buffer);
    printf("Cantidad de elementos detectados: %d\n", n);

    if (n == 0) {
        printf("No se encontraron alumnos en el archivo.\n");
        free(buffer);
        return 0;
    }

    /* 2) Recién ahora se reserva la memoria dinámica del tamaño justo */
    Alumno *alumnos = (Alumno *) malloc(n * sizeof(Alumno));
    if (!alumnos) {
        printf("Error al reservar memoria para los alumnos\n");
        free(buffer);
        return 1;
    }

    /* 3) Se cargan los datos y se imprimen */
    cargar_alumnos(buffer, alumnos, n);
    imprimir_alumnos(alumnos, n);

    free(alumnos);
    free(buffer);
    return 0;
}

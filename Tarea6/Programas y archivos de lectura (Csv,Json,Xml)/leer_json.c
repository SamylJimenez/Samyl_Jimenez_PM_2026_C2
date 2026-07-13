/* ============================================================
   leer_json.c
   Lee un archivo JSON con un array de alumnos, cuenta cuántos
   objetos hay (buscando la clave "Nombre"), reserva un array
   dinámico de ese tamaño y carga los datos para imprimirlos.

   Formato esperado (alumnos.json):
   [
     { "Nombre": "...", "Apellido": "...", "Promedio": 8.5, "Materia": "..." },
     ...
   ]
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

/* Cuenta cuántos objetos alumno hay, buscando la clave "Nombre" */
int contar_alumnos(const char *buffer) {
    int contador = 0;
    const char *p = buffer;
    while ((p = strstr(p, "\"Nombre\"")) != NULL) {
        contador++;
        p += strlen("\"Nombre\"");
    }
    return contador;
}

/* Extrae el valor STRING asociado a una clave "clave": "valor" */
void extraer_string(const char *bloque, const char *clave, char *destino) {
    char patron[40];
    snprintf(patron, sizeof(patron), "\"%s\"", clave);

    const char *pos = strstr(bloque, patron);
    if (!pos) { destino[0] = '\0'; return; }
    pos += strlen(patron);

    /* Buscar la primera comilla que abre el valor (después de los ':') */
    const char *comilla1 = strchr(pos, '"');
    if (!comilla1) { destino[0] = '\0'; return; }
    comilla1++;

    const char *comilla2 = strchr(comilla1, '"');
    if (!comilla2) { destino[0] = '\0'; return; }

    long len = comilla2 - comilla1;
    if (len >= MAX_STR) len = MAX_STR - 1;
    strncpy(destino, comilla1, len);
    destino[len] = '\0';
}

/* Extrae el valor NUMÉRICO asociado a una clave "clave": numero */
float extraer_numero(const char *bloque, const char *clave) {
    char patron[40];
    snprintf(patron, sizeof(patron), "\"%s\"", clave);

    const char *pos = strstr(bloque, patron);
    if (!pos) return 0.0f;
    pos += strlen(patron);

    const char *dos_puntos = strchr(pos, ':');
    if (!dos_puntos) return 0.0f;

    return (float) atof(dos_puntos + 1);
}

/* Carga los n alumnos del buffer en el array ya reservado */
void cargar_alumnos(const char *buffer, Alumno *lista, int n) {
    const char *p = buffer;

    for (int i = 0; i < n; i++) {
        const char *inicio_obj = strchr(p, '{');
        const char *fin_obj = strchr(inicio_obj, '}');

        long len_bloque = fin_obj - inicio_obj;
        char *bloque = (char *) malloc((len_bloque + 1) * sizeof(char));
        strncpy(bloque, inicio_obj, len_bloque);
        bloque[len_bloque] = '\0';

        extraer_string(bloque, "Nombre", lista[i].nombre);
        extraer_string(bloque, "Apellido", lista[i].apellido);
        lista[i].promedio = extraer_numero(bloque, "Promedio");
        extraer_string(bloque, "Materia", lista[i].materia);

        free(bloque);
        p = fin_obj + 1;
    }
}

void imprimir_alumnos(Alumno *lista, int n) {
    printf("\nSe encontraron %d alumnos en el archivo JSON:\n", n);
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
    const char *ruta = (argc > 1) ? argv[1] : "alumnos.json";

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

/*
 * Programa 2
 * ----------
 * Identifica archivos según su contenido real (usando los "numeros magicos"
 * o firma binaria de cada formato) y verifica si coincide con la extension
 * que tiene el nombre del archivo.
 *
 * Ejemplo: si un archivo se llama "foto.png" pero en realidad su contenido
 * es un JPG, el programa lo marcara como NO VALIDO e indicara el tipo real.
 *
 * Compilar:
 *   gcc identificar_archivos.c -o identificar_archivos
 *
 * Ejecutar:
 *   ./identificar_archivos [carpeta]     (por defecto usa la carpeta actual ".")
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_RUTA 1024
#define MAX_FIRMA 8

/* Tabla de firmas (numeros magicos) mas comunes.
 * Cada entrada tiene: bytes de la firma, longitud, y el tipo detectado. */
typedef struct {
    unsigned char firma[MAX_FIRMA];
    int longitud;
    const char *tipo;
} FirmaArchivo;

static const FirmaArchivo FIRMAS[] = {
    { {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A}, 8, "png" },
    { {0xFF, 0xD8, 0xFF},                            3, "jpg" },
    { {'B', 'M'},                                    2, "bmp" },
    { {'G', 'I', 'F', '8', '7', 'a'},                6, "gif" },
    { {'G', 'I', 'F', '8', '9', 'a'},                6, "gif" },
    { {'%', 'P', 'D', 'F', '-'},                     5, "pdf" },
    { {'P', 'K', 0x03, 0x04},                        4, "zip/docx/xlsx/pptx" },
    { {0x1F, 0x8B, 0x08},                             3, "gz" },
};
static const int NUM_FIRMAS = sizeof(FIRMAS) / sizeof(FIRMAS[0]);

/* Devuelve el tipo real detectado según la firma, o NULL si no coincide con ninguna */
const char *identificar_tipo_real(const char *ruta) {
    unsigned char encabezado[16] = {0};
    FILE *f = fopen(ruta, "rb");
    if (!f) return NULL;

    size_t leidos = fread(encabezado, 1, sizeof(encabezado), f);
    fclose(f);
    (void)leidos;

    for (int i = 0; i < NUM_FIRMAS; i++) {
        if (memcmp(encabezado, FIRMAS[i].firma, FIRMAS[i].longitud) == 0) {
            return FIRMAS[i].tipo;
        }
    }
    return NULL;
}

/* Extrae la extension de un nombre de archivo, en minusculas, sin el punto */
void obtener_extension(const char *nombre, char *destino, size_t tam_destino) {
    const char *punto = strrchr(nombre, '.');
    destino[0] = '\0';
    if (!punto || punto == nombre) return;

    size_t i = 0;
    punto++; /* saltar el punto */
    while (*punto && i < tam_destino - 1) {
        destino[i++] = (char)tolower((unsigned char)*punto);
        punto++;
    }
    destino[i] = '\0';
}

/* Verifica si la extension declarada es equivalente/compatible con el tipo real */
int extension_coincide(const char *extension, const char *tipo_real) {
    if (strcmp(extension, tipo_real) == 0) return 1;

    /* jpg y jpeg se consideran equivalentes */
    if ((strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0) &&
        strcmp(tipo_real, "jpg") == 0) {
        return 1;
    }

    /* tipo_real puede venir con varias posibilidades separadas por "/" (ej: zip/docx/xlsx/pptx) */
    char copia[64];
    strncpy(copia, tipo_real, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    char *token = strtok(copia, "/");
    while (token) {
        if (strcmp(extension, token) == 0) return 1;
        token = strtok(NULL, "/");
    }
    return 0;
}

int comparar_nombres(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[]) {
    const char *carpeta = (argc > 1) ? argv[1] : ".";

    DIR *dir = opendir(carpeta);
    if (!dir) {
        printf("La carpeta '%s' no existe o no se pudo abrir.\n", carpeta);
        return 1;
    }

    /* Recolectar nombres de archivos regulares */
    char *nombres[4096];
    int total = 0;
    struct dirent *entrada;
    char ruta[MAX_RUTA];

    while ((entrada = readdir(dir)) != NULL && total < 4096) {
        snprintf(ruta, sizeof(ruta), "%s/%s", carpeta, entrada->d_name);
        struct stat info;
        if (stat(ruta, &info) == 0 && S_ISREG(info.st_mode)) {
            nombres[total++] = strdup(entrada->d_name);
        }
    }
    closedir(dir);

    if (total == 0) {
        printf("No se encontraron archivos en esta carpeta.\n");
        return 0;
    }

    qsort(nombres, total, sizeof(char *), comparar_nombres);

    printf("Carpeta analizada: %s\n\n", carpeta);
    printf("%-25s%-18s%-20s%s\n", "ARCHIVO", "EXT. DECLARADA", "TIPO REAL", "ESTADO");
    for (int i = 0; i < 80; i++) putchar('-');
    putchar('\n');

    for (int i = 0; i < total; i++) {
        snprintf(ruta, sizeof(ruta), "%s/%s", carpeta, nombres[i]);

        char extension[32];
        obtener_extension(nombres[i], extension, sizeof(extension));

        const char *tipo_real = identificar_tipo_real(ruta);
        const char *estado;
        char tipo_mostrado[64];

        if (tipo_real == NULL) {
            strcpy(tipo_mostrado, "no identificado");
            estado = "TIPO DESCONOCIDO";
        } else {
            strncpy(tipo_mostrado, tipo_real, sizeof(tipo_mostrado) - 1);
            tipo_mostrado[sizeof(tipo_mostrado) - 1] = '\0';
            estado = extension_coincide(extension, tipo_real) ? "VALIDO" : "NO VALIDO";
        }

        printf("%-25s%-18s%-20s%s\n",
               nombres[i],
               extension[0] ? extension : "(sin ext.)",
               tipo_mostrado,
               estado);

        free(nombres[i]);
    }

    return 0;
}

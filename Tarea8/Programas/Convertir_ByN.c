#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

// Definimos las implementaciones de STB
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX_ARCHIVOS 100
#define MAX_NOMBRE 256

// Función para verificar si el archivo tiene una extensión válida
int es_imagen_valida(const char *nombre) {
    const char *ext = strrchr(nombre, '.');
    if (!ext) return 0;

    return (strcasecmp(ext, ".png") == 0 ||
            strcasecmp(ext, ".jpg") == 0 ||
            strcasecmp(ext, ".jpeg") == 0 ||
            strcasecmp(ext, ".bmp") == 0);
}

// Función para listar imágenes en el directorio actual
int listar_imagenes(char lista[MAX_ARCHIVOS][MAX_NOMBRE]) {
    DIR *dir;
    struct dirent *entrada;
    int contador = 0;

    dir = opendir(".");
    if (dir == NULL) {
        printf("Error al abrir la carpeta actual.\n");
        return 0;
    }

    while ((entrada = readdir(dir)) != NULL && contador < MAX_ARCHIVOS) {
        if (es_imagen_valida(entrada->d_name)) {
            strncpy(lista[contador], entrada->d_name, MAX_NOMBRE);
            contador++;
        }
    }
    closedir(dir);
    return contador;
}

// Función para convertir la imagen seleccionada a blanco y negro
void convertir_a_blanco_y_negro(const char *nombre_archivo) {
    int ancho, alto, canales;

    // Forzamos la carga a 3 canales (RGB) para simplificar el proceso
    unsigned char *pixeles = stbi_load(nombre_archivo, &ancho, &alto, &canales, 3);

    if (!pixeles) {
        printf("No se pudo cargar la imagen: %s\n", nombre_archivo);
        return;
    }

    printf("\nProcesando: %s (%dx%d px)...\n", nombre_archivo, ancho, alto);

    // Modificamos los píxeles usando la fórmula de luminancia estándar:
    // Y = 0.2126*R + 0.7152*G + 0.0722*B
    for (int i = 0; i < ancho * alto * 3; i += 3) {
        unsigned char r = pixeles[i];
        unsigned char g = pixeles[i + 1];
        unsigned char b = pixeles[i + 2];

        // Calcular el gris
        unsigned char gris = (unsigned char)(0.2126f * r + 0.7152f * g + 0.0722f * b);

        // Asignar el mismo valor a los tres canales (RGB) crea el efecto B/N
        pixeles[i]     = gris;
        pixeles[i + 1] = gris;
        pixeles[i + 2] = gris;
    }

    // Generar el nombre del nuevo archivo (ej: foto.jpg -> bn_foto.jpg)
    char nombre_salida[MAX_NOMBRE + 4];
    snprintf(nombre_salida, sizeof(nombre_salida), "bn_%s", nombre_archivo);

    // Obtener la extensión para guardarlo en el formato correcto
    const char *ext = strrchr(nombre_archivo, '.');
    int guardado_exitoso = 0;

    if (strcasecmp(ext, ".png") == 0) {
        guardado_exitoso = stbi_write_png(nombre_salida, ancho, alto, 3, pixeles, ancho * 3);
    } else if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0) {
        guardado_exitoso = stbi_write_jpg(nombre_salida, ancho, alto, 3, pixeles, 100); // 100% calidad
    } else if (strcasecmp(ext, ".bmp") == 0) {
        guardado_exitoso = stbi_write_bmp(nombre_salida, ancho, alto, 3, pixeles);
    }

    if (guardado_exitoso) {
        printf("¡Imagen convertida con éxito! Guardada como: %s\n", nombre_salida);
    } else {
        printf("Error al guardar la nueva imagen.\n");
    }

    // Liberar la memoria asignada por STB
    stbi_image_free(pixeles);
}

int main() {
    char imagenes[MAX_ARCHIVOS][MAX_NOMBRE];
    int total_imagenes = listar_imagenes(imagenes);

    if (total_imagenes == 0) {
        printf("No se encontraron archivos .png, .jpg, .jpeg o .bmp en esta carpeta.\n");
        printf("Coloca este programa en la misma carpeta donde están tus fotos.\n");
        return 0;
    }

    int seleccion = -1;
    while (1) {
        printf("\n=== MENÚ DE CONVERSIÓN A BLANCO Y NEGRO ===\n");
        for (int i = 0; i < total_imagenes; i++) {
            printf("[%d] %s\n", i + 1, imagenes[i]);
        }
        printf("[0] Salir\n");
        printf("Seleccione el número del archivo que desea convertir: ");

        if (scanf("%d", &seleccion) != 1) {
            printf("Entrada no válida. Intente de nuevo.\n");
            while (getchar() != '\n'); // Limpiar el búfer de entrada
            continue;
        }

        if (seleccion == 0) {
            printf("Saliendo del programa...\n");
            break;
        }

        if (seleccion > 0 && seleccion <= total_imagenes) {
            // El índice en el arreglo es seleccion - 1
            convertir_a_blanco_y_negro(imagenes[seleccion - 1]);
        } else {
            printf("Opción fuera de rango. Intente de nuevo.\n");
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Variable global para contar los movimientos y poder numerarlos correctamente
int contador_movimientos = 0;

// Declaración de la función recursiva de Hanoi
void hanoi(int n, char ini, char fin, char ayu);

int main() {
    int dis;

    // Bucle do-while para validar que el número de discos sea mayor que cero
    do {
        printf("¿Cuantos discos quiere?: ");
        scanf("%d", &dis);

        if (dis <= 0) {
            printf("Error. Esto no va a jalar.\n\n");
        }
    } while (dis <= 0);

    // Inicializar/limpiar el archivo de texto al comenzar una nueva simulación
    FILE *archivo = fopen("movimientos_hanoi.txt", "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo de texto.\n");
        return 1;
    }
    fprintf(archivo, "--- INICIO DE MOVIMIENTOS (Torres de Hanoi con %d discos) ---\n", dis);
    fclose(archivo);

    printf("\nMovimientos (tambien se estan guardando en 'movimientos_hanoi.txt'):\n");

    // Llamada inicial a la función de Hanoi
    hanoi(dis, 'A', 'B', 'C');

    // Cálculo y muestra del total de movimientos usando la fórmula matemática (2^n - 1)
    int total = (int)pow(2, dis) - 1;
    printf("\nTotal movimientos: %d\n", total);

    // Guardar el total al final del archivo de texto
    archivo = fopen("movimientos_hanoi.txt", "a");
    if (archivo != NULL) {
        fprintf(archivo, "--- TOTAL MOVIMIENTOS: %d ---\n", total);
        fclose(archivo);
    }

    return 0;
}

// Definición del motor recursivo de las Torres de Hanoi
void hanoi(int n, char ini, char fin, char ayu) {
    // Abrir el archivo en modo "append" (añadir al final)
    FILE *archivo = fopen("movimientos_hanoi.txt", "a");
    if (archivo == NULL) {
        printf("Error al abrir el archivo durante la recursion.\n");
        return;
    }

    // Caso base: si solo queda un disco
    if (n == 1) {
        contador_movimientos++;
        // Imprime en pantalla
        printf("Movimiento %d: Mover disco 1 de %c a %c\n", contador_movimientos, ini, fin);
        // Graba en el archivo de texto
        fprintf(archivo, "Movimiento %d: Mover disco 1 de %c a %c\n", contador_movimientos, ini, fin);
        fclose(archivo);
        return;
    }

    // Cerramos el archivo temporalmente para que las sub-llamadas puedan usarlo
    fclose(archivo);

    // 1. Mover n-1 discos del origen (ini) al auxiliar (ayu)
    hanoi(n - 1, ini, ayu, fin);

    // Reabrimos el archivo para registrar el movimiento del disco actual (n)
    archivo = fopen("movimientos_hanoi.txt", "a");
    if (archivo != NULL) {
        contador_movimientos++;
        // Imprime en pantalla
        printf("Movimiento %d: Mover disco %d de %c a %c\n", contador_movimientos, n, ini, fin);
        // Graba en el archivo de texto
        fprintf(archivo, "Movimiento %d: Mover disco %d de %c a %c\n", contador_movimientos, n, ini, fin);
        fclose(archivo);
    }

    // 3. Mover los n-1 discos del auxiliar (ayu) al destino (fin)
    hanoi(n - 1, ayu, fin, ini);
}

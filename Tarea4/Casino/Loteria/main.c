#include <stdio.h>

int Generarnumerocasual(int semilla)
{
    int resultado;
    resultado = (semilla * 31 + 17) % 40;
    if (resultado == 0)
    {
        return 40;
    }
    return resultado;
}

void main(void)
{
    int opcion;
    int numJugadas = 0;
    int i;
    int numeroGanador;
    int huboGanador;
    int acumuladorSemilla = 0;
    int numerosApostados[100];
    int dineroJugado[100];

    do
    {
        printf("\n--- Menu de Loteria ---\n");
        printf("1. Elegir numero de jugadas\n");
        printf("2. Jugar\n");
        printf("3. Salir\n");
        printf("Seleccione una opcion: ");
        scanf("%d", &opcion);

        switch (opcion)
        {
        case 1:
            printf("\n¿Cuantas jugadas deseas realizar?: ");
            scanf("%d", &numJugadas);

            if (numJugadas <= 0 || numJugadas > 100)
            {
                printf("Cantidad invalida. Intente de nuevo.\n");
                numJugadas = 0;
            }
            else
            {
                printf("Se registraron %d jugadas.\n", numJugadas);
            }
            break;

        case 2:
            if (numJugadas == 0)
            {
                printf("\n[Error] Primero debe elegir la cantidad de jugadas en la opcion 1.\n");
                break;
            }
            for (i = 0; i < numJugadas; i++)
            {
               printf("\n--- JUGADA %d ---\n", i + 1);

               do
               {
                   printf("Pregunta 1 -> Ingrese el numero a apostar (1 al 40): ");
                   scanf("%d", &numerosApostados[i]);

                   if (numerosApostados[i] < 1 || numerosApostados[i] > 40)
                   {
                       printf("Numero incorrecto. Debe ser entre 1 y 40\n");
                   }
               }
               while (numerosApostados[i] < 1 || numerosApostados[i] > 40);

               printf("Pregunta 2 -> Ingrese la cantidad de dinero a jugar: ");
               scanf("%d", &dineroJugado[i]);

               acumuladorSemilla = acumuladorSemilla + numerosApostados[i] + dineroJugado[i];
            }
            printf("\nGenerando un numero casual de loteria...\n");
            numeroGanador = Generarnumerocasual(acumuladorSemilla);
            printf("El numero casual que salio es: %d\n", numeroGanador);

            huboGanador = 0;
            for (i = 0; i < numJugadas; i++)
            {
                if (numerosApostados[i] == numeroGanador)
                {
                    int premio = dineroJugado[i] * 1000;
                    printf("\n¡Felicidades! La jugada %d es Ganadora.\n", i + 1);
                    printf("Te sacaste con el numero %d y ganaste un premio de %d\n", numeroGanador, premio);
                    huboGanador = 1;
                }
            }
            if (huboGanador == 0)
            {
                printf("\nNadie salio ganador en esta ronda. Puede volver a intentar o salir.\n");
            }
            break;

        case 3:
            printf("\nSaliendo del programa de loteria. ¡Hasta Luego!\n");
            break;

        default:
            printf("\nOpcion no valida. Elija 1, 2 o 3,\n");
            break;
        }
    }
    while (opcion != 3);
}

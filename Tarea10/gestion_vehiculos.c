/* ============================================================================
   GESTOR DE COSTOS DE VEHICULOS -
   ============================================================================
   Calcula:
     - Amortizacion del vehiculo (costo / vida util)
     - Gastos de mantenimiento durante la vida util
     - Consumo de combustible promedio (en GALONES, como se usa en RD)
     - Costo por km en ciudad y en autopista
     - Costo REAL por km (incluye amortizacion + mantenimiento + seguro +
       neumaticos + combustible)
     - Costo de combustible y costo real de un viaje especifico

   Los vehiculos se guardan en el archivo "vehiculos.txt" y el precio del
   combustible en "combustible.txt", para que la informacion persista entre
   ejecuciones del programa.
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VEHICULOS   100
#define ARCHIVO_VEHIC   "vehiculos.txt"
#define ARCHIVO_COMB    "combustible.txt"
#define PRECIO_COMB_DEFAULT 285.00   /* RD$ por galon (ajustable por el usuario) */

typedef struct {
    int    id;
    char   nombre[50];
    double costo;                 /* precio de compra del vehiculo (RD$) */
    double valor_residual;        /* valor de reventa al final de la vida util */
    int    vida_util_anios;       /* vida util estimada en anios */
    double km_ciudad_anual;       /* km recorridos en ciudad por anio */
    double km_autopista_anual;    /* km recorridos en autopista por anio */
    double consumo_ciudad;        /* km recorridos por galon en ciudad */
    double consumo_autopista;     /* km recorridos por galon en autopista */
    double seguro_anual;          /* costo del seguro por anio (RD$) */
    double mantenimiento_anual;   /* costo de mantenimiento por anio (RD$) */
    double costo_neumaticos;      /* costo de un juego de neumaticos (RD$) */
    double vida_neumaticos_km;    /* duracion de un juego de neumaticos en km */
} Vehiculo;

/* -------------------------- utilidades de entrada ----------------------- */

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

double leerDouble(const char *mensaje) {
    double valor;
    while (1) {
        printf("%s", mensaje);
        if (scanf("%lf", &valor) == 1) { limpiarBuffer(); return valor; }
        printf("  -> Entrada invalida, intente de nuevo.\n");
        limpiarBuffer();
    }
}

int leerEntero(const char *mensaje) {
    int valor;
    while (1) {
        printf("%s", mensaje);
        if (scanf("%d", &valor) == 1) { limpiarBuffer(); return valor; }
        printf("  -> Entrada invalida, intente de nuevo.\n");
        limpiarBuffer();
    }
}

void leerTexto(const char *mensaje, char *destino, int tam) {
    printf("%s", mensaje);
    fgets(destino, tam, stdin);
    destino[strcspn(destino, "\n")] = '\0';   /* quitar salto de linea */
}

/* ------------------------- persistencia en archivo ----------------------- */

int cargarVehiculos(Vehiculo v[]) {
    FILE *f = fopen(ARCHIVO_VEHIC, "r");
    int n = 0;
    if (!f) return 0;
    while (n < MAX_VEHICULOS &&
           fscanf(f, "%d;%49[^;];%lf;%lf;%d;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf\n",
                  &v[n].id, v[n].nombre, &v[n].costo, &v[n].valor_residual,
                  &v[n].vida_util_anios, &v[n].km_ciudad_anual,
                  &v[n].km_autopista_anual, &v[n].consumo_ciudad,
                  &v[n].consumo_autopista, &v[n].seguro_anual,
                  &v[n].mantenimiento_anual, &v[n].costo_neumaticos,
                  &v[n].vida_neumaticos_km) == 13) {
        n++;
    }
    fclose(f);
    return n;
}

void guardarVehiculos(Vehiculo v[], int n) {
    FILE *f = fopen(ARCHIVO_VEHIC, "w");
    if (!f) { printf("Error al guardar el archivo de vehiculos.\n"); return; }
    for (int i = 0; i < n; i++) {
        fprintf(f, "%d;%s;%.2f;%.2f;%d;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f\n",
                v[i].id, v[i].nombre, v[i].costo, v[i].valor_residual,
                v[i].vida_util_anios, v[i].km_ciudad_anual, v[i].km_autopista_anual,
                v[i].consumo_ciudad, v[i].consumo_autopista, v[i].seguro_anual,
                v[i].mantenimiento_anual, v[i].costo_neumaticos, v[i].vida_neumaticos_km);
    }
    fclose(f);
}

double cargarPrecioCombustible(void) {
    FILE *f = fopen(ARCHIVO_COMB, "r");
    double precio;
    if (!f) return PRECIO_COMB_DEFAULT;
    if (fscanf(f, "%lf", &precio) != 1) precio = PRECIO_COMB_DEFAULT;
    fclose(f);
    return precio;
}

void guardarPrecioCombustible(double precio) {
    FILE *f = fopen(ARCHIVO_COMB, "w");
    if (!f) { printf("Error al guardar el precio del combustible.\n"); return; }
    fprintf(f, "%.2f\n", precio);
    fclose(f);
}

/* ------------------------------ busqueda --------------------------------- */

int buscarPorId(Vehiculo v[], int n, int id) {
    for (int i = 0; i < n; i++) if (v[i].id == id) return i;
    return -1;
}

int siguienteId(Vehiculo v[], int n) {
    int max = 0;
    for (int i = 0; i < n; i++) if (v[i].id > max) max = v[i].id;
    return max + 1;
}

/* ------------------------- calculos principales --------------------------
   Los costos "fijos" (amortizacion, mantenimiento, seguro, neumaticos) se
   reparten entre TODOS los km que recorre el vehiculo al anio (ciudad +
   autopista), porque ocurren sin importar donde se maneje. El costo de
   COMBUSTIBLE si depende de si el km fue en ciudad o en autopista, porque
   el consumo (km/galon) normalmente es distinto en cada caso.
   ---------------------------------------------------------------------- */

typedef struct {
    double km_total_anual;
    double amortizacion_km;
    double mantenimiento_km;
    double seguro_km;
    double neumaticos_km;
    double costo_fijo_km;         /* suma de los 4 anteriores */
    double combustible_ciudad_km;
    double combustible_autopista_km;
    double costo_ciudad_km;       /* fijo + combustible ciudad */
    double costo_autopista_km;    /* fijo + combustible autopista */
    double costo_promedio_km;     /* promedio ponderado segun uso real */
} AnalisisCostos;

int calcularCostos(Vehiculo v, double precio_comb, AnalisisCostos *a) {
    a->km_total_anual = v.km_ciudad_anual + v.km_autopista_anual;
    if (a->km_total_anual <= 0 || v.vida_util_anios <= 0 ||
        v.consumo_ciudad <= 0 || v.consumo_autopista <= 0 ||
        v.vida_neumaticos_km <= 0) {
        return 0; /* datos invalidos, evita division entre cero */
    }

    a->amortizacion_km   = ((v.costo - v.valor_residual) / v.vida_util_anios) / a->km_total_anual;
    a->mantenimiento_km  = v.mantenimiento_anual / a->km_total_anual;
    a->seguro_km         = v.seguro_anual / a->km_total_anual;
    a->neumaticos_km     = v.costo_neumaticos / v.vida_neumaticos_km;
    a->costo_fijo_km     = a->amortizacion_km + a->mantenimiento_km + a->seguro_km + a->neumaticos_km;

    a->combustible_ciudad_km    = precio_comb / v.consumo_ciudad;
    a->combustible_autopista_km = precio_comb / v.consumo_autopista;

    a->costo_ciudad_km    = a->costo_fijo_km + a->combustible_ciudad_km;
    a->costo_autopista_km = a->costo_fijo_km + a->combustible_autopista_km;

    a->costo_promedio_km = (v.km_ciudad_anual * a->costo_ciudad_km +
                             v.km_autopista_anual * a->costo_autopista_km) / a->km_total_anual;
    return 1;
}

void mostrarAnalisis(Vehiculo v, double precio_comb) {
    AnalisisCostos a;
    if (!calcularCostos(v, precio_comb, &a)) {
        printf("\n  No se puede calcular: revise que km anuales, vida util,\n");
        printf("  consumo y vida de neumaticos sean mayores que cero.\n");
        return;
    }
    double mantenimiento_total_vida = v.mantenimiento_anual * v.vida_util_anios;

    printf("\n================ ANALISIS DE COSTOS: %s ================\n", v.nombre);
    printf("Precio de combustible usado: RD$ %.2f por galon\n\n", precio_comb);

    printf("-- Amortizacion --\n");
    printf("  Amortizacion anual        : RD$ %.2f\n",
           (v.costo - v.valor_residual) / v.vida_util_anios);
    printf("  Amortizacion por km       : RD$ %.2f\n", a.amortizacion_km);

    printf("\n-- Mantenimiento (vida util de %d anios) --\n", v.vida_util_anios);
    printf("  Mantenimiento total       : RD$ %.2f\n", mantenimiento_total_vida);
    printf("  Mantenimiento por km      : RD$ %.2f\n", a.mantenimiento_km);

    printf("\n-- Seguro y neumaticos --\n");
    printf("  Seguro por km             : RD$ %.2f\n", a.seguro_km);
    printf("  Neumaticos por km         : RD$ %.2f\n", a.neumaticos_km);

    printf("\n-- Combustible --\n");
    printf("  Consumo ciudad            : %.2f km/galon -> RD$ %.2f por km\n",
           v.consumo_ciudad, a.combustible_ciudad_km);
    printf("  Consumo autopista         : %.2f km/galon -> RD$ %.2f por km\n",
           v.consumo_autopista, a.combustible_autopista_km);

    printf("\n-- Costo total por km --\n");
    printf("  Costo fijo por km (sin comb.): RD$ %.2f\n", a.costo_fijo_km);
    printf("  COSTO REAL EN CIUDAD         : RD$ %.2f por km\n", a.costo_ciudad_km);
    printf("  COSTO REAL EN AUTOPISTA      : RD$ %.2f por km\n", a.costo_autopista_km);
    printf("  COSTO PROMEDIO PONDERADO     : RD$ %.2f por km\n", a.costo_promedio_km);
    printf("=========================================================\n");
}

/* ------------------------------- opciones --------------------------------- */

void crearVehiculo(Vehiculo v[], int *n) {
    if (*n >= MAX_VEHICULOS) { printf("Limite de vehiculos alcanzado.\n"); return; }
    Vehiculo nuevo;
    nuevo.id = siguienteId(v, *n);
    leerTexto("Nombre/modelo del vehiculo: ", nuevo.nombre, sizeof(nuevo.nombre));
    nuevo.costo               = leerDouble("Costo de compra (RD$): ");
    nuevo.valor_residual      = leerDouble("Valor residual al final de vida util (RD$): ");
    nuevo.vida_util_anios     = leerEntero("Vida util estimada (anios): ");
    nuevo.km_ciudad_anual     = leerDouble("Km recorridos en CIUDAD por anio: ");
    nuevo.km_autopista_anual  = leerDouble("Km recorridos en AUTOPISTA por anio: ");
    nuevo.consumo_ciudad      = leerDouble("Consumo en CIUDAD (km por galon): ");
    nuevo.consumo_autopista   = leerDouble("Consumo en AUTOPISTA (km por galon): ");
    nuevo.seguro_anual        = leerDouble("Costo del seguro por anio (RD$): ");
    nuevo.mantenimiento_anual = leerDouble("Costo de mantenimiento por anio (RD$): ");
    nuevo.costo_neumaticos    = leerDouble("Costo de un juego de neumaticos (RD$): ");
    nuevo.vida_neumaticos_km  = leerDouble("Duracion de los neumaticos (km): ");

    v[*n] = nuevo;
    (*n)++;
    guardarVehiculos(v, *n);
    printf("\nVehiculo '%s' guardado con ID %d.\n", nuevo.nombre, nuevo.id);
}

void borrarVehiculo(Vehiculo v[], int *n) {
    int id = leerEntero("ID del vehiculo a borrar: ");
    int idx = buscarPorId(v, *n, id);
    if (idx == -1) { printf("No existe un vehiculo con ese ID.\n"); return; }
    printf("Se eliminara '%s'. Confirmar (1=Si, 0=No): ", v[idx].nombre);
    int conf = leerEntero("");
    if (!conf) { printf("Cancelado.\n"); return; }
    for (int i = idx; i < *n - 1; i++) v[i] = v[i + 1];
    (*n)--;
    guardarVehiculos(v, *n);
    printf("Vehiculo eliminado.\n");
}

void listarVehiculos(Vehiculo v[], int n) {
    if (n == 0) { printf("No hay vehiculos registrados.\n"); return; }
    printf("\n%-4s %-25s %-14s %-10s\n", "ID", "Nombre", "Costo (RD$)", "Vida util");
    for (int i = 0; i < n; i++) {
        printf("%-4d %-25s %-14.2f %-10d\n",
               v[i].id, v[i].nombre, v[i].costo, v[i].vida_util_anios);
    }
}

void modificarVehiculo(Vehiculo v[], int n) {
    listarVehiculos(v, n);
    if (n == 0) return;
    int id = leerEntero("\nID del vehiculo a modificar: ");
    int idx = buscarPorId(v, n, id);
    if (idx == -1) { printf("No existe un vehiculo con ese ID.\n"); return; }

    int opc;
    do {
        printf("\n--- Modificar '%s' ---\n", v[idx].nombre);
        printf(" 1. Nombre               (actual: %s)\n", v[idx].nombre);
        printf(" 2. Costo de compra      (actual: %.2f)\n", v[idx].costo);
        printf(" 3. Valor residual       (actual: %.2f)\n", v[idx].valor_residual);
        printf(" 4. Vida util (anios)    (actual: %d)\n", v[idx].vida_util_anios);
        printf(" 5. Km ciudad/anio       (actual: %.2f)\n", v[idx].km_ciudad_anual);
        printf(" 6. Km autopista/anio    (actual: %.2f)\n", v[idx].km_autopista_anual);
        printf(" 7. Consumo ciudad       (actual: %.2f km/gal)\n", v[idx].consumo_ciudad);
        printf(" 8. Consumo autopista    (actual: %.2f km/gal)\n", v[idx].consumo_autopista);
        printf(" 9. Seguro anual         (actual: %.2f)\n", v[idx].seguro_anual);
        printf("10. Mantenimiento anual  (actual: %.2f)\n", v[idx].mantenimiento_anual);
        printf("11. Costo neumaticos     (actual: %.2f)\n", v[idx].costo_neumaticos);
        printf("12. Vida neumaticos (km) (actual: %.2f)\n", v[idx].vida_neumaticos_km);
        printf(" 0. Terminar\n");
        opc = leerEntero("Seleccione campo a modificar: ");
        switch (opc) {
            case 1:  leerTexto("Nuevo nombre: ", v[idx].nombre, sizeof(v[idx].nombre)); break;
            case 2:  v[idx].costo               = leerDouble("Nuevo costo: "); break;
            case 3:  v[idx].valor_residual      = leerDouble("Nuevo valor residual: "); break;
            case 4:  v[idx].vida_util_anios     = leerEntero("Nueva vida util (anios): "); break;
            case 5:  v[idx].km_ciudad_anual     = leerDouble("Nuevos km ciudad/anio: "); break;
            case 6:  v[idx].km_autopista_anual  = leerDouble("Nuevos km autopista/anio: "); break;
            case 7:  v[idx].consumo_ciudad      = leerDouble("Nuevo consumo ciudad (km/gal): "); break;
            case 8:  v[idx].consumo_autopista   = leerDouble("Nuevo consumo autopista (km/gal): "); break;
            case 9:  v[idx].seguro_anual        = leerDouble("Nuevo seguro anual: "); break;
            case 10: v[idx].mantenimiento_anual = leerDouble("Nuevo mantenimiento anual: "); break;
            case 11: v[idx].costo_neumaticos    = leerDouble("Nuevo costo neumaticos: "); break;
            case 12: v[idx].vida_neumaticos_km  = leerDouble("Nueva vida neumaticos (km): "); break;
            case 0:  break;
            default: printf("Opcion invalida.\n");
        }
    } while (opc != 0);

    guardarVehiculos(v, n);
    printf("Cambios guardados.\n");
}

void calcularViaje(Vehiculo v[], int n, double precio_comb) {
    listarVehiculos(v, n);
    if (n == 0) return;
    int id = leerEntero("\nID del vehiculo para el viaje: ");
    int idx = buscarPorId(v, n, id);
    if (idx == -1) { printf("No existe un vehiculo con ese ID.\n"); return; }

    double km_ciudad    = leerDouble("Km del viaje en CIUDAD: ");
    double km_autopista = leerDouble("Km del viaje en AUTOPISTA: ");

    AnalisisCostos a;
    if (!calcularCostos(v[idx], precio_comb, &a)) {
        printf("No se puede calcular el viaje: revise los datos del vehiculo.\n");
        return;
    }

    double galones_ciudad    = (v[idx].consumo_ciudad > 0)    ? km_ciudad / v[idx].consumo_ciudad : 0;
    double galones_autopista = (v[idx].consumo_autopista > 0) ? km_autopista / v[idx].consumo_autopista : 0;
    double galones_totales   = galones_ciudad + galones_autopista;
    double costo_combustible = galones_totales * precio_comb;
    double costo_real_total  = km_ciudad * a.costo_ciudad_km + km_autopista * a.costo_autopista_km;

    printf("\n================ RESUMEN DEL VIAJE: %s ================\n", v[idx].nombre);
    printf("Km en ciudad: %.2f   Km en autopista: %.2f   Total: %.2f km\n",
           km_ciudad, km_autopista, km_ciudad + km_autopista);
    printf("\nGalones consumidos en ciudad    : %.3f gal\n", galones_ciudad);
    printf("Galones consumidos en autopista  : %.3f gal\n", galones_autopista);
    printf("Galones totales                  : %.3f gal\n", galones_totales);
    printf("COSTO DE COMBUSTIBLE DEL VIAJE    : RD$ %.2f\n", costo_combustible);
    printf("\nCOSTO REAL TOTAL DEL VIAJE (incl.\n");
    printf("amortizacion, mantenimiento, seguro\n");
    printf("y neumaticos)                     : RD$ %.2f\n", costo_real_total);
    printf("=========================================================\n");
}

void modificarPrecioCombustible(double *precio) {
    printf("Precio actual del combustible: RD$ %.2f por galon\n", *precio);
    double nuevo = leerDouble("Nuevo precio por galon (RD$): ");
    *precio = nuevo;
    guardarPrecioCombustible(*precio);
    printf("Precio actualizado.\n");
}

/* --------------------------------- main ----------------------------------- */

int main(void) {
    Vehiculo vehiculos[MAX_VEHICULOS];
    int n = cargarVehiculos(vehiculos);
    double precio_comb = cargarPrecioCombustible();

    int opcion;
    do {
        printf("\n============ GESTOR DE COSTOS DE VEHICULOS (RD) ============\n");
        printf("Precio de combustible actual: RD$ %.2f/galon\n", precio_comb);
        printf("--------------------------------------------------------------\n");
        printf("1. Crear vehiculo\n");
        printf("2. Borrar vehiculo\n");
        printf("3. Modificar datos de un vehiculo\n");
        printf("4. Ver analisis de costos de un vehiculo (costo por km)\n");
        printf("5. Calcular un viaje\n");
        printf("6. Modificar precio del combustible\n");
        printf("7. Listar vehiculos\n");
        printf("0. Salir\n");
        opcion = leerEntero("Seleccione una opcion: ");

        switch (opcion) {
            case 1: crearVehiculo(vehiculos, &n); break;
            case 2: borrarVehiculo(vehiculos, &n); break;
            case 3: modificarVehiculo(vehiculos, n); break;
            case 4: {
                listarVehiculos(vehiculos, n);
                if (n > 0) {
                    int id = leerEntero("\nID del vehiculo a analizar: ");
                    int idx = buscarPorId(vehiculos, n, id);
                    if (idx != -1) mostrarAnalisis(vehiculos[idx], precio_comb);
                    else printf("No existe un vehiculo con ese ID.\n");
                }
                break;
            }
            case 5: calcularViaje(vehiculos, n, precio_comb); break;
            case 6: modificarPrecioCombustible(&precio_comb); break;
            case 7: listarVehiculos(vehiculos, n); break;
            case 0: printf("Hasta luego.\n"); break;
            default: printf("Opcion invalida.\n");
        }
    } while (opcion != 0);

    return 0;
}

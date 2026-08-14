/* ============================================================================
   GESTION DE RIESGOS - METODO MOSLER
   ----------------------------------------------------------------------------
   El metodo Mosler evalua la GRAVEDAD DE UN RIESGO (GR) combinando 6
   criterios, cada uno calificado de 1 a 5:

     F = Funcion        -> importancia de lo que se pone en riesgo
     S = Sustitucion     -> facilidad para reemplazar lo danado
     P = Profundidad     -> efecto perturbador / repercusion
     E = Extension       -> alcance del dano (personas, area, etc.)
     A = Agresion         -> probabilidad de que el riesgo se presente
     V = Vulnerabilidad   -> probabilidad de que, presentado el riesgo,
                             efectivamente cause dano

   Formulas:
     C  = F + S + P + E      (Criterio de Consecuencias)
     GR = C * A * V          (Gravedad del Riesgo)

   Clasificacion utilizada en este programa:
     GR <  3             -> Muy Baja
     3  <= GR <  5        -> Baja
     5  <= GR <  8        -> Media
     8  <= GR < 13        -> Alta
     13 <= GR < 21        -> Muy Alta
     GR >= 21             -> Gravisima

   ----------------------------------------------------------------------------
   COMPILACION:
     gcc mosler.c -o mosler

   Para habilitar la consulta real a la API de Claude (item "Usar IA" con
   IA verdadera, no solo la heuristica local) se necesita libcurl y una
   variable de entorno ANTHROPIC_API_KEY:

     gcc mosler.c -o mosler -DUSE_CLAUDE_API -lcurl
     export ANTHROPIC_API_KEY="tu_api_key"
     ./mosler

   Sin esa bandera, el programa usa automaticamente una IA "heuristica"
   local (reglas expertas) que no requiere internet.
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef USE_CLAUDE_API
#include <curl/curl.h>
#endif

#define TAM_NOMBRE 80
#define TAM_DESC   240
#define TAM_LINEA  1024
#define TAM_RUTA   260

/* ---------------------------- ESTRUCTURA -------------------------------- */

typedef struct {
    int id;
    char nombre[TAM_NOMBRE];
    char descripcion[TAM_DESC];
    int F, S, P, E, A, V;   /* criterios 1..5 */
    double C;                /* consecuencias  */
    double GR;                /* gravedad del riesgo */
    char clasificacion[20];
} Riesgo;

/* ---------------------------- VARIABLES GLOBALES ------------------------- */

static Riesgo *riesgos = NULL;      /* arreglo dinamico */
static int totalRiesgos = 0;
static int capacidadRiesgos = 0;
static char archivoActual[TAM_RUTA] = "riesgos.txt";

/* ---------------------------- UTILIDADES DE ENTRADA ----------------------- */

static void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static int leerEntero(const char *mensaje, int min, int max) {
    char linea[128];
    int valor, ok;
    do {
        printf("%s", mensaje);
        if (!fgets(linea, sizeof(linea), stdin)) { exit(0); }
        ok = (sscanf(linea, "%d", &valor) == 1) && valor >= min && valor <= max;
        if (!ok) {
            printf("  -> Valor invalido. Debe estar entre %d y %d.\n", min, max);
        }
    } while (!ok);
    return valor;
}

static void leerCadena(const char *mensaje, char *destino, int tam) {
    printf("%s", mensaje);
    if (!fgets(destino, tam, stdin)) { destino[0] = '\0'; return; }
    destino[strcspn(destino, "\n")] = '\0';
}

/* ---------------------------- MANEJO DEL ARREGLO DINAMICO ----------------- */

static void asegurarCapacidad(void) {
    if (totalRiesgos < capacidadRiesgos) return;
    capacidadRiesgos = (capacidadRiesgos == 0) ? 4 : capacidadRiesgos * 2;
    riesgos = (Riesgo *) realloc(riesgos, (size_t) capacidadRiesgos * sizeof(Riesgo));
    if (!riesgos) {
        fprintf(stderr, "Error: no hay memoria suficiente.\n");
        exit(1);
    }
}

static int obtenerIndicePorId(int id) {
    int i;
    for (i = 0; i < totalRiesgos; i++) {
        if (riesgos[i].id == id) return i;
    }
    return -1;
}

static int siguienteId(void) {
    int i, max = 0;
    for (i = 0; i < totalRiesgos; i++) {
        if (riesgos[i].id > max) max = riesgos[i].id;
    }
    return max + 1;
}

/* ---------------------------- CALCULO Y CLASIFICACION --------------------- */

static void clasificarRiesgo(Riesgo *r) {
    if (r->GR < 3.0)       strcpy(r->clasificacion, "Muy Baja");
    else if (r->GR < 5.0)  strcpy(r->clasificacion, "Baja");
    else if (r->GR < 8.0)  strcpy(r->clasificacion, "Media");
    else if (r->GR < 13.0) strcpy(r->clasificacion, "Alta");
    else if (r->GR < 21.0) strcpy(r->clasificacion, "Muy Alta");
    else                    strcpy(r->clasificacion, "Gravisima");
}

static void calcularRiesgo(Riesgo *r) {
    r->C  = r->F + r->S + r->P + r->E;
    r->GR = r->C * r->A * r->V;
    clasificarRiesgo(r);
}

static void calcularRiesgos(void) {
    int i;
    if (totalRiesgos == 0) {
        printf("No hay riesgos registrados.\n");
        return;
    }
    for (i = 0; i < totalRiesgos; i++) {
        calcularRiesgo(&riesgos[i]);
    }
    printf("Se recalcularon %d riesgo(s).\n", totalRiesgos);
}

/* ---------------------------- OPCION 1: CREAR RIESGO ----------------------- */

static void crearRiesgo(void) {
    Riesgo nuevo;
    memset(&nuevo, 0, sizeof(nuevo));

    nuevo.id = siguienteId();
    leerCadena("Nombre del riesgo: ", nuevo.nombre, TAM_NOMBRE);
    leerCadena("Descripcion: ", nuevo.descripcion, TAM_DESC);

    printf("Ingrese los 6 criterios de Mosler (escala 1 a 5):\n");
    nuevo.F = leerEntero("  F - Funcion:        ", 1, 5);
    nuevo.S = leerEntero("  S - Sustitucion:     ", 1, 5);
    nuevo.P = leerEntero("  P - Profundidad:     ", 1, 5);
    nuevo.E = leerEntero("  E - Extension:       ", 1, 5);
    nuevo.A = leerEntero("  A - Agresion:        ", 1, 5);
    nuevo.V = leerEntero("  V - Vulnerabilidad:  ", 1, 5);

    calcularRiesgo(&nuevo);

    asegurarCapacidad();
    riesgos[totalRiesgos++] = nuevo;

    printf("Riesgo creado con ID %d. GR = %.2f (%s)\n",
           nuevo.id, nuevo.GR, nuevo.clasificacion);
}

/* ---------------------------- OPCION 2: MODIFICAR RIESGO ------------------- */

static void mostrarListaResumen(void) {
    int i;
    if (totalRiesgos == 0) {
        printf("No hay riesgos registrados.\n");
        return;
    }
    printf("%-4s %-30s %-10s\n", "ID", "Nombre", "GR");
    for (i = 0; i < totalRiesgos; i++) {
        printf("%-4d %-30s %-10.2f\n", riesgos[i].id, riesgos[i].nombre, riesgos[i].GR);
    }
}

static void modificarRiesgo(void) {
    int id, idx, opcion;
    mostrarListaResumen();
    if (totalRiesgos == 0) return;

    id = leerEntero("\nID del riesgo a modificar: ", 0, 1000000);
    idx = obtenerIndicePorId(id);
    if (idx == -1) { printf("No existe un riesgo con ese ID.\n"); return; }

    do {
        printf("\n--- Modificar riesgo #%d: %s ---\n", riesgos[idx].id, riesgos[idx].nombre);
        printf("1. Nombre\n2. Descripcion\n3. F\n4. S\n5. P\n6. E\n7. A\n8. V\n0. Terminar\n");
        opcion = leerEntero("Elija campo a modificar: ", 0, 8);
        switch (opcion) {
            case 1: leerCadena("Nuevo nombre: ", riesgos[idx].nombre, TAM_NOMBRE); break;
            case 2: leerCadena("Nueva descripcion: ", riesgos[idx].descripcion, TAM_DESC); break;
            case 3: riesgos[idx].F = leerEntero("Nuevo valor F (1-5): ", 1, 5); break;
            case 4: riesgos[idx].S = leerEntero("Nuevo valor S (1-5): ", 1, 5); break;
            case 5: riesgos[idx].P = leerEntero("Nuevo valor P (1-5): ", 1, 5); break;
            case 6: riesgos[idx].E = leerEntero("Nuevo valor E (1-5): ", 1, 5); break;
            case 7: riesgos[idx].A = leerEntero("Nuevo valor A (1-5): ", 1, 5); break;
            case 8: riesgos[idx].V = leerEntero("Nuevo valor V (1-5): ", 1, 5); break;
            default: break;
        }
    } while (opcion != 0);

    calcularRiesgo(&riesgos[idx]);
    printf("Riesgo actualizado. GR = %.2f (%s)\n", riesgos[idx].GR, riesgos[idx].clasificacion);
}

/* ---------------------------- OPCION 4: IMPRIMIR RIESGOS ------------------- */

static void imprimirRiesgos(void) {
    int i;
    if (totalRiesgos == 0) {
        printf("No hay riesgos registrados.\n");
        return;
    }
    printf("\n%-4s %-22s %-4s %-4s %-4s %-4s %-4s %-4s %-6s %-8s %-10s\n",
           "ID", "Nombre", "F", "S", "P", "E", "A", "V", "C", "GR", "Clasific.");
    for (i = 0; i < totalRiesgos; i++) {
        Riesgo *r = &riesgos[i];
        printf("%-4d %-22s %-4d %-4d %-4d %-4d %-4d %-4d %-6.1f %-8.2f %-10s\n",
               r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V, r->C, r->GR, r->clasificacion);
    }
}

/* ---------------------------- OPCION 5: BORRAR RIESGO ---------------------- */

static void borrarRiesgo(void) {
    int id, idx, i;
    mostrarListaResumen();
    if (totalRiesgos == 0) return;

    id = leerEntero("\nID del riesgo a borrar: ", 0, 1000000);
    idx = obtenerIndicePorId(id);
    if (idx == -1) { printf("No existe un riesgo con ese ID.\n"); return; }

    for (i = idx; i < totalRiesgos - 1; i++) {
        riesgos[i] = riesgos[i + 1];
    }
    totalRiesgos--;
    printf("Riesgo %d eliminado.\n", id);
}

/* ---------------------------- SELECCION / CREACION DE ARCHIVO ------------- */

static void seleccionarArchivo(void) {
    char ruta[TAM_RUTA];
    FILE *f;
    leerCadena("Ruta del archivo a usar (existente): ", ruta, TAM_RUTA);
    f = fopen(ruta, "r");
    if (!f) {
        printf("Ese archivo no existe todavia. Puede crearlo con la opcion "
               "'Crear archivo nuevo'.\n");
        return;
    }
    fclose(f);
    strncpy(archivoActual, ruta, TAM_RUTA - 1);
    archivoActual[TAM_RUTA - 1] = '\0';
    printf("Archivo activo: %s\n", archivoActual);
}

static void crearArchivoNuevo(void) {
    char ruta[TAM_RUTA];
    FILE *f;
    leerCadena("Nombre del nuevo archivo (ej: riesgos.json): ", ruta, TAM_RUTA);
    f = fopen(ruta, "w");
    if (!f) {
        printf("No se pudo crear el archivo (revise la ruta/permisos).\n");
        return;
    }
    fclose(f);
    strncpy(archivoActual, ruta, TAM_RUTA - 1);
    archivoActual[TAM_RUTA - 1] = '\0';
    printf("Archivo '%s' creado y establecido como archivo activo.\n", archivoActual);
}

/* ---------------------------- ESCAPES PARA JSON / XML ---------------------- */

static void escaparJSON(const char *origen, char *destino, int tam) {
    int i = 0, j = 0;
    for (i = 0; origen[i] != '\0' && j < tam - 2; i++) {
        char c = origen[i];
        if (c == '"' || c == '\\') { destino[j++] = '\\'; destino[j++] = c; }
        else if (c == '\n') { destino[j++] = ' '; }
        else { destino[j++] = c; }
    }
    destino[j] = '\0';
}

static void escaparXML(const char *origen, char *destino, int tam) {
    int i = 0, j = 0;
    for (i = 0; origen[i] != '\0' && j < tam - 6; i++) {
        char c = origen[i];
        if (c == '&')      { strcpy(&destino[j], "&amp;");  j += 5; }
        else if (c == '<') { strcpy(&destino[j], "&lt;");   j += 4; }
        else if (c == '>') { strcpy(&destino[j], "&gt;");   j += 4; }
        else if (c == '"') { strcpy(&destino[j], "&quot;"); j += 6; }
        else { destino[j++] = c; }
    }
    destino[j] = '\0';
}

/* ---------------------------- OPCION 9: GUARDAR EN ARCHIVO ----------------- */

static void guardarTXT(FILE *f) {
    int i;
    for (i = 0; i < totalRiesgos; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f, "===== RIESGO %d =====\n", r->id);
        fprintf(f, "Nombre: %s\n", r->nombre);
        fprintf(f, "Descripcion: %s\n", r->descripcion);
        fprintf(f, "F=%d S=%d P=%d E=%d A=%d V=%d\n", r->F, r->S, r->P, r->E, r->A, r->V);
        fprintf(f, "C=%.2f\n", r->C);
        fprintf(f, "GR=%.2f\n", r->GR);
        fprintf(f, "Clasificacion=%s\n\n", r->clasificacion);
    }
}

static void guardarCSV(FILE *f) {
    int i;
    fprintf(f, "id,nombre,descripcion,F,S,P,E,A,V,C,GR,clasificacion\n");
    for (i = 0; i < totalRiesgos; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f, "%d,\"%s\",\"%s\",%d,%d,%d,%d,%d,%d,%.2f,%.2f,%s\n",
                r->id, r->nombre, r->descripcion, r->F, r->S, r->P, r->E, r->A, r->V,
                r->C, r->GR, r->clasificacion);
    }
}

static void guardarJSON(FILE *f) {
    int i;
    char nomEsc[TAM_NOMBRE * 2], descEsc[TAM_DESC * 2];
    fprintf(f, "[\n");
    for (i = 0; i < totalRiesgos; i++) {
        Riesgo *r = &riesgos[i];
        escaparJSON(r->nombre, nomEsc, sizeof(nomEsc));
        escaparJSON(r->descripcion, descEsc, sizeof(descEsc));
        fprintf(f, "  {\n");
        fprintf(f, "    \"id\": %d,\n", r->id);
        fprintf(f, "    \"nombre\": \"%s\",\n", nomEsc);
        fprintf(f, "    \"descripcion\": \"%s\",\n", descEsc);
        fprintf(f, "    \"F\": %d, \"S\": %d, \"P\": %d, \"E\": %d, \"A\": %d, \"V\": %d,\n",
                r->F, r->S, r->P, r->E, r->A, r->V);
        fprintf(f, "    \"C\": %.2f,\n", r->C);
        fprintf(f, "    \"GR\": %.2f,\n", r->GR);
        fprintf(f, "    \"clasificacion\": \"%s\"\n", r->clasificacion);
        fprintf(f, "  }%s\n", (i == totalRiesgos - 1) ? "" : ",");
    }
    fprintf(f, "]\n");
}

static void guardarXML(FILE *f) {
    int i;
    char nomEsc[TAM_NOMBRE * 6], descEsc[TAM_DESC * 6];
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<riesgos>\n");
    for (i = 0; i < totalRiesgos; i++) {
        Riesgo *r = &riesgos[i];
        escaparXML(r->nombre, nomEsc, sizeof(nomEsc));
        escaparXML(r->descripcion, descEsc, sizeof(descEsc));
        fprintf(f, "  <riesgo>\n");
        fprintf(f, "    <id>%d</id>\n", r->id);
        fprintf(f, "    <nombre>%s</nombre>\n", nomEsc);
        fprintf(f, "    <descripcion>%s</descripcion>\n", descEsc);
        fprintf(f, "    <F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>\n",
                r->F, r->S, r->P, r->E, r->A, r->V);
        fprintf(f, "    <C>%.2f</C>\n", r->C);
        fprintf(f, "    <GR>%.2f</GR>\n", r->GR);
        fprintf(f, "    <clasificacion>%s</clasificacion>\n", r->clasificacion);
        fprintf(f, "  </riesgo>\n");
    }
    fprintf(f, "</riesgos>\n");
}

static void guardarArchivo(void) {
    int opcion;
    FILE *f;
    char ruta[TAM_RUTA];

    if (totalRiesgos == 0) { printf("No hay riesgos para guardar.\n"); return; }

    printf("Archivo activo: %s\n", archivoActual);
    printf("1. Usar archivo activo\n2. Escribir otra ruta\n");
    opcion = leerEntero("Opcion: ", 1, 2);
    if (opcion == 2) {
        leerCadena("Nueva ruta: ", ruta, TAM_RUTA);
        strncpy(archivoActual, ruta, TAM_RUTA - 1);
        archivoActual[TAM_RUTA - 1] = '\0';
    }

    printf("Formato de salida:\n1. Texto plano (.txt)\n2. CSV (.csv)\n3. JSON (.json)\n4. XML (.xml)\n");
    opcion = leerEntero("Elija formato: ", 1, 4);

    f = fopen(archivoActual, "w");
    if (!f) { printf("No se pudo abrir '%s' para escritura.\n", archivoActual); return; }

    switch (opcion) {
        case 1: guardarTXT(f);  break;
        case 2: guardarCSV(f);  break;
        case 3: guardarJSON(f); break;
        case 4: guardarXML(f);  break;
        default: break;
    }
    fclose(f);
    printf("Guardado en '%s'.\n", archivoActual);
}

/* ---------------------------- OPCION 8: CARGAR CON MEMORIA DINAMICA -------- */
/* Nota: los parseadores de JSON y XML aqui son deliberadamente simples:
   funcionan con archivos generados por este mismo programa (formato fijo),
   no son parseadores JSON/XML de proposito general. */

static void agregarRiesgoCargado(Riesgo r) {
    calcularRiesgo(&r);
    asegurarCapacidad();
    riesgos[totalRiesgos++] = r;
}

static void cargarTXT(FILE *f) {
    char linea[TAM_LINEA];
    Riesgo actual; int hayActual = 0;
    memset(&actual, 0, sizeof(actual));

    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n")] = '\0';
        if (strncmp(linea, "===== RIESGO", 12) == 0) {
            if (hayActual) agregarRiesgoCargado(actual);
            memset(&actual, 0, sizeof(actual));
            sscanf(linea, "===== RIESGO %d =====", &actual.id);
            hayActual = 1;
        } else if (strncmp(linea, "Nombre: ", 8) == 0) {
            strncpy(actual.nombre, linea + 8, TAM_NOMBRE - 1);
        } else if (strncmp(linea, "Descripcion: ", 13) == 0) {
            strncpy(actual.descripcion, linea + 13, TAM_DESC - 1);
        } else if (strncmp(linea, "F=", 2) == 0) {
            sscanf(linea, "F=%d S=%d P=%d E=%d A=%d V=%d",
                   &actual.F, &actual.S, &actual.P, &actual.E, &actual.A, &actual.V);
        }
        /* Las lineas C=, GR= y Clasificacion= se recalculan, no se necesitan */
    }
    if (hayActual) agregarRiesgoCargado(actual);
}

static int parsearLineaCSV(char *linea, char campos[][256], int maxCampos) {
    int nc = 0, i = 0, j = 0;
    int dentroComillas = 0;
    int len = (int) strlen(linea);
    campos[0][0] = '\0';
    for (i = 0; i < len && nc < maxCampos; i++) {
        char c = linea[i];
        if (c == '"') {
            dentroComillas = !dentroComillas;
        } else if (c == ',' && !dentroComillas) {
            campos[nc][j] = '\0';
            nc++; j = 0;
            campos[nc][0] = '\0';
        } else {
            if (j < 255) campos[nc][j++] = c;
        }
    }
    campos[nc][j] = '\0';
    nc++;
    return nc;
}

static void cargarCSV(FILE *f) {
    char linea[TAM_LINEA];
    char campos[12][256];
    int primera = 1;

    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n")] = '\0';
        if (linea[0] == '\0') continue;
        if (primera) { primera = 0; continue; } /* saltar encabezado */

        if (parsearLineaCSV(linea, campos, 12) >= 12) {
            Riesgo r; memset(&r, 0, sizeof(r));
            r.id = atoi(campos[0]);
            strncpy(r.nombre, campos[1], TAM_NOMBRE - 1);
            strncpy(r.descripcion, campos[2], TAM_DESC - 1);
            r.F = atoi(campos[3]); r.S = atoi(campos[4]);
            r.P = atoi(campos[5]); r.E = atoi(campos[6]);
            r.A = atoi(campos[7]); r.V = atoi(campos[8]);
            agregarRiesgoCargado(r);
        }
    }
}

static const char *despuesDe(const char *linea, const char *clave) {
    const char *p = strstr(linea, clave);
    if (!p) return NULL;
    return p + strlen(clave);
}

static void cargarJSON(FILE *f) {
    char linea[TAM_LINEA];
    Riesgo actual; int hayActual = 0;
    const char *v;
    memset(&actual, 0, sizeof(actual));

    while (fgets(linea, sizeof(linea), f)) {
        if (strstr(linea, "{")) {
            if (hayActual) agregarRiesgoCargado(actual);
            memset(&actual, 0, sizeof(actual));
            hayActual = 1;
        } else if ((v = despuesDe(linea, "\"id\":")) != NULL) {
            actual.id = atoi(v);
        } else if ((v = despuesDe(linea, "\"nombre\":")) != NULL) {
            sscanf(v, " \"%79[^\"]\"", actual.nombre);
        } else if ((v = despuesDe(linea, "\"descripcion\":")) != NULL) {
            sscanf(v, " \"%239[^\"]\"", actual.descripcion);
        } else if ((v = despuesDe(linea, "\"F\":")) != NULL) {
            sscanf(v, "%d, \"S\": %d, \"P\": %d, \"E\": %d, \"A\": %d, \"V\": %d",
                   &actual.F, &actual.S, &actual.P, &actual.E, &actual.A, &actual.V);
        }
    }
    if (hayActual) agregarRiesgoCargado(actual);
}

static void cargarXML(FILE *f) {
    char linea[TAM_LINEA];
    Riesgo actual; int hayActual = 0;
    memset(&actual, 0, sizeof(actual));

    while (fgets(linea, sizeof(linea), f)) {
        if (strstr(linea, "<riesgo>")) {
            memset(&actual, 0, sizeof(actual));
            hayActual = 1;
        } else if (strstr(linea, "</riesgo>")) {
            if (hayActual) agregarRiesgoCargado(actual);
            hayActual = 0;
        } else if (strstr(linea, "<id>")) {
            sscanf(strstr(linea, "<id>"), "<id>%d</id>", &actual.id);
        } else if (strstr(linea, "<nombre>")) {
            sscanf(strstr(linea, "<nombre>"), "<nombre>%79[^<]", actual.nombre);
        } else if (strstr(linea, "<descripcion>")) {
            sscanf(strstr(linea, "<descripcion>"), "<descripcion>%239[^<]", actual.descripcion);
        } else if (strstr(linea, "<F>")) {
            sscanf(linea, " <F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>",
                   &actual.F, &actual.S, &actual.P, &actual.E, &actual.A, &actual.V);
        }
    }
}

static int strcasecmp_local(const char *a, const char *b);

static const char *obtenerExtension(const char *ruta) {
    const char *punto = strrchr(ruta, '.');
    return punto ? punto + 1 : "";
}

static void cargarDesdeArchivo(void) {
    FILE *f;
    const char *ext;
    int antes = totalRiesgos;

    printf("Archivo activo: %s\n", archivoActual);
    f = fopen(archivoActual, "r");
    if (!f) { printf("No se pudo abrir '%s'.\n", archivoActual); return; }

    ext = obtenerExtension(archivoActual);
    if (strcasecmp_local(ext, "csv") == 0)       cargarCSV(f);
    else if (strcasecmp_local(ext, "json") == 0) cargarJSON(f);
    else if (strcasecmp_local(ext, "xml") == 0)  cargarXML(f);
    else                                          cargarTXT(f);

    fclose(f);
    printf("Se cargaron %d riesgo(s) nuevos (memoria dinamica, capacidad actual = %d).\n",
           totalRiesgos - antes, capacidadRiesgos);
}

/* strcasecmp no es estandar en C puro / algunos compiladores Windows;
   se define una version propia y portable. */
static int strcasecmp_local(const char *a, const char *b) {
    while (*a && *b) {
        int ca = tolower((unsigned char) *a);
        int cb = tolower((unsigned char) *b);
        if (ca != cb) return ca - cb;
        a++; b++;
    }
    return tolower((unsigned char) *a) - tolower((unsigned char) *b);
}

/* ---------------------------- MODULO DE IA ---------------------------------- */

/* IA heuristica local: no requiere internet. Analiza cual de los 6 criterios
   domina la gravedad y sugiere una linea de accion, siguiendo el enfoque de
   que GR = C * A * V (si C, A o V son altos, ese es el punto a atacar). */
static void sugerenciaHeuristica(const Riesgo *r, char *buffer, int tam) {
    int max = r->F;
    char *criterioDominante = "Funcion (F)";

    if (r->S > max) { max = r->S; criterioDominante = "Sustitucion (S)"; }
    if (r->P > max) { max = r->P; criterioDominante = "Profundidad (P)"; }
    if (r->E > max) { max = r->E; criterioDominante = "Extension (E)"; }
    if (r->A > max) { max = r->A; criterioDominante = "Agresion / Probabilidad (A)"; }
    if (r->V > max) { max = r->V; criterioDominante = "Vulnerabilidad (V)"; }

    if (strcmp(r->clasificacion, "Gravisima") == 0 || strcmp(r->clasificacion, "Muy Alta") == 0) {
        snprintf(buffer, (size_t) tam,
            "Riesgo '%s' (GR=%.2f, %s). El criterio dominante es %s. "
            "Se recomienda accion inmediata: si domina A o V, priorizar controles "
            "preventivos/proteccion; si domina C (F/S/P/E), reducir la exposicion "
            "o el impacto potencial y definir un plan de contingencia documentado.",
            r->nombre, r->GR, r->clasificacion, criterioDominante);
    } else if (strcmp(r->clasificacion, "Alta") == 0 || strcmp(r->clasificacion, "Media") == 0) {
        snprintf(buffer, (size_t) tam,
            "Riesgo '%s' (GR=%.2f, %s). Criterio dominante: %s. "
            "Se sugiere establecer controles administrativos y de ingenieria, "
            "y revisar el riesgo periodicamente.",
            r->nombre, r->GR, r->clasificacion, criterioDominante);
    } else {
        snprintf(buffer, (size_t) tam,
            "Riesgo '%s' (GR=%.2f, %s). Nivel aceptable; mantener el monitoreo "
            "habitual y las medidas de control ya existentes.",
            r->nombre, r->GR, r->clasificacion);
    }
}

#ifdef USE_CLAUDE_API

struct MemoriaRespuesta { char *datos; size_t tam; };

static size_t escribirCallback(void *contenido, size_t tam, size_t nmemb, void *userp) {
    size_t total = tam * nmemb;
    struct MemoriaRespuesta *mem = (struct MemoriaRespuesta *) userp;
    char *nuevo = (char *) realloc(mem->datos, mem->tam + total + 1);
    if (!nuevo) return 0;
    mem->datos = nuevo;
    memcpy(&(mem->datos[mem->tam]), contenido, total);
    mem->tam += total;
    mem->datos[mem->tam] = '\0';
    return total;
}

/* Consulta real a la API de Claude (api.anthropic.com/v1/messages).
   Requiere -DUSE_CLAUDE_API -lcurl y la variable ANTHROPIC_API_KEY. */
static void consultarIA(const Riesgo *r) {
    CURL *curl;
    CURLcode res;
    struct MemoriaRespuesta resp = { NULL, 0 };
    char cuerpo[2048];
    const char *apiKey = getenv("ANTHROPIC_API_KEY");

    if (!apiKey) {
        printf("No se encontro ANTHROPIC_API_KEY en el entorno. Usando IA local.\n");
        { char buf[512]; sugerenciaHeuristica(r, buf, sizeof(buf)); printf("%s\n", buf); }
        return;
    }

    snprintf(cuerpo, sizeof(cuerpo),
        "{"
        "\"model\":\"claude-sonnet-4-6\","
        "\"max_tokens\":300,"
        "\"messages\":[{\"role\":\"user\",\"content\":"
        "\"Analiza este riesgo evaluado con el metodo Mosler y da una "
        "recomendacion breve en espanol. Nombre: %s. GR=%.2f (%s). "
        "F=%d S=%d P=%d E=%d A=%d V=%d.\"}]"
        "}",
        r->nombre, r->GR, r->clasificacion, r->F, r->S, r->P, r->E, r->A, r->V);

    curl = curl_easy_init();
    if (!curl) { printf("No se pudo inicializar curl.\n"); return; }

    {
        struct curl_slist *headers = NULL;
        char headerAuth[300];
        snprintf(headerAuth, sizeof(headerAuth), "x-api-key: %s", apiKey);
        headers = curl_slist_append(headers, headerAuth);
        headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
        headers = curl_slist_append(headers, "content-type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, "https://api.anthropic.com/v1/messages");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cuerpo);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, escribirCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            printf("Error al llamar a la API: %s\n", curl_easy_strerror(res));
        } else {
            printf("Respuesta de la IA:\n%s\n", resp.datos ? resp.datos : "(vacia)");
        }
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    free(resp.datos);
}

#else

static void consultarIA(const Riesgo *r) {
    char buf[512];
    sugerenciaHeuristica(r, buf, sizeof(buf));
    printf("[IA heuristica local]\n%s\n", buf);
}

#endif

static void menuIA(void) {
    int id, idx;
    mostrarListaResumen();
    if (totalRiesgos == 0) return;
    id = leerEntero("\nID del riesgo a analizar con IA: ", 0, 1000000);
    idx = obtenerIndicePorId(id);
    if (idx == -1) { printf("No existe un riesgo con ese ID.\n"); return; }
    consultarIA(&riesgos[idx]);
}

/* ---------------------------- MENU PRINCIPAL -------------------------------- */

static void mostrarMenu(void) {
    printf("\n===================== METODO MOSLER =====================\n");
    printf(" Archivo activo: %s\n", archivoActual);
    printf(" 1. Crear riesgo\n");
    printf(" 2. Modificar parametros de un riesgo\n");
    printf(" 3. Calcular (recalcular) todos los riesgos\n");
    printf(" 4. Imprimir riesgos\n");
    printf(" 5. Borrar riesgo\n");
    printf(" 6. Seleccionar archivo existente\n");
    printf(" 7. Crear archivo nuevo\n");
    printf(" 8. Cargar riesgos desde el archivo activo (memoria dinamica)\n");
    printf(" 9. Guardar riesgos (TXT / CSV / JSON / XML)\n");
    printf("10. Sugerencia de IA sobre un riesgo\n");
    printf(" 0. Salir\n");
    printf("===========================================================\n");
}

int main(void) {
    int opcion;
    do {
        mostrarMenu();
        opcion = leerEntero("Elija una opcion: ", 0, 10);
        switch (opcion) {
            case 1: crearRiesgo();          break;
            case 2: modificarRiesgo();      break;
            case 3: calcularRiesgos();      break;
            case 4: imprimirRiesgos();      break;
            case 5: borrarRiesgo();         break;
            case 6: seleccionarArchivo();   break;
            case 7: crearArchivoNuevo();    break;
            case 8: cargarDesdeArchivo();   break;
            case 9: guardarArchivo();       break;
            case 10: menuIA();               break;
            case 0: printf("Hasta luego.\n"); break;
            default: break;
        }
    } while (opcion != 0);

    free(riesgos);
    return 0;
}

#include "funcion.h"


int opcionValida(int min, int max){
    int numero;
    while(1)
    {
        if (scanf ("%d", &numero) ==1 && numero >= min && numero<=max)
            {
                while(getchar()!='\n');
                return numero;
            }else{
                printf("Error, ingrese una opcion valida: ");
                while(getchar()!='\n');
            } 
    }
}

void eliminarSaltoLinea(char *cadena){
    size_t len = strlen(cadena);
    if(len > 0 && cadena[len - 1] == '\n'){
        cadena[len - 1] = '\0';
    }
}

float leerFloatValido(float min){
    float valor;
    int valido;

    do{
        valido = scanf("%f", &valor);

        while(getchar() != '\n'); // limpia buffer

        if(valido != 1){
            printf("Error: Ingrese un numero valido: ");
        } 
        else if(valor < min){
            printf("Error: No se permiten valores negativos. Ingrese nuevamente: ");
            valido = 0;
        }

    }while(valido != 1);

    return valor;
}


// menu principal del main
int menu(){
    int opc;
    printf("----------- SISTEMA DE PREDICCION DE CONTAMINACION ---------\n");
    printf("1. Precargar datos (TXT -> DAT)\n");
    printf("2. Mostrar datos por zona y mes\n");
    printf("3. Ingresar nuevo dia y predecir\n");
    printf("4. Recomendaciones y ver predicciones anteriores\n");
    printf("5. Salir\n");
    printf("Seleccione una opcion: ");
    opc=opcionValida(1,5);
    return opc;
}
// menu secundario para elegir la zona
/* ================= MENUS ================= */

int seleccionarZona(){
    printf("\nSeleccione la zona:\n");
    printf("1. Centro\n");
    printf("2. Norte\n");
    printf("3. Sur\n");
    printf("4. Valle\n");
    printf("5. Quitumbe\n");
    printf("6. Salir\n");
    printf("Opcion: ");
    return opcionValida(1, 6);
}


int seleccionarMes(){
    printf("\nSeleccione el mes:\n");
    printf("1 - 12  (0 para salir)\n");
    printf("Opcion: ");
    int m = opcionValida(0, 12);
    return m - 1;   // si es 0 devuelve -1
}

/* ================= ARCHIVOS ================= */

void precargarDatos(){
    Zona zonas[5];
    FILE *txt = fopen("historico.txt", "r");
    FILE *dat = fopen("zonas.dat", "wb");

    if(txt == NULL || dat == NULL){
        printf("Error al abrir historico.txt o zonas.dat\n");
        return;
    }

    char linea[200];
    int z, m, d;

    for(z = 0; z < 5; z++){
        /* Leer nombre de la zona */
        do {
            if(fgets(linea, sizeof(linea), txt) == NULL){
                printf("Error leyendo nombre de zona\n");
                fclose(txt);
                fclose(dat);
                return;
            }
        } while(linea[0] == '\n' || linea[0] == '#');

        eliminarSaltoLinea(linea);
        strcpy(zonas[z].nombre, linea);

        /* Leer meses y días */
        for(m = 0; m < 12; m++){
            d = 0;
            while(d < 30){
                if(fgets(linea, sizeof(linea), txt) == NULL){
                    printf("Error leyendo datos\n");
                    fclose(txt);
                    fclose(dat);
                    return;
                }

                if(linea[0] == '\n' || linea[0] == '#')
                    continue;

                sscanf(linea, "%f %f %f %f %f",
                    &zonas[z].meses[m].dias[d].na,
                    &zonas[z].meses[m].dias[d].no2,
                    &zonas[z].meses[m].dias[d].so2,
                    &zonas[z].meses[m].dias[d].co2,
                    &zonas[z].meses[m].dias[d].pm25
                );
                d++;
            }
        }
    }

    fwrite(zonas, sizeof(Zona), 5, dat);

    fclose(txt);
    fclose(dat);

    printf("Datos precargados correctamente.\n");
}



int leerDAT(Zona *zonas){
    FILE *dat = fopen("zonas.dat", "rb");
    if(dat == NULL){
        printf("Debe precargar los datos primero.\n");
        return 0;
    }

    fread(zonas, sizeof(Zona), 5, dat);
    fclose(dat);
    return 1;
}

/* ================= MOSTRAR DATOS ================= */

void mostrarDatos(Zona *zonas){
    int z = seleccionarZona();
    if(z == 6){
        printf("Regresando al menu principal...\n");
        return;
    }
    z--;   // convertir a índice 0-4

    int m = seleccionarMes();
    if(m == -1){
        printf("Regresando al menu principal...\n");
        return;
    }

    int d;
    printf("\nZona: %s | Mes %d\n", zonas[z].nombre, m + 1);
    printf("Dia  NA  NO2  SO2  CO2  PM2.5\n");

    for(d = 0; d < 30; d++){
        printf("%2d  %.1f  %.1f  %.1f  %.1f  %.1f\n",
            d + 1,
            zonas[z].meses[m].dias[d].na,
            zonas[z].meses[m].dias[d].no2,
            zonas[z].meses[m].dias[d].so2,
            zonas[z].meses[m].dias[d].co2,
            zonas[z].meses[m].dias[d].pm25
        );
    }
}


/* ================= PREDICCION Y RECOMENDACIONES ================= */

ResultadoPrediccion prediccion(Zona *zonas){
    ResultadoPrediccion r;
    float pesos[11] = {5,5,5,6,6,7,8,10,12,14,22};
    

    int z = seleccionarZona();
    if(z == 6){  // si el usuario decide salir del menú de zona
        r.pNA = r.pNO2 = r.pSO2 = r.pCO2 = r.pPM25 = r.IC = 0;
        r.zona = -1;
        r.mes  = -1;
        r.dia  = -1;
        return r;
    }
    z--; // Ajuste a índice 0-based

    int m = seleccionarMes();
    if(m == -1){  // si el usuario decide salir del menú de mes
        r.pNA = r.pNO2 = r.pSO2 = r.pCO2 = r.pPM25 = r.IC = 0;
        r.zona = -1;
        r.mes  = -1;
        r.dia  = -1;
        return r;
    }

    // Guardamos la zona y mes seleccionados en r
    r.zona = z;
    r.mes  = m;

    Dia nuevoDia;
    printf("\nIngrese datos del nuevo dia\n");

    printf("NA: ");
    nuevoDia.na = leerFloatValido(0);

    printf("NO2: ");
    nuevoDia.no2 = leerFloatValido(0);

    printf("SO2: ");
    nuevoDia.so2 = leerFloatValido(0);

    printf("CO2: ");
    nuevoDia.co2 = leerFloatValido(0);

    printf("PM2.5: ");
    nuevoDia.pm25 = leerFloatValido(0);

    float sumaNA = 0, sumaNO2 = 0, sumaSO2 = 0, sumaCO2 = 0, sumaPM25 = 0;

    /* Últimos 10 días */
    int i;
    for(i = 0; i < 10; i++){
        sumaNA   += zonas[z].meses[m].dias[20+i].na   * pesos[i];
        sumaNO2  += zonas[z].meses[m].dias[20+i].no2  * pesos[i];
        sumaSO2  += zonas[z].meses[m].dias[20+i].so2  * pesos[i];
        sumaCO2  += zonas[z].meses[m].dias[20+i].co2  * pesos[i];
        sumaPM25 += zonas[z].meses[m].dias[20+i].pm25 * pesos[i];
    }

    /* Día ingresado */
    sumaNA   += nuevoDia.na   * pesos[10];
    sumaNO2  += nuevoDia.no2  * pesos[10];
    sumaSO2  += nuevoDia.so2  * pesos[10];
    sumaCO2  += nuevoDia.co2  * pesos[10];
    sumaPM25 += nuevoDia.pm25 * pesos[10];

    /* Promedios ponderados */
    r.pNA   = sumaNA / 100;
    r.pNO2  = sumaNO2 / 100;
    r.pSO2  = sumaSO2 / 100;
    r.pCO2  = sumaCO2 / 100;
    r.pPM25 = sumaPM25 / 100;

    /* Guardamos el día ingresado */
    r.dia = 31; // asumimos que es el nuevo día después del día 30

    /* COEFICIENTES (VALORES TEMPORALES) */
    float coefNA   = 0.2;   // REVISAR
    float coefNO2  = 0.25;  // REVISAR
    float coefSO2  = 0.2;   // REVISAR
    float coefCO2  = 0.15;  // REVISAR
    float coefPM25 = 0.2;   // REVISAR

    /* Índice de Contaminación */
    r.IC = (coefNA   * r.pNA) +
           (coefNO2  * r.pNO2) +
           (coefSO2  * r.pSO2) +
           (coefCO2  * r.pCO2) +
           (coefPM25 * r.pPM25);

    return r;
}


void mostrarRecomendaciones(ResultadoPrediccion r){
    printf("\n--- ALERTAS Y RECOMENDACIONES ---\n");

    float limiteNA   = 100;
    float limiteNO2  = 40;
    float limiteSO2  = 20;
    float limiteCO2  = 5000;
    float limitePM25 = 15;

    /* NA */
    printf("\nNA: %.2f (Limite: %.2f)\n", r.pNA, limiteNA);
    if(r.pNA > limiteNA){
        printf("Supera el limite en %.2f\n", r.pNA - limiteNA);
        printf("Recomendacion: Limitar actividades al aire libre.\n");
    }else{
        printf("Dentro del limite en %.2f\n", limiteNA - r.pNA);
    }

    /* NO2 */
    printf("\nNO2: %.2f (Limite: %.2f)\n", r.pNO2, limiteNO2);
    if(r.pNO2 > limiteNO2){
        printf("Supera el limite en %.2f\n", r.pNO2 - limiteNO2);
        printf("Recomendacion: Reducir uso de vehiculos y evitar zonas congestionadas.\n");
    }else{
        printf("Dentro del limite en %.2f\n", limiteNO2 - r.pNO2);
    }

    /* SO2 */
    printf("\nSO2: %.2f (Limite: %.2f)\n", r.pSO2, limiteSO2);
    if(r.pSO2 > limiteSO2){
        printf("Supera el limite en %.2f\n", r.pSO2 - limiteSO2);
        printf("Recomendacion: Evitar exposicion industrial prolongada.\n");
    }else{
        printf("Dentro del limite en %.2f\n", limiteSO2 - r.pSO2);
    }

    /* CO2 */
    printf("\nCO2: %.2f (Limite: %.2f)\n", r.pCO2, limiteCO2);
    if(r.pCO2 > limiteCO2){
        printf("Supera el limite en %.2f\n", r.pCO2 - limiteCO2);
        printf("Recomendacion: Mejorar ventilacion y reducir emisiones.\n");
    }else{
        printf("Dentro del limite en %.2f\n", limiteCO2 - r.pCO2);
    }

    /* PM2.5 */
    printf("\nPM2.5: %.2f (Limite: %.2f)\n", r.pPM25, limitePM25);
    if(r.pPM25 > limitePM25){
        printf("Supera el limite en %.2f\n", r.pPM25 - limitePM25);
        printf("Recomendacion: Uso de mascarilla y evitar actividad fisica.\n");
    }else{
        printf("Dentro del limite en %.2f\n", limitePM25 - r.pPM25);
    }
}

void mostrarPrediccionesPorZona(Zona *zonas, int z) {

    FILE *archivo = fopen("predicciones.dat", "rb");
    if(archivo == NULL){
        printf("No hay predicciones registradas.\n");
        return;
    }

    ResultadoPrediccion r;
    int encontrado = 0;

    printf("\nPredicciones de la zona: %s\n", zonas[z].nombre);
    printf("Mes | Dia | NA  | NO2 | SO2 | CO2 | PM2.5 | IC\n");

    while(fread(&r, sizeof(ResultadoPrediccion), 1, archivo) == 1){
        if(r.zona == z){
            printf("%3d | %3d | %.2f | %.2f | %.2f | %.2f | %.2f | %.2f\n",
                   r.mes + 1, r.dia,
                   r.pNA, r.pNO2, r.pSO2,
                   r.pCO2, r.pPM25, r.IC);
            encontrado = 1;
        }
    }

    if(!encontrado){
        printf("No hay predicciones para esta zona.\n");
    }

    fclose(archivo);
}



void guardarPrediccion(ResultadoPrediccion r) {
    FILE *archivo = fopen("predicciones.dat", "ab"); // agregar al final
    if(archivo == NULL){
        printf("Error al abrir archivo de predicciones.\n");
        return;
    }

    fwrite(&r, sizeof(ResultadoPrediccion), 1, archivo);
    fclose(archivo);
}

void mostrarPrediccion(ResultadoPrediccion r, Zona *zonas){
    if(r.zona == -1 || r.mes == -1){
        printf("No se selecciono zona o mes válido.\n");
        return;
    }

    printf("\n--- RESULTADO DE PREDICCION ---\n");
    printf("Zona: %s\n", zonas[r.zona].nombre);
    printf("Mes: %d\n", r.mes + 1);
    printf("Dia ingresado: %d\n", r.dia);
    printf("NA: %.2f\n", r.pNA);
    printf("NO2: %.2f\n", r.pNO2);
    printf("SO2: %.2f\n", r.pSO2);
    printf("CO2: %.2f\n", r.pCO2);
    printf("PM2.5: %.2f\n", r.pPM25);
    printf("Indice de Contaminacion: %.2f\n", r.IC);
}

ResultadoPrediccion leerUltimaPrediccionZona(int zona) {
    ResultadoPrediccion r;
    r.zona = -1; // indicador de que no se encontró

    FILE *archivo = fopen("predicciones.dat", "rb");
    if(!archivo) return r;

    ResultadoPrediccion temp;
    while(fread(&temp, sizeof(ResultadoPrediccion), 1, archivo) == 1) {
        if(temp.zona == zona) {
            r = temp; // guarda la última predicción encontrada
        }
    }

    fclose(archivo);
    return r;
}

void mostrarPrediccionesYRecomendaciones(Zona *zonas) {

    int z = seleccionarZona();
    if(z == 6) return;
    z--;

    FILE *archivo = fopen("predicciones.dat", "rb");
    if(archivo == NULL){
        printf("No hay predicciones registradas.\n");
        return;
    }

    ResultadoPrediccion r;
    int encontrado = 0;

    printf("\n=== PREDICCIONES Y RECOMENDACIONES ===\n");
    printf("Zona: %s\n", zonas[z].nombre);

    while(fread(&r, sizeof(ResultadoPrediccion), 1, archivo) == 1){
        if(r.zona == z){

            printf("\nMes: %d | Dia: %d\n", r.mes + 1, r.dia);
            printf("NA: %.2f  NO2: %.2f  SO2: %.2f  CO2: %.2f  PM2.5: %.2f\n",
                   r.pNA, r.pNO2, r.pSO2, r.pCO2, r.pPM25);
            printf("Indice IC: %.2f\n", r.IC);

            mostrarRecomendaciones(r);
            encontrado = 1;
        }
    }

    fclose(archivo);

    if(!encontrado){
        printf("No hay predicciones para esta zona.\n");
    }
    }

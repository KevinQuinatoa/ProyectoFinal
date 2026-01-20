#include "funcion.h"

/* ================= VALIDACIONES ================= */

int opcionValida(int min, int max){
    int numero;
    while(1){
        if(scanf("%d",&numero)==1 && numero>=min && numero<=max){
            while(getchar()!='\n');
            return numero;
        }
        printf("Error, opcion invalida: ");
        while(getchar()!='\n');
    }
}

float leerFloatValido(float min){
    float v;
    while(1){
        if(scanf("%f",&v)==1 && v>=min){
            while(getchar()!='\n');
            return v;
        }
        printf("Error, ingrese un valor valido: ");
        while(getchar()!='\n');
    }
}

void eliminarSaltoLinea(char *c){
    size_t l=strlen(c);
    if(l>0 && c[l-1]=='\n') c[l-1]='\0';
}

/* ================= MENUS ================= */

int menu(){
    printf("\n--- SISTEMA DE MONITOREO ---\n");
    printf("1. Precargar datos\n");
    printf("2. Mostrar datos\n");
    printf("3. Ingresar nuevo dia y predecir\n");
    printf("4. Recomendaciones\n");
    printf("5. Reportes\n");
    printf("6. Salir\n");
    printf("Opcion: ");
    return opcionValida(1,6);
}

int seleccionarZona(){
    printf("\n1.Centro 2.Norte 3.Sur 4.Valle 5.Quitumbe 6.Salir\n");
    return opcionValida(1,6);
}

int seleccionarMes(){
    printf("Mes (1-12, 0 salir): ");
    int m=opcionValida(0,12);
    return m-1;
}

/* ================= ARCHIVOS ================= */

void precargarDatos(){
    Zona zonas[5];
    FILE *txt=fopen("historico.txt","r");
    FILE *dat=fopen("zonas.dat","wb");
    if(!txt||!dat){ printf("Error archivos\n"); return; }

    char linea[200];
    for(int z=0;z<5;z++){
        do{ fgets(linea,200,txt);}while(linea[0]=='#'||linea[0]=='\n');
        eliminarSaltoLinea(linea);
        strcpy(zonas[z].nombre,linea);

        for(int m=0;m<12;m++){
            for(int d=0;d<30;d++){
                do{ fgets(linea,200,txt);}while(linea[0]=='#'||linea[0]=='\n');
                sscanf(linea,"%f %f %f %f %f %f %f %f",
                    &zonas[z].meses[m].dias[d].na,
                    &zonas[z].meses[m].dias[d].no2,
                    &zonas[z].meses[m].dias[d].so2,
                    &zonas[z].meses[m].dias[d].co2,
                    &zonas[z].meses[m].dias[d].pm25,
                    &zonas[z].meses[m].dias[d].temperatura,
                    &zonas[z].meses[m].dias[d].humedad,
                    &zonas[z].meses[m].dias[d].viento);
            }
        }
    }
    fwrite(zonas,sizeof(Zona),5,dat);
    fclose(txt); fclose(dat);
    printf("Datos precargados correctamente\n");
}

int leerDAT(Zona *zonas){
    FILE *f=fopen("zonas.dat","rb");
    if(!f){ printf("Debe precargar datos\n"); return 0;}
    fread(zonas,sizeof(Zona),5,f);
    fclose(f);
    return 1;
}

/* ================= MOSTRAR ================= */

void mostrarDatos(Zona *zonas){
    int z=seleccionarZona(); if(z==6) return; z--;
    int m=seleccionarMes(); if(m==-1) return;

    printf("\nDia NA NO2 SO2 CO2 PM2.5 T H V\n");
    for(int d=0;d<30;d++){
        Dia x=zonas[z].meses[m].dias[d];
        printf("%2d %.1f %.1f %.1f %.1f %.1f %.1f %.1f %.1f\n",
            d+1,x.na,x.no2,x.so2,x.co2,x.pm25,
            x.temperatura,x.humedad,x.viento);
    }
}

/* ================= PREDICCION ================= */

ResultadoPrediccion prediccion(Zona *zonas){
    ResultadoPrediccion r={0};
    int z=seleccionarZona(); if(z==6){r.zona=-1;return r;} z--;
    int m=seleccionarMes(); if(m==-1){r.zona=-1;return r;}

    r.zona=z; r.mes=m;

    Dia nuevo;
    printf("\n--- NUEVO DIA ---\n");
    printf("NA: "); nuevo.na=leerFloatValido(0);
    printf("NO2: "); nuevo.no2=leerFloatValido(0);
    printf("SO2: "); nuevo.so2=leerFloatValido(0);
    printf("CO2: "); nuevo.co2=leerFloatValido(0);
    printf("PM2.5: "); nuevo.pm25=leerFloatValido(0);
    printf("Temperatura: "); nuevo.temperatura=leerFloatValido(-50);
    printf("Humedad: "); nuevo.humedad=leerFloatValido(0);
    printf("Viento: "); nuevo.viento=leerFloatValido(0);

    r.pNA=nuevo.na;
    r.pNO2=nuevo.no2;
    r.pSO2=nuevo.so2;
    r.pCO2=nuevo.co2;
    r.pPM25=nuevo.pm25;

    float factor=1;
    if(nuevo.temperatura>30) factor*=1.1;
    if(nuevo.humedad>70) factor*=1.05;
    if(nuevo.viento<3) factor*=1.1;

    r.pNA*=factor; r.pNO2*=factor; r.pSO2*=factor;
    r.pCO2*=factor; r.pPM25*=factor;

    r.IC=0.1*r.pNA+0.3*r.pNO2+0.15*r.pSO2+0.15*r.pCO2+0.3*r.pPM25;
    r.dia=31;
    return r;
}

int hayAlerta(ResultadoPrediccion r){
    return (r.pNA   > 100 ||
            r.pNO2  > 25  ||
            r.pSO2  > 40  ||
            r.pCO2  > 4   ||
            r.pPM25 > 15);
}


void mostrarRecomendaciones(ResultadoPrediccion r){
    printf("\n--- ALERTAS Y RECOMENDACIONES ---\n");

    float limiteNA   = 100;
    float limiteNO2  = 25;
    float limiteSO2  = 40;
    float limiteCO2  = 4;
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

void generarReporteHistorico(Zona *zonas){

    int z = seleccionarZona();
    if(z == 6) return;
    z--;

    int m = seleccionarMes();
    if(m == -1) return;

    int diaR, mesR, anioR;
    obtenerFechaActual(&diaR, &mesR, &anioR);

    FILE *reporte = fopen("reporte_historico.txt", "w");
    if(!reporte){
        printf("Error al crear reporte historico.\n");
        return;
    }

    fprintf(reporte, "===== REPORTE HISTORICO DE MONITOREO =====\n");
    fprintf(reporte, "Fecha de generacion: %02d/%02d/%d\n", diaR, mesR, anioR);
    fprintf(reporte, "Zona: %s\n", zonas[z].nombre);
    fprintf(reporte, "Mes analizado: %d\n\n", m + 1);

    fprintf(reporte, "Dia\tNA\tNO2\tSO2\tCO2\tPM2.5\n");

    for(int d = 0; d < 30; d++){
        fprintf(reporte, "%2d\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
            d + 1,
            zonas[z].meses[m].dias[d].na,
            zonas[z].meses[m].dias[d].no2,
            zonas[z].meses[m].dias[d].so2,
            zonas[z].meses[m].dias[d].co2,
            zonas[z].meses[m].dias[d].pm25
        );
    }

    fclose(reporte);
    printf("Reporte historico generado correctamente.\n");
}


void generarReporteActual(Zona *zonas){

    int diaR, mesR, anioR;
    obtenerFechaActual(&diaR, &mesR, &anioR);

    FILE *archivo = fopen("predicciones.dat", "rb");
    FILE *reporte = fopen("reporte_actual.txt", "w");

    if(!archivo || !reporte){
        printf("Error al generar reporte actual.\n");
        return;
    }

    fprintf(reporte, "===== REPORTE ACTUAL DE MONITOREO =====\n");
    fprintf(reporte, "Fecha de generacion: %02d/%02d/%d\n\n", diaR, mesR, anioR);

    ResultadoPrediccion r;

    while(fread(&r, sizeof(ResultadoPrediccion), 1, archivo) == 1){

        fprintf(reporte, "Zona: %s\n", zonas[r.zona].nombre);
        fprintf(reporte, "Mes: %d | Dia: %d\n", r.mes + 1, r.dia);

        fprintf(reporte, "NA: %.2f\n", r.pNA);
        fprintf(reporte, "NO2: %.2f\n", r.pNO2);
        fprintf(reporte, "SO2: %.2f\n", r.pSO2);
        fprintf(reporte, "CO2: %.2f\n", r.pCO2);
        fprintf(reporte, "PM2.5: %.2f\n", r.pPM25);
        fprintf(reporte, "Indice IC: %.2f\n", r.IC);
        fprintf(reporte, "------------------------------------\n");
    }

    fclose(archivo);
    fclose(reporte);

    printf("Reporte actual generado correctamente.\n");
}


void obtenerFechaActual(int *dia, int *mes, int *anio){
    time_t t = time(NULL);
    struct tm fecha = *localtime(&t);

    *dia = fecha.tm_mday;
    *mes = fecha.tm_mon + 1;
    *anio = fecha.tm_year + 1900;
}


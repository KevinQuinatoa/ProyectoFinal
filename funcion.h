#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    float na;
    float no2;
    float so2;
    float co2;
    float pm25;
} Dia;

typedef struct {
    Dia dias[30];
} Mes;

typedef struct {
    char nombre[20];
    Mes meses[12];
} Zona;

typedef struct {
    int zona;
    int mes;
    int dia;
    float pNA, pNO2, pSO2, pCO2, pPM25;
    float IC;
} ResultadoPrediccion;

void eliminarSaltoLinea(char *cadena);
int opcionValida(int min, int max);
float leerFloatValido(float min);
int menu();
int seleccionarZona();
int seleccionarMes();
void mostrarDatos(Zona *zonas);
ResultadoPrediccion prediccion(Zona *zonas);
void precargarDatos();
int leerDAT(Zona *zonas);
void mostrarDatos(Zona *zonas);
void mostrarRecomendaciones(ResultadoPrediccion r);
void mostrarPrediccionesPorZona(Zona *zonas, int z);
void guardarPrediccion(ResultadoPrediccion r);
void mostrarPrediccion(ResultadoPrediccion r, Zona *zonas);
ResultadoPrediccion leerUltimaPrediccionZona(int zona);
void mostrarPrediccionesYRecomendaciones(Zona *zonas);
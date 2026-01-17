#include <stdio.h>
#include "funcion.h"

int main(int argc, char const *argv[])
{
    Zona zonas[5];
    ResultadoPrediccion r;
    int opc;

    do
    {
        opc=menu();
        switch (opc)
        {
        case 1:
            precargarDatos();
            break;
        case 2:
            if(leerDAT(zonas)){
            mostrarDatos(zonas);
            }
            break;
        case 3:
             if (leerDAT(zonas)) {
                    r = prediccion(zonas);          // Calcula predicción
                    guardarPrediccion(r);           // Guarda en historial
                    mostrarPrediccion(r, zonas);    // Muestra resultado
                }
                break;
       case 4:  // Recomendaciones de la última predicción
        if (leerDAT(zonas)) {

            int z = seleccionarZona();
            if(z == 6) break;  // Salir al menú
            z--; // índice 0-4

            ResultadoPrediccion r = leerUltimaPrediccionZona(z);

            if(r.zona == -1) {
                printf("No hay predicciones registradas para esta zona.\n");
            } else {
                mostrarRecomendaciones(r);  // muestra alertas y recomendaciones
            }

            // Aquí NO llamamos a mostrarPrediccionesPorZona()
            // Se regresa directamente al menú
        }
        break;

        case 5:
            printf("Saliendo del programa.......\n");
            return 0;
        default:
            printf("Opcion no valida\n");
            break;
        }
        
        } while (opc != 5);
    
    return 0;

}

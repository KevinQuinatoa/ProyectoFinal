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
            r = prediccion(zonas);
            if(r.zona != -1){
                guardarPrediccion(r);
                mostrarPrediccion(r, zonas);
            } else {
                printf("Prediccion cancelada.\n");
            }
        }
        break;
       case 4:  // Recomendaciones de la última predicción
        if(leerDAT(zonas)){
        mostrarPrediccionesYRecomendaciones(zonas);
        }
        break;

        case 5:
            if(leerDAT(zonas)){
                generarReporteHistorico(zonas);
                generarReporteActual(zonas);
            }
            break;
        case 6:
            printf("Saliendo del programa.......\n");
            return 0;
        default:
            printf("Opcion no valida\n");
            break;
        }
        
        } while (opc != 6);
    
    return 0;

}

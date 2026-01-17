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
            r = prediccion(zonas);
            mostrarPrediccion(r, zonas);
            break;
        case 4:
            r = prediccion(zonas);

            mostrarRecomendaciones(r);
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

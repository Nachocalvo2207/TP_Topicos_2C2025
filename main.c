#include "juego.h"
#include "setup.h"
#include "eventos.h"
#include "logica.h"
#include "graficos.h"
#include "estadisticas.h"

/// Apellido, nombre: Calvo, Ignacio
/// DNI: 41162300
/// Entrega: Si

/// Apellido, nombre: Benito, Pablo
/// DNI: 43569575
/// Entrega: Si

/// Apellido, nombre: Cardoso, Christopfer
/// DNI: 38511608
/// Entrega: Si

int main(int argc, char* argv[])
{
    tJuego juego;
    bool corriendo = true;
    srand(time(NULL));

    if (sdl_Iniciar(&juego))
    {
        limpieza_juego(&juego, ERROR_SALIDA);
    }
    if (crearTexto(&juego))
    {
        limpieza_juego(&juego, ERROR_SALIDA);
    }
    inicializarConfiguracion(&juego);
    inicializarColores(&juego);
    cargar_estadisticas(&juego);

    reiniciarJuego(&juego);


    while (corriendo)
    {

        manejarEventos(&juego, &corriendo);
        actualizarJuego(&juego);

        switch (juego.estado_juego)
        {
        case INICIO:
            mostrarPantallaPresentacion(&juego);
            break;
        case MENU_CONFIG:
            mostrarMenuConfiguracion(&juego);
            break;
        case PIDIENDO_NOMBRE:
            pedirNombreJugador(&juego);
            break;
        case SECUENCIA:
        case JUGANDO:
        case MODO_DESAFIO:
            dibujar_juego(&juego);
            break;
        case VICTORIA:
            mostrarPantallaVictoria(&juego);
            break;
        case MOSTRANDO_ERROR:
            dibujar_juego(&juego);
            break;
        case ERROR_MOZART:
            mostrarPantallaErrorMozart(&juego);
            break;
        case AYUDA:
            mostrarPantallaAyuda(&juego);
            break;
        case FINALIZADO:
            mostrar_estadisticas(&juego);
            break;
        }

        SDL_Delay(16);
    }

    limpieza_juego(&juego, OK_SALIDA);
    return 0;
}


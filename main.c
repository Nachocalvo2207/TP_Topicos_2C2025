#include "TP_Topicos.h"

/// Apellido, nombre: Calvo, Ignacio
/// DNI: 41162300
/// Entrega: Si

/// Apellido, nombre: Benito, Pablo
/// DNI: 43569575
/// Entrega: Si

/// Apellido, nombre: Calvo, Ignacio
/// DNI: 41162300
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

    reiniciarJuego(&juego); // Estado inicial es INICIO

    // BUCLE PRINCIPAL DEL JUEGO
    while (corriendo)
    {
        // 1. Manejamos los eventos (esto cambia el estado)
        manejarEventos(&juego, &corriendo);

        // 2. Actualizamos la lógica del juego (esto solo hace algo en el estado SECUENCIA)
        actualizarJuego(&juego);

        // 3. Decidimos QUÉ dibujar basado en el estado actual
        switch (juego.estado_juego)
        {
        case INICIO:
            mostrarPantallaPresentacion(&juego);
            break;
        case MENU_CONFIG:
            mostrarMenuConfiguracion(&juego);
            break;
        case PIDIENDO_NOMBRE:
            pedirNombreJugador(&juego, &corriendo);
            break;
        case SECUENCIA:
        case JUGANDO:
            dibujar_juego(&juego); // Esta función dibuja el tablero
            break;
        case FINALIZADO:
            fflush(stdout);
            mostrar_estadisticas(&juego);
            break;
        }
    }

    limpieza_juego(&juego, OK_SALIDA);
    return 0;

}


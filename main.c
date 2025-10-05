#include "TP_Topicos.h"

int main(int argc, char* argv[])
{
    ///SDL_init( SDL_INIT_EVERYTHING );

    ///Genero un numero aleatorio
    srand(time(NULL));

    tJuego juego;
    reiniciarJuego(&juego);

    juego.config.num_botones = 4;           // puede cambiarse en menú
    juego.config.duracion_inicial_ms = 2000; // consigna: como mínimo 2000ms
    juego.config.modo = MODO_SCHONBERG;
    juego.config.ruta_melodia[0] = '\0';

    // estadísticas
    juego.partidas_jugadas = 0;
    juego.partidas_ganadas = 0;
    juego.partidas_perdidas = 0;
    juego.nombre_jugador[0] = '\0';

    if(sdl_Iniciar(&juego))
    {
        limpieza_juego(&juego,ERROR_SALIDA);
        printf("Todo mal!\n");
        getchar();
        exit(1);
    }

    if(crearTexto(&juego))
    {
        limpieza_juego(&juego,ERROR_SALIDA);
        printf("No se pudo crear el texto!\n");
        getchar();
        exit(1);
    }

    bool corriendo = true;
    // Genera primera secuencia
    juego.estado_juego = INICIO;

    ///Permite que se genere la imagen inicial
    while(corriendo)
    {

        manejarEventos(&juego, &corriendo);

        if(juego.estado_juego == INICIO)
        {
            mostrarPantallaPresentacion(&juego);
            SDL_Delay(16);
            continue;
        }

        // Si pedimos nombre
        if(juego.estado_juego == PIDIENDO_NOMBRE)
        {
            pedirNombreJugador(&juego, &corriendo);
            SDL_Delay(16);
            continue;
        }

        ///Logica del juego
        actualizarJuego(&juego);

        ///Imagen pantalla
        dibujar_juego(&juego);

        SDL_Delay(16); ///5 segs
    }

    limpieza_juego(&juego,OK_SALIDA);
    printf("Todo correcto!\n");
    getchar();
    return 0;
    
}


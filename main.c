#include "TP_Topicos.h"

int main(int argc, char* argv[])
{
    ///SDL_init( SDL_INIT_EVERYTHING );

    ///Genero un numero aleatorio
    srand(time(NULL));

    tJuego juego;
    reiniciarJuego(&juego);

    if(sdl_Iniciar(&juego))
    {
        limpieza_juego(&juego,ERROR_SALIDA);
        printf("Todo mal!\n");

        exit(1);
    }

    if(crearTexto(&juego))
    {
        limpieza_juego(&juego,ERROR_SALIDA);
        printf("No se pudo crear el texto!\n");
        exit(1);
    }

    bool corriendo = true;
    // Genera primera secuencia


    ///Permite que se genere la imagen inicial
    while(corriendo)
    {

        manejarEventos(&juego, &corriendo);

        ///Logica del juego
        actualizarJuego(&juego);

        ///Imagen pantalla
        dibujar_juego(&juego);

        SDL_Delay(16); ///5 segs
    }

    limpieza_juego(&juego,OK_SALIDA);
    printf("Todo correcto!\n");
    return 0;


}

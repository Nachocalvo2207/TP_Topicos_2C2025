#include "TP_Topicos.h"

int main(int argc, char* argv[])
{
    ///SDL_init( SDL_INIT_EVERYTHING );

    tJuego juego;

    if(sdl_Iniciar(&juego))
    {
        limpieza_juego(&juego,ERROR_SALIDA);
        printf("Todo mal!\n");

        exit(1);
    }

    dibujarTablero(&juego);

    int secuencia[MAX_SEQ];
    int longitud = 1;

    ///Permite que se genere la imagen inicial
    while(true)
    {
    SDL_Event event;
    ///Agarro todos los eventos que pasen en el loop
    while(SDL_PollEvent(&event))
    {
        switch(event.type){
            case SDL_QUIT:
                limpieza_juego(&juego,OK_SALIDA);
                break;
            ///PARA SALIR APRETANDO ESC
            case SDL_KEYDOWN:
                switch(event.key.keysym.scancode)
                {
                case SDL_SCANCODE_ESCAPE:
                    limpieza_juego(&juego,OK_SALIDA);
                default:
                    break;
                }

            default:
                break;
        }
    }
    SDL_RenderClear(juego.renderizar);
    SDL_RenderPresent(juego.renderizar);
    SDL_Delay(16); ///5 segs
    }

    limpieza_juego(&juego,OK_SALIDA);
    printf("Todo correcto!\n");
    return 0;


}

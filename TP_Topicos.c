#include "TP_Topicos.h"

// --- NUEVA FUNCIÓN ---
bool sdl_Iniciar(tJuego *juego)
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
        return true;
    ;

    ///CREA LA VENTANA QUE DA LA IMAGEN
    juego->ventana = SDL_CreateWindow
                        (
                            TITULO
                            ,SDL_WINDOWPOS_CENTERED
                            ,SDL_WINDOWPOS_CENTERED
                            ,PIXELES_HORIZONTALES
                            ,PIXELES_VERTICALES
                            ,0
                        );

    if(!juego->ventana)
    {
        fprintf(stderr,"ERROR CREANDO VENTANA: %s \n",SDL_GetError());
        return true;
    }

    ///
    juego->renderizar = SDL_CreateRenderer(juego->ventana,-1,0);

    if(!juego->renderizar)
    {
        fprintf(stderr,"ERROR CREANDO RENDERIZADO: %s \n",SDL_GetError());
        return true;
    }

    return false; ///FALSE ES NUESTRO CASO DE EXITO EN ESTE CASO
}

void limpieza_juego(tJuego *juego, int Estatus_Salida)
{
    SDL_DestroyRenderer(juego->renderizar);
    SDL_DestroyWindow(juego->ventana);
    SDL_Quit();
    exit(Estatus_Salida);

}

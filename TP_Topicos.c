#include "TP_Topicos.h"

int secuencia[MAX_SEQ];
int nivelActual = 0;

// --- NUEVA FUNCIÓN ---
bool sdl_Iniciar(tJuego *juego)
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
        return false;
    

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
        return false;
    }

    ///
    juego->renderizar = SDL_CreateRenderer(juego->ventana,-1,0);

    if(!juego->renderizar)
    {
        fprintf(stderr,"ERROR CREANDO RENDERIZADO: %s \n",SDL_GetError());
        return false;
    }

    return true; /// CASO DE EXITO
}

void limpieza_juego(tJuego *juego, int Estatus_Salida)
{
    SDL_DestroyRenderer(juego->renderizar);
    SDL_DestroyWindow(juego->ventana);
    SDL_Quit();
    exit(Estatus_Salida);

}

// ----- SIMON -----

void dibujarTablero(tJuego *juego){
    SDL_Rect r;

    // VERDE arriba izq
    r.x = 0; r.y = 0; r.w = PIXELES_HORIZONTALES/2; r.h = PIXELES_VERTICALES/2;
    SDL_SetRenderDrawColor(juego->renderizar, 0,200,0,255);
    SDL_RenderFillRect(juego->renderizar,&r);

    // ROJO arriba der
    r.x = PIXELES_HORIZONTALES/2; r.y = 0;
    SDL_SetRenderDrawColor(juego->renderizar, 200,0,0,255);
    SDL_RenderFillRect(juego->renderizar,&r);

    // AMARILLO abajo izq
    r.x = 0; r.y = PIXELES_VERTICALES/2;
    SDL_SetRenderDrawColor(juego->renderizar, 200,200,0,255);
    SDL_RenderFillRect(juego->renderizar,&r);

    // AZUL abajo der
    r.x = PIXELES_HORIZONTALES/2; r.y = PIXELES_VERTICALES/2;
    SDL_SetRenderDrawColor(juego->renderizar, 0,0,200,255);
    SDL_RenderFillRect(juego->renderizar,&r);

    SDL_RenderPresent(juego->renderizar);
}

int detectarBotonClick(int x, int y){
    bool top = (y < PIXELES_VERTICALES/2);
    bool left = (x < PIXELES_HORIZONTALES/2);
    if(top && left) return VERDE;
    if(top && !left) return ROJO;
    if(!top && left) return AMARILLO;
    return AZUL;
}

void generarSecuencia() {
    for(int i = 0; i < MAX_SEQ; i++) {
        secuencia[i] = rand() % 4; // VERDE=0, ROJO=1, AMARILLO=2, AZUL=3
    }
}

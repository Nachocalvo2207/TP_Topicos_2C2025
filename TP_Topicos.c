#include "TP_Topicos.h"


bool sdl_Iniciar(tJuego *juego)
{
    if(SDL_Init(SDL_INIT_EVERYTHING))
        return true;
    ;

    if(TTF_Init())
    {
        fprintf(stderr,"Error iniciando SDL_ttf: %s\n",TTF_GetError());
        return true;
    }

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

void reiniciarJuego(tJuego *juego)
{
    juego->estado_juego = INICIO;

    juego->nivel_actual = 1;
    juego->paso_actual_jugador = 0;
    juego->paso_secuencia = 0;
    juego->color_iluminado = SIN_COLOR; // -1 = Ningún color iluminado
    juego->tiempo_ultimo_cambio = 0;
}


bool crearTexto(tJuego *juego)
{
    ///Texto blanco
    juego->texto_color.r = 255;
    juego->texto_color.g = 255;
    juego->texto_color.b = 255;
    juego->texto_color.a = 255;

    juego->texto_fuente = TTF_OpenFont("fonts/freesansbold.ttf",TEXT_SIZE);
    if(!juego->texto_fuente)
    {
        fprintf(stderr,"ERROR CREANDO LA FUENTE: %s\n",TTF_GetError());
        return true;
    }

    SDL_Surface *superficie = TTF_RenderText_Blended(juego->texto_fuente,"VA QUERIENDO",juego->texto_color);

    if(!superficie)
    {
        juego->texto_fuente = TTF_OpenFont("fonts/freesansbold.ttf",TEXT_SIZE);
        fprintf(stderr,"ERROR CREANDO SUPERFICIE: %s\n",SDL_GetError());
    }


    juego->texto_rect.w = superficie->w; ///Ancho texto
    juego->texto_rect.h = superficie->h; ///Altura texto
    juego->texto_rect.x = (PIXELES_HORIZONTALES - juego->texto_rect.w) / 2;
    juego->texto_rect.y = (PIXELES_VERTICALES - juego->texto_rect.h) / 2;
    //

    juego->textura_imagen = SDL_CreateTextureFromSurface(juego->renderizar,superficie);
    SDL_FreeSurface(superficie);
    if(!juego->texto_fuente)
    {
        fprintf(stderr,"ERROR CREANDO LA TEXTURA: %s\n",SDL_GetError());
        return true;
    }


    return false;
}

const char* traducirColor(int color)
{
    switch (color)
    {
    case VERDE:
        return "VERDE";
    case ROJO:
        return "ROJO";
    case AMARILLO:
        return "AMARILLO";
    case AZUL:
        return "AZUL";
    default:
        return "DESCONOCIDO";
    }
}
// VERSIÓN CORREGIDA
void manejarEventos(tJuego *juego, bool *corriendo)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_QUIT:
            *corriendo = false;
            break;

        case SDL_KEYDOWN:
            switch(event.key.keysym.scancode)
            {
            case SDL_SCANCODE_ESCAPE:
                *corriendo = false;
                break;

            case SDL_SCANCODE_SPACE:
                /// Si arrancamos o perdimos
                if (juego->estado_juego == INICIO || juego->estado_juego == FINALIZADO)
                {
                    reiniciarJuego(juego);
                    agregar_nuevo_color_secuencia(juego); ///Genero el primer color
                    juego->estado_juego = SECUENCIA;      /// Modifico el estado del juego
                    printf("Juego iniciado. Nivel 1. Secuencia: %s\n", traducirColor(juego->secuencia[0]));
                }
                break;
            default:
                break;
            }
            break;

        /// Acciones que realizamos con el mouse
        case SDL_MOUSEBUTTONDOWN:
        {
            if(juego->estado_juego == JUGANDO)
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                int color_clickeado = detectarBotonClick(mouseX, mouseY);
                printf("\nclick color: %s\n", traducirColor(color_clickeado));

                if(color_clickeado == juego->secuencia[juego->paso_actual_jugador])
                {
                    juego->paso_actual_jugador++;

                    if(juego->paso_actual_jugador >= juego->nivel_actual)
                    {
                        juego->nivel_actual++;
                        agregar_nuevo_color_secuencia(juego); //
                        juego->estado_juego = SECUENCIA;
                        juego->paso_actual_jugador = 0;
                        // Vemos el nuevo color que se añadió para el siguiente nivel.
                        printf("Nivel completado. Nuevo color agregado: %s\n", traducirColor(juego->secuencia[juego->nivel_actual - 1]));

                        // secuencia completa
                        printf("Secuencia completa para Nivel %d: ", juego->nivel_actual);
                        for (int i = 0; i < juego->nivel_actual; i++)
                        {
                            printf("%s ", traducirColor(juego->secuencia[i]));
                        }

                    }
                }
                else
                {
                    juego->estado_juego = FINALIZADO;
                }
            }
            break;
        }

        }
    }
}


// ----- SIMON -----

///Divido el tablero en 4 partes iguales de distintos colores
void dibujarTablero(tJuego *juego)
{
    SDL_Rect r;
    r.w = PIXELES_HORIZONTALES / 2;
    r.h = PIXELES_VERTICALES / 2;

    ///VERDE (arriba izq) ---
    r.x = 0;
    r.y = 0;
    if (juego->color_iluminado == VERDE)
    {
        SDL_SetRenderDrawColor(juego->renderizar, 0, 255, 0, 255); // Color brillante
    }
    else
    {
        SDL_SetRenderDrawColor(juego->renderizar, 0, 180, 0, 255); // Color normal
    }
    SDL_RenderFillRect(juego->renderizar, &r);

    ///ROJO (arriba der)
    r.x = PIXELES_HORIZONTALES / 2;
    r.y = 0;
    if (juego->color_iluminado == ROJO)
    {
        SDL_SetRenderDrawColor(juego->renderizar, 255, 0, 0, 255); // Brillante
    }
    else
    {
        SDL_SetRenderDrawColor(juego->renderizar, 150, 0, 0, 255); // Normal
    }
    SDL_RenderFillRect(juego->renderizar, &r);

    ///DIBUJA AMARILLO (abajo izq)
    r.x = 0;
    r.y = PIXELES_VERTICALES / 2;
    if (juego->color_iluminado == AMARILLO)
    {
        SDL_SetRenderDrawColor(juego->renderizar, 255, 255, 0, 255); // Brillante
    }
    else
    {
        SDL_SetRenderDrawColor(juego->renderizar, 180, 180, 0, 255); // Normal
    }
    SDL_RenderFillRect(juego->renderizar, &r);

    ///DIBUJA AZUL (abajo der)
    r.x = PIXELES_HORIZONTALES / 2;
    r.y = PIXELES_VERTICALES / 2;
    if (juego->color_iluminado == AZUL)
    {
        SDL_SetRenderDrawColor(juego->renderizar, 50, 50, 255, 255); // Brillante
    }
    else
    {
        SDL_SetRenderDrawColor(juego->renderizar, 0, 0, 150, 255); // Normal
    }
    SDL_RenderFillRect(juego->renderizar, &r);
}


void dibujar_juego(tJuego *juego)
{

    SDL_RenderClear(juego->renderizar);
    //Dibujar el fondo/tablero
    dibujarTablero(juego);
    //Dibujar el texto encima del tablero
    SDL_RenderCopy(juego->renderizar, juego->textura_imagen, NULL, &juego->texto_rect);
    //Mostrar todo en la pantalla
    SDL_RenderPresent(juego->renderizar);
}


int detectarBotonClick(int x, int y)
{
    bool top = (y < PIXELES_VERTICALES/2);
    bool left = (x < PIXELES_HORIZONTALES/2);
    if(top && left) return VERDE;
    if(top && !left) return ROJO;
    if(!top && left) return AMARILLO;
    return AZUL;
}


void agregar_nuevo_color_secuencia(tJuego *juego)
{
    ///ACA HAY QUE PONER UN REALLOC PARA MAX_SEQ, que vaya aumentando la memoria a medida que pasa el tiempo
    int indice = juego->nivel_actual - 1;
    if (indice < MAX_SEQ)
    {
        juego->secuencia[indice] = rand() % 4;
    }
}




/// REALIZO LA SECUENCIA DE COLORES
void actualizarJuego(tJuego *juego)
{
    ///Chequeo que sea el momento de mostrar la secuencia
    if (juego->estado_juego != SECUENCIA)
    {
        return;
    }

    /// Obtenemos el tiempo actual
    Uint32 tiempoActual = SDL_GetTicks();

    /// Medio segundo
    if (tiempoActual > juego->tiempo_ultimo_cambio + 500)
    {

        juego->tiempo_ultimo_cambio = tiempoActual;

        if (juego->color_iluminado == -1)
        {

            if (juego->paso_secuencia < juego->nivel_actual)
            {

                juego->color_iluminado = juego->secuencia[juego->paso_secuencia];
            }
            else
            {
                juego->estado_juego = JUGANDO;
                juego->paso_secuencia = 0;
                juego->color_iluminado = -1;
            }
        }
        else
        {

            juego->color_iluminado = -1;
            juego->paso_secuencia++;
        }
    }
}


void limpieza_juego(tJuego *juego, int Estatus_Salida)
{
    SDL_DestroyTexture(juego->textura_imagen);
    TTF_CloseFont(juego->texto_fuente);
    SDL_DestroyRenderer(juego->renderizar);
    SDL_DestroyWindow(juego->ventana);
    TTF_Quit();
    SDL_Quit();
    exit(Estatus_Salida);
}

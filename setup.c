#include "setup.h"


bool sdl_Iniciar(tJuego *juego)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        fprintf(stderr,"Error iniciando SDL: %s\n", SDL_GetError());
        return true;
    }

    if(TTF_Init())
    {
        fprintf(stderr,"Error iniciando SDL_ttf: %s\n",TTF_GetError());
        return true;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr,"Error iniciando SDL_mixer: %s\n",Mix_GetError());
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

    ///Cargo cant. niveles iniciales con mem.dinamica:
    juego->capacidad_secuencia = MAX_SEQ;
    juego->secuencia = malloc(juego->capacidad_secuencia * sizeof(int));
    if(!juego->secuencia)
    {
        fprintf(stderr, "Error: No se pudo reservar memoria para la secuencia.\n");
        return true;
    }



    ///Carga de sonidos de los colores
    char *basePath = SDL_GetBasePath();
    char fullPath[256];

    const char* archivos[CANT_BOTONES] = {SND_VERDE,SND_ROJO,SND_AMARILLO,SND_AZUL, SND_NARANJA, SND_ROSA, SND_VIOLETA, SND_AQUAMARINO};

    for(int i=0; i<CANT_BOTONES; i++)

    {
        snprintf(fullPath, sizeof(fullPath), "%s%s", basePath, archivos[i]);
        juego->sonidos[i] = Mix_LoadWAV(fullPath);
        if(!juego->sonidos[i])
        {
            fprintf(stderr,"Error cargando %s: %s\n", fullPath, Mix_GetError());
        }
    }
    SDL_free(basePath);

    ///Carga sonido error
    char errorPath[256];
    basePath = SDL_GetBasePath();
    snprintf(errorPath, sizeof(errorPath), "%s%s", basePath, SND_ERROR);
    juego->sonido_error = Mix_LoadWAV(errorPath);
    if (!juego->sonido_error)
    {
        fprintf(stderr, "Error cargando %s: %s\n", errorPath, Mix_GetError());
    }
    SDL_free(basePath);

    return false; ///FALSE ES NUESTRO CASO DE EXITO EN ESTE CASO
}


void inicializarConfiguracion(tJuego *juego)
{
    juego->config.num_botones = 4;
    juego->config.duracion_inicial_ms = DURACION_INICIAL;
    juego->config.modo = MODO_SCHONBERG;
    strcpy(juego->config.ruta_melodia, RUTA_MOZART);
}


void inicializarColores(tJuego *juego)
{
    tColorData colores_iniciales[8] =
    {
        // [0] VERDE
        {70, 230, 39, 31, 102, 17},
        // [1] ROJO
        {230, 39, 71, 115, 20, 35},
        // [2] AMARILLO
        {237, 222, 17, 166, 156, 15},
        // [3] AZUL
        {20, 85, 217, 10, 42, 105},
        // [4] NARANJA
        {227, 117, 27, 148, 74, 18},
        // [5] ROSA
        {214, 19, 130, 135, 14, 82},
        // [6] VIOLETA
        {165, 18, 219, 98, 12, 130},
        // [7] AQUAMARINO
        {17, 217, 160, 12, 133, 98}
    };

    memcpy(juego->lista_colores, colores_iniciales, sizeof(tColorData) * 8);
}

bool crearTexto(tJuego *juego)
{
    ///Texto blanco
    juego->texto_color = (SDL_Color)
    {
        255, 255, 255, 255
    };

    juego->texto_fuente = TTF_OpenFont(RUTA_FUENTE,TEXT_SIZE);
    if(!juego->texto_fuente)
    {
        fprintf(stderr,"ERROR CREANDO LA FUENTE: %s\n",TTF_GetError());
        return true;
    }

    juego->texto_config = TTF_OpenFont(RUTA_FUENTE,TEXT_CONFIG_SIZE);
    if(!juego->texto_config)
    {
        fprintf(stderr,"ERROR CREANDO LA FUENTE DE LA CONFIG: %s\n",TTF_GetError());
        return true;
    }

    juego->texto_ayuda_fuente = TTF_OpenFont(RUTA_FUENTE, TEXT_AYUDA_SIZE); // Tamaño más chico
    if(!juego->texto_ayuda_fuente)
    {
        fprintf(stderr,"ERROR CREANDO LA FUENTE DE AYUDA: %s\n",TTF_GetError());
        return true;
    }

    juego->texto_boton_control = TTF_OpenFont(RUTA_FUENTE, TEXT_BOTON_CONTROL_SIZE);
    if (!juego->texto_boton_control)
    {
        fprintf(stderr, "ERROR CREANDO LA FUENTE DE BOTONES DE CONTROL: %s\n", TTF_GetError());
        return true;
    }
    SDL_Surface *superficie = TTF_RenderText_Blended(juego->texto_fuente,"SIMON",juego->texto_color);

    if(!superficie)
    {
        juego->texto_fuente = TTF_OpenFont(RUTA_FUENTE,TEXT_SIZE);
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

void limpieza_juego(tJuego *juego, int Estatus_Salida)
{
    for(int i=0; i<7; i++)
    {
        if(juego->sonidos[i])
        {
            Mix_FreeChunk(juego->sonidos[i]);
            juego->sonidos[i] = NULL;
        }
    }
    if(juego->sonido_error)
        Mix_FreeChunk(juego->sonido_error);
    Mix_CloseAudio();
    SDL_DestroyTexture(juego->textura_imagen);
    TTF_CloseFont(juego->texto_fuente);
    TTF_CloseFont(juego->texto_config);
    TTF_CloseFont(juego->texto_ayuda_fuente);
    if (juego->texto_boton_control)
        TTF_CloseFont(juego->texto_boton_control);
    SDL_DestroyRenderer(juego->renderizar);
    SDL_DestroyWindow(juego->ventana);
    TTF_Quit();
    SDL_Quit();
    exit(Estatus_Salida);
}

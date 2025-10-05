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

    SDL_Surface *superficie = TTF_RenderText_Blended(juego->texto_fuente,"SIMON",juego->texto_color);

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

void mostrarPantallaPresentacion(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    // Fondo negro
    SDL_SetRenderDrawColor(juego->renderizar, 10,10,10,255);
    SDL_Rect full = {0,0,PIXELES_HORIZONTALES,PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &full);

    // Texto grande (usamos la textura creada en crearTexto como ejemplo)
    if(juego->textura_imagen)
    {
        SDL_Rect dest = juego->texto_rect;
        // Ajustar posición hacia arriba
        dest.y = 80;
        SDL_RenderCopy(juego->renderizar, juego->textura_imagen, NULL, &dest);
    }

    // Mensaje secundario
    char buf[128];
    snprintf(buf, sizeof(buf), "Presione SPACE para comenzar | M = Menu config | Esc = Salir");
    SDL_Surface *surf = TTF_RenderText_Blended(juego->texto_fuente, buf, juego->texto_color);
    SDL_Texture *tex = SDL_CreateTextureFromSurface(juego->renderizar, surf);
    SDL_Rect r2 = {20, PIXELES_VERTICALES - 60, surf->w, surf->h};
    SDL_FreeSurface(surf);
    SDL_RenderCopy(juego->renderizar, tex, NULL, &r2);
    SDL_DestroyTexture(tex);

    SDL_RenderPresent(juego->renderizar);
}

// --- Menú de configuración gráfico (mínimo) ---
void mostrarMenuConfiguracion(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    // Fondo
    SDL_SetRenderDrawColor(juego->renderizar, 30,30,40,255);
    SDL_Rect full = {0,0,PIXELES_HORIZONTALES,PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &full);

    // Título
    char title[64];
    snprintf(title,sizeof(title),"MENU CONFIGURACION");
    SDL_Surface *sTitle = TTF_RenderText_Blended(juego->texto_fuente, title, juego->texto_color);
    SDL_Texture *tTitle = SDL_CreateTextureFromSurface(juego->renderizar, sTitle);
    SDL_Rect rTitle = {20,20,sTitle->w,sTitle->h};
    SDL_RenderCopy(juego->renderizar, tTitle, NULL, &rTitle);
    SDL_FreeSurface(sTitle);
    SDL_DestroyTexture(tTitle);

    // Opciones: num botones y duración inicial
    char opt1[80];
    snprintf(opt1,sizeof(opt1),"Botones (3-8): %d   (teclas 3..8 para cambiar)", juego->config.num_botones);
    SDL_Surface *s1 = TTF_RenderText_Blended(juego->texto_fuente, opt1, juego->texto_color);
    SDL_Texture *t1 = SDL_CreateTextureFromSurface(juego->renderizar, s1);
    SDL_Rect r1 = {20, 80, s1->w, s1->h};
    SDL_RenderCopy(juego->renderizar, t1, NULL, &r1);
    SDL_FreeSurface(s1);
    SDL_DestroyTexture(t1);

    char opt2[120];
    snprintf(opt2,sizeof(opt2),"Duracion inicial (ms) >=200 : %d   (teclas +/- para cambiar)", juego->config.duracion_inicial_ms);
    SDL_Surface *s2 = TTF_RenderText_Blended(juego->texto_fuente, opt2, juego->texto_color);
    SDL_Texture *t2 = SDL_CreateTextureFromSurface(juego->renderizar, s2);
    SDL_Rect r2 = {20, 130, s2->w, s2->h};
    SDL_RenderCopy(juego->renderizar, t2, NULL, &r2);
    SDL_FreeSurface(s2);
    SDL_DestroyTexture(t2);

    char opt3[160];
    snprintf(opt3,sizeof(opt3),"Modo: %s   (tecla T para togglear Sch/mozart)", juego->config.modo==MODO_SCHONBERG ? "Schönberg (aleatorio)" : "Mozart (archivo)" );
    SDL_Surface *s3 = TTF_RenderText_Blended(juego->texto_fuente, opt3, juego->texto_color);
    SDL_Texture *t3 = SDL_CreateTextureFromSurface(juego->renderizar, s3);
    SDL_Rect r3 = {20, 180, s3->w, s3->h};
    SDL_RenderCopy(juego->renderizar, t3, NULL, &r3);
    SDL_FreeSurface(s3);
    SDL_DestroyTexture(t3);

    // Si modo Mozart, mostrar ruta (si existe)
    if(juego->config.modo == MODO_MOZART) {
        char opt4[256];
        snprintf(opt4,sizeof(opt4),"Archivo melodia: %s  (tecla L para cargar nombre de archivo)", juego->config.ruta_melodia);
        SDL_Surface *s4 = TTF_RenderText_Blended(juego->texto_fuente, opt4, juego->texto_color);
        SDL_Texture *t4 = SDL_CreateTextureFromSurface(juego->renderizar, s4);
        SDL_Rect r4 = {20, 230, s4->w, s4->h};
        SDL_RenderCopy(juego->renderizar, t4, NULL, &r4);
        SDL_FreeSurface(s4);
        SDL_DestroyTexture(t4);
    }

    // Instrucciones
    char inst[200];
    snprintf(inst,sizeof(inst),"Enter: volver | SPACE: iniciar juego | M: toggle menu");
    SDL_Surface *sInst = TTF_RenderText_Blended(juego->texto_fuente, inst, juego->texto_color);
    SDL_Texture *tInst = SDL_CreateTextureFromSurface(juego->renderizar, sInst);
    SDL_Rect rInst = {20, PIXELES_VERTICALES - 40, sInst->w, sInst->h};
    SDL_RenderCopy(juego->renderizar, tInst, NULL, &rInst);
    SDL_FreeSurface(sInst);
    SDL_DestroyTexture(tInst);

    SDL_RenderPresent(juego->renderizar);
}

// --- Pedir nombre del jugador (interacción simple por teclado)
    void pedirNombreJugador(tJuego *juego, bool *corriendo)
    {
    // Dibujar prompt
    SDL_RenderClear(juego->renderizar);
    char prompt[128];
    snprintf(prompt, sizeof(prompt), "Ingrese nombre del jugador (ENTER para aceptar): %s", juego->nombre_jugador);
    SDL_Surface *s = TTF_RenderText_Blended(juego->texto_fuente, prompt, juego->texto_color);
    SDL_Texture *t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    SDL_Rect r = {20, PIXELES_VERTICALES/2 - s->h/2, s->w, s->h};
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
    SDL_RenderPresent(juego->renderizar);

    // Recibir eventos de teclado (sin bloquear)
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT) { *corriendo = false; return; }
        if(event.type == SDL_KEYDOWN)
        {
            if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) { *corriendo = false; return; }
            if(event.key.keysym.scancode == SDL_SCANCODE_RETURN)
            {
                // si vacío, poner "Anon"
                if(strlen(juego->nombre_jugador) == 0) strcpy(juego->nombre_jugador, "Anon");
                // volver a estado inicio para luego iniciar juego con SPACE
                juego->estado_juego = INICIO;
                return;
            }
            // backspace
            if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
            {
                size_t L = strlen(juego->nombre_jugador);
                if(L) juego->nombre_jugador[L-1] = '\0';
            }
            else {
                // capturamos letras
                SDL_Keycode key = event.key.keysym.sym;
                if((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z') || (key == ' '))
                {
                    size_t L = strlen(juego->nombre_jugador);
                    if(L + 1 < sizeof(juego->nombre_jugador)) {
                        juego->nombre_jugador[L] = (char)key;
                        juego->nombre_jugador[L+1] = '\0';
                    }
                }
            }
        }
    }
}

// --- Cargar melodía desde archivo (modo Mozart)
// Formato simple: lista de enteros 0..(num_botones-1) separados por espacios o nuevas lineas.
// Devuelve cantidad de notas cargadas, o -1 en error
int cargarMelodiaDesdeArchivo(const char *ruta, tJuego *juego)
{
    FILE *f = fopen(ruta, "r");
    if(!f) return -1;
    int count = 0;
    while(!feof(f) && count < MAX_SEQ)
    {
        int v;
        if(fscanf(f, "%d", &v) == 1) {
            if(v >= 0 && v < juego->config.num_botones) {
                juego->secuencia[count++] = v;
            } else {
                // si el archivo tiene valores fuera de rango, se ignora
            }
        } else {
            break;
        }
    }
    fclose(f);
    return count;
}

// --- calcular duración por nota dada la regla: por cada nota agregada se resta 3% (cumulativo)
// Ejemplo en enunciado: 1000, 970, 941, ...
int calcularDuracionPorNota(int duracion_inicial_ms, int cantidad_notas)
{
    double dur = (double)duracion_inicial_ms;
    for(int i=1;i<=(cantidad_notas-1);i++)
    {
        dur *= 0.97; // reduce 3% por nota adicional
    }
    return (int)(dur + 0.5);
}


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
                if (juego->estado_juego == INICIO)
                {
                   juego->estado_juego = PIDIENDO_NOMBRE;
                   SDL_StartTextInput();
                   printf("Juego iniciado. Nivel 1. Secuencia: %s\n", traducirColor(juego->secuencia[0]));
                }

                    else if(juego->estado_juego == FINALIZADO)
                {
                    reiniciarJuego(juego);
                    agregar_nuevo_color_secuencia(juego); ///Genero el primer color
                    juego->estado_juego = SECUENCIA;      /// Modifico el estado del juego
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
                    juego->partidas_jugadas++;
                    juego->partidas_perdidas++;
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
     SDL_SetRenderDrawColor(juego->renderizar, 0, 0, 0, 255);
    SDL_RenderClear(juego->renderizar);

    int cx = PIXELES_HORIZONTALES / 2;
    int cy = PIXELES_VERTICALES / 2;
    int radioExterior = (PIXELES_VERTICALES < PIXELES_HORIZONTALES ?
                         PIXELES_VERTICALES : PIXELES_HORIZONTALES) / 2 - 10;
    int radioInterior = radioExterior / 2; // agujero central

    int r,g,b;

    // recorremos solo la zona que ocupa el círculo
    for(int y = cy - radioExterior; y <= cy + radioExterior; y++)
    {
        for(int x = cx - radioExterior; x <= cx + radioExterior; x++)
        {
            int dx = x - cx;
            int dy = y - cy;
            int dist2 = dx*dx + dy*dy;

            // fuera del círculo de juego → saltar
            if(dist2 > radioExterior*radioExterior) continue;
            // dentro del agujero central → pintamos negro
            if(dist2 < radioInterior*radioInterior) {
                SDL_SetRenderDrawColor(juego->renderizar, 0,0,0,255);
                SDL_RenderDrawPoint(juego->renderizar, x, y);
                continue;
            }

            // calculamos ángulo
            double ang = atan2((double)dy, (double)dx); // [-π, π]
            if(ang < 0) ang += 2*M_PI; // a [0, 2π)

            // Determinar color del sector
            // Brillo base
            int brilloOscuro = 150;
            int brilloBrillante = 255;

            if(ang >= 0 && ang < M_PI/2) {       // 0 a 90° → abajo derecha → Azul
                if(juego->color_iluminado == AZUL) { r=0; g=0; b=brilloBrillante; }
                else                                { r=0; g=0; b=brilloOscuro; }
            }
            else if(ang >= M_PI/2 && ang < M_PI) { // 90 a 180° → abajo izquierda → Amarillo
                if(juego->color_iluminado == AMARILLO) { r=brilloBrillante; g=brilloBrillante; b=0; }
                else                                    { r=brilloOscuro; g=brilloOscuro; b=0; }
            }
            else if(ang >= M_PI && ang < 3*M_PI/2) { // 180 a 270° → arriba izquierda → Verde
                if(juego->color_iluminado == VERDE) { r=0; g=brilloBrillante; b=0; }
                else                                 { r=0; g=brilloOscuro; b=0; }
            }
            else {                                  // 270 a 360° → arriba derecha → Rojo
                if(juego->color_iluminado == ROJO) { r=brilloBrillante; g=0; b=0; }
                else                                { r=brilloOscuro; g=0; b=0; }
            }

            SDL_SetRenderDrawColor(juego->renderizar, r, g, b, 255);
            SDL_RenderDrawPoint(juego->renderizar, x, y);
        }
    }

    SDL_RenderPresent(juego->renderizar);
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
    int cx = PIXELES_HORIZONTALES / 2;
    int cy = PIXELES_VERTICALES / 2;
    int radioExterior = (PIXELES_VERTICALES < PIXELES_HORIZONTALES ?
                         PIXELES_VERTICALES : PIXELES_HORIZONTALES) / 2 - 10;
    int radioInterior = radioExterior / 2;

    int dx = x - cx;
    int dy = y - cy;
    double dist = sqrt(dx*dx + dy*dy);

    if(dist < radioInterior || dist > radioExterior)
        return SIN_COLOR;  // clic fuera de la zona de colores

    // determinar sector usando los ejes
    bool top = (y < cy);
    bool left = (x < cx);

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
    SDL_RenderClear(juego->renderizar);

    // Mostrar prompt
    char prompt[128];
    snprintf(prompt, sizeof(prompt), "Ingrese su nombre (ENTER para aceptar): %s", juego->nombre_jugador);

    SDL_Surface *s = TTF_RenderText_Blended(juego->texto_fuente, prompt, juego->texto_color);
    SDL_Texture *t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    SDL_Rect r = {40, PIXELES_VERTICALES/2 - s->h/2, s->w, s->h};
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);

    SDL_RenderPresent(juego->renderizar);

    // Procesar eventos
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            *corriendo = false; return;
        }

        if (event.type == SDL_TEXTINPUT)
        {
            if (strlen(juego->nombre_jugador) + strlen(event.text.text) < sizeof(juego->nombre_jugador) - 1)
                strcat(juego->nombre_jugador, event.text.text);
        }

        else if(event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key = event.key.keysym.sym;

            if(key == SDLK_ESCAPE) {
                *corriendo = false;
                SDL_StopTextInput();
                return;
            }
            else if(key == SDLK_RETURN) {
                if(strlen(juego->nombre_jugador) == 0)
                    strcpy(juego->nombre_jugador, "Anon");
                // cuando confirma, arrancamos el juego
                reiniciarJuego(juego);
                agregar_nuevo_color_secuencia(juego);
                juego->estado_juego = SECUENCIA;
                SDL_StopTextInput();
                return;
            }
            else if(key == SDLK_BACKSPACE) {
                size_t L = strlen(juego->nombre_jugador);
                if(L > 0) juego->nombre_jugador[L-1] = '\0';
            }
        }
        else if(event.type == SDL_TEXTINPUT) {   // <---- captura caracteres
            if(strlen(juego->nombre_jugador) + strlen(event.text.text) < sizeof(juego->nombre_jugador) - 1) {
                strcat(juego->nombre_jugador, event.text.text);
            }
        }
    }
}


void mostrarEstadisticas(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    SDL_Color blanco = {255,255,255,255};

    char linea[128];
    SDL_Surface *s;
    SDL_Texture *t;
    SDL_Rect r;
    int y = 100;

    // Título
    s = TTF_RenderText_Blended(juego->texto_fuente, "ESTADISTICAS DEL JUEGO", blanco);
    t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    r = (SDL_Rect){(PIXELES_HORIZONTALES - s->w)/2, y, s->w, s->h};
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
    y += 60;

    // Nombre del jugador
    snprintf(linea, sizeof(linea), "Jugador: %s", juego->nombre_jugador);
    s = TTF_RenderText_Blended(juego->texto_fuente, linea, blanco);
    t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    r = (SDL_Rect){50, y, s->w, s->h};
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
    y += 40;

    // Partidas jugadas
    snprintf(linea, sizeof(linea), "Partidas jugadas: %d", juego->partidas_jugadas);
    s = TTF_RenderText_Blended(juego->texto_fuente, linea, blanco);
    t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    r.y = y; r.x = 50; r.w = s->w; r.h = s->h;
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
    y += 40;

    // Ganadas
    snprintf(linea, sizeof(linea), "Partidas ganadas: %d", juego->partidas_ganadas);
    s = TTF_RenderText_Blended(juego->texto_fuente, linea, blanco);
    t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    r.y = y; r.x = 50; r.w = s->w; r.h = s->h;
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);
    y += 40;

    // Perdidas
    snprintf(linea, sizeof(linea), "Partidas perdidas: %d", juego->partidas_perdidas);
    s = TTF_RenderText_Blended(juego->texto_fuente, linea, blanco);
    t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    r.y = y; r.x = 50; r.w = s->w; r.h = s->h;
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);

    // Instrucciones
    s = TTF_RenderText_Blended(juego->texto_fuente, "Presione SPACE para volver a jugar o ESC para salir", blanco);
    t = SDL_CreateTextureFromSurface(juego->renderizar, s);
    r.y = PIXELES_VERTICALES - 60; r.x = 20; r.w = s->w; r.h = s->h;
    SDL_RenderCopy(juego->renderizar, t, NULL, &r);
    SDL_FreeSurface(s);
    SDL_DestroyTexture(t);

    SDL_RenderPresent(juego->renderizar);
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

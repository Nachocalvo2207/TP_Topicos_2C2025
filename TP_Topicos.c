#include "TP_Topicos.h"

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



    ///Carga de sonidos
    char *basePath = SDL_GetBasePath();
    char fullPath[256];

    const char* archivos[CANT_BOTONES] = {"sound_0.wav","sound_1.wav","sound_2.wav","sound_3.wav"
                              ,"sound_4.wav","sound_5.wav","sound_6.wav","sound_7.wav"
                            };

    for(int i=0; i<CANT_BOTONES; i++)
    {
        snprintf(fullPath, sizeof(fullPath), "%sSounds/%s", basePath, archivos[i]);
        juego->sonidos[i] = Mix_LoadWAV(fullPath);
        if(!juego->sonidos[i])
        {
            fprintf(stderr,"Error cargando %s: %s\n", fullPath, Mix_GetError());
        }
    }
    SDL_free(basePath);

    return false; ///FALSE ES NUESTRO CASO DE EXITO EN ESTE CASO
}


void inicializarConfiguracion(tJuego *juego)
{
    juego->config.num_botones = 4;
    juego->config.duracion_inicial_ms = DURACION_INICIAL; // Usa la macro del .h
    juego->config.modo = MODO_SCHONBERG;
    strcpy(juego->config.ruta_melodia, "ninguno");
}

void reiniciarJuego(tJuego *juego)
{
    // Esta función AHORA solo resetea los valores de una partida
    juego->estado_juego = INICIO;
    juego->nivel_actual = 1;
    juego->paso_actual_jugador = 0;
    juego->paso_secuencia = 0;
    juego->color_iluminado = SIN_COLOR;
    juego->tiempo_ultimo_cambio = 0;
    juego->nombre_jugador[0] = '\0'; // ¡ESTO ES CRUCIAL!

}


void palabra_mayus(char *palabra)
{
    // Mientras no lleguemos al final de la cadena (el carácter '\0')
    while (*palabra)
    {
        *palabra = toupper((unsigned char)*palabra);
        palabra++;
    }
}

void ordenar_top(tEstadistica *top_jugadores, int cantidad)
{
    tEstadistica elem_a_insertar;
    int j;

    for (int i = 1; i < cantidad; i++)
    {
        elem_a_insertar = top_jugadores[i];
        j = i - 1;

        while (j >= 0 && top_jugadores[j].nivel_alcanzado < elem_a_insertar.nivel_alcanzado)
        {

            top_jugadores[j + 1] = top_jugadores[j];
            j--;
        }

        top_jugadores[j + 1] = elem_a_insertar;
    }
}


bool crearTexto(tJuego *juego)
{
    ///Texto blanco
    juego->texto_color = (SDL_Color)
    {
        255, 255, 255, 255
    };

    juego->texto_fuente = TTF_OpenFont("fonts/freesansbold.ttf",TEXT_SIZE);
    if(!juego->texto_fuente)
    {
        fprintf(stderr,"ERROR CREANDO LA FUENTE: %s\n",TTF_GetError());
        return true;
    }

    juego->texto_config = TTF_OpenFont("fonts/freesansbold.ttf",TEXT_CONFIG_SIZE);
    if(!juego->texto_config)
    {
        fprintf(stderr,"ERROR CREANDO LA FUENTE DE LA CONFIG: %s\n",TTF_GetError());
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




void manejarEventos(tJuego *juego, bool *corriendo)
{
    SDL_Event event;

    // Procesa todos los eventos pendientes en la cola
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            *corriendo = false;
            break;

        case SDL_KEYDOWN:
        {
            ///Cierro todo con ESC
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
            {
                *corriendo = false;
                break;
            }
            // Usamos un switch principal para el estado del juego
            switch (juego->estado_juego)
            {
            case INICIO:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_1:
                    juego->estado_juego = PIDIENDO_NOMBRE;
                    SDL_StartTextInput();
                    break;
                case SDL_SCANCODE_2:
                    juego->estado_juego = MENU_CONFIG;
                    break;
                case SDL_SCANCODE_ESCAPE:
                    *corriendo = false;
                    break;
                default:
                    break;
                }
                break;

            case MENU_CONFIG:
                switch (event.key.keysym.scancode)
                {

                ///Ir al inicio nuevamente
                case SDL_SCANCODE_M:
                    juego->estado_juego = INICIO;
                    break;

                ///Cant. Botones SIMON
                case SDL_SCANCODE_3:
                    juego->config.num_botones = 3;
                    break;
                case SDL_SCANCODE_4:
                    juego->config.num_botones = 4;
                    break;
                case SDL_SCANCODE_5:
                    juego->config.num_botones = 5;
                    break;
                case SDL_SCANCODE_6:
                    juego->config.num_botones = 6;
                    break;
                case SDL_SCANCODE_7:
                    juego->config.num_botones = 7;
                    break;
                case SDL_SCANCODE_8:
                    juego->config.num_botones = 8;
                    break;
                case SDL_SCANCODE_E:
                    juego->config.duracion_inicial_ms += 100;
                    break;


                ///Aumentar velocidad
                case SDL_SCANCODE_H:
                    if (juego->config.duracion_inicial_ms > 200)
                    {
                        juego->config.duracion_inicial_ms -= 100;
                    }
                    break;
                ///Modo de juego
                case SDL_SCANCODE_T:
                    juego->config.modo = (juego->config.modo == MODO_SCHONBERG) ? MODO_MOZART : MODO_SCHONBERG;
                    break;
                default:
                    break;
                }
                break; ///FIN MENU_CONFIG

            case FINALIZADO:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_SPACE:
                    strcpy(juego->nombre_jugador, "");
                    juego->estado_juego = PIDIENDO_NOMBRE;
                    SDL_StartTextInput();
                    break;

                default:
                    break;
                }
                break; /// Fin FINALIZADO

            default:
                if(event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                    *corriendo = false;
                break;
            }
            break;
        }

        ///clic del mouse
        case SDL_MOUSEBUTTONDOWN:
        {
            if (juego->estado_juego == JUGANDO)
            {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                int color_clickeado = detectarBotonClick(mouseX, mouseY);

                if (color_clickeado == SIN_COLOR)
                {
                    break;
                }
                juego->color_iluminado = color_clickeado;
                if(juego->sonidos[color_clickeado])
                {
                    Mix_PlayChannel(-1, juego->sonidos[color_clickeado], 0);
                }
                dibujarTablero(juego);
                SDL_Delay(DURACION_FLASH_JUGADOR); // Usando la macro
                juego->color_iluminado = SIN_COLOR;

                if (color_clickeado == juego->secuencia[juego->paso_actual_jugador])
                {
                    juego->paso_actual_jugador++;
                    if (juego->paso_actual_jugador >= juego->nivel_actual)
                    {
                        if(juego->config.modo == MODO_SCHONBERG) ///MODO SCHONBERG
                        {
                            SDL_Delay(1000);
                            juego->nivel_actual++;
                            agregar_nuevo_color_secuencia(juego);
                            juego->estado_juego = SECUENCIA;
                            juego->paso_actual_jugador = 0;
                            juego->tiempo_ultimo_cambio = SDL_GetTicks();
                        }
                        else  ///MODO MOZART
                        {
                            if(juego->nivel_actual >= juego->long_melodia_mozart) ///Gano el juego
                            {
                                ///Agregar pantalla de victoria despues
                                actualizar_TOP(juego);
                                juego->estado_juego = FINALIZADO;
                            }
                            else
                            {
                                SDL_Delay(1000);
                                juego->nivel_actual++;
                                juego->estado_juego = SECUENCIA;
                                juego->paso_actual_jugador = 0;
                                juego->tiempo_ultimo_cambio = SDL_GetTicks();
                            }

                        }
                    }
                }else
                {
                    actualizar_TOP(juego);
                    juego->estado_juego = FINALIZADO;
                    juego->partidas_jugadas++;
                }

            }
            break;
        }

        } // Fin del switch(event.type)
    } // Fin del while(SDL_PollEvent)
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

            // fuera del círculo de juego saltar
            if(dist2 > radioExterior*radioExterior) continue;
            /// dentro del agujero central → pintamos negro
            if(dist2 < radioInterior*radioInterior)
            {
                SDL_SetRenderDrawColor(juego->renderizar, 0,0,0,255);
                SDL_RenderDrawPoint(juego->renderizar, x, y);
                continue;
            }

            // calculamos ángulo
            double ang = atan2((double)dy, (double)dx); // [-π, π]
            if(ang < 0) ang += 2*M_PI; // a [0, 2π)


            if(ang >= 0 && ang < M_PI/2)         // 0 a 90° → abajo derecha → Azul
            {
                if(juego->color_iluminado == AZUL)
                {
                    r=0;
                    g=0;
                    b=brilloBrillante;
                }
                else
                {
                    r=0;
                    g=0;
                    b=brilloOscuro;
                }
            }
            else if(ang >= M_PI/2 && ang < M_PI)   // 90 a 180° → abajo izquierda → Amarillo
            {
                if(juego->color_iluminado == AMARILLO)
                {
                    r=brilloBrillante;
                    g=brilloBrillante;
                    b=0;
                }
                else
                {
                    r=brilloOscuro;
                    g=brilloOscuro;
                    b=0;
                }
            }
            else if(ang >= M_PI && ang < 3*M_PI/2)   // 180 a 270° → arriba izquierda → Verde
            {
                if(juego->color_iluminado == VERDE)
                {
                    r=0;
                    g=brilloBrillante;
                    b=0;
                }
                else
                {
                    r=0;
                    g=brilloOscuro;
                    b=0;
                }
            }
            else                                    // 270 a 360° → arriba derecha → Rojo
            {
                if(juego->color_iluminado == ROJO)
                {
                    r=brilloBrillante;
                    g=0;
                    b=0;
                }
                else
                {
                    r=brilloOscuro;
                    g=0;
                    b=0;
                }
            }

            SDL_SetRenderDrawColor(juego->renderizar, r, g, b, 255);
            SDL_RenderDrawPoint(juego->renderizar, x, y);
        }
    }

    SDL_RenderPresent(juego->renderizar);
}

///TEXTO CONFIG
// Dibuja texto alineado a la izquierda (el texto comienza en la coordenada X)
void dibujar_texto_izquierda(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c)
{
    SDL_Surface* superficie = TTF_RenderUTF8_Blended(f, t, c);
    if (!juego) return;
    SDL_Texture* textura = SDL_CreateTextureFromSurface(juego->renderizar, superficie);
    SDL_Rect r = {x, y, superficie->w, superficie->h};
    SDL_RenderCopy(juego->renderizar, textura, NULL, &r);
    SDL_FreeSurface(superficie);
    SDL_DestroyTexture(textura);
}

// Dibuja texto centrado horizontalmente en la coordenada X
void dibujar_texto_centro(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c)
{
    SDL_Surface* superficie = TTF_RenderUTF8_Blended(f, t, c);
    if (!superficie)
        return;
    SDL_Texture* textura = SDL_CreateTextureFromSurface(juego->renderizar, superficie);
    SDL_Rect r = {x - superficie->w / 2, y, superficie->w, superficie->h}; // Ajusta X para centrar
    SDL_RenderCopy(juego->renderizar, textura, NULL, &r);
    SDL_FreeSurface(superficie);
    SDL_DestroyTexture(textura);
}

// Dibuja texto alineado a la derecha (el texto termina en la coordenada X)
void dibujar_texto_derecha(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c)
{
    SDL_Surface* superficie = TTF_RenderUTF8_Blended(f, t, c);
    if (!superficie)
        return;
    SDL_Texture* textura = SDL_CreateTextureFromSurface(juego->renderizar, superficie);
    SDL_Rect r = {x - superficie->w, y, superficie->w, superficie->h};
    SDL_RenderCopy(juego->renderizar, textura, NULL, &r);
    SDL_FreeSurface(superficie);
    SDL_DestroyTexture(textura);
}


void dibujar_juego(tJuego *juego)
{

    SDL_RenderClear(juego->renderizar);
    //Dibujar el fondo/tablero
    dibujarTablero(juego);
    //Dibujar el texto encima del tablero
    SDL_RenderCopy(juego->renderizar, juego->textura_imagen, NULL, &juego->texto_rect);

    ///Muestro nivel actual:
    char texto_nivel[32];
    snprintf(texto_nivel,sizeof(texto_nivel),"Nivel: %d", juego->nivel_actual);


    SDL_Surface *superficie_nivel = TTF_RenderText_Blended(juego->texto_fuente,texto_nivel,juego->texto_color);

    if (!superficie_nivel)
    {
        fprintf(stderr,"ERROR CREANDO SUPERFICIE: %s\n",SDL_GetError());
        return;
    }

    SDL_Texture *textura_nivel = SDL_CreateTextureFromSurface(juego->renderizar,superficie_nivel);

    SDL_Rect rect_Nivel;

    rect_Nivel.w = superficie_nivel->w; // Ancho del texto
    rect_Nivel.h = superficie_nivel->h; // Alto del texto
    rect_Nivel.x = PIXELES_HORIZONTALES - rect_Nivel.w - 15; // Coordenada X (con 15px de margen derecho)
    rect_Nivel.y = PIXELES_VERTICALES - rect_Nivel.h - 15;   // Coordenada Y (con 15px de margen inferior)

    SDL_RenderCopy(juego->renderizar, textura_nivel, NULL, &rect_Nivel);

    SDL_FreeSurface(superficie_nivel);
    SDL_DestroyTexture(textura_nivel);


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

/// GENERO UN TONO AL AZAR
int generar_tono(int limite){
    return rand() % limite;
}


void agregar_nuevo_color_secuencia(tJuego *juego)
{
    ///Realloc para agregar memoria en caso de que haya llegado a la maxima sec
    if(juego->nivel_actual >= juego->capacidad_secuencia)
    {
        size_t nueva_capacidad = juego->capacidad_secuencia * DOBLE_CAPACIDAD;

        int *aux = realloc(juego->secuencia,nueva_capacidad *  sizeof(int));

        if(!aux)
        {
            fprintf(stderr, "Error: No se pudo ampliar la memoria para la secuencia.\n");
            return;
        }

        juego->secuencia = aux;
        juego->capacidad_secuencia = nueva_capacidad;

    }

    int indice = juego->nivel_actual - 1;
    juego->secuencia[indice] = generar_tono(juego->config.num_botones);
}


/// REALIZO LA SECUENCIA DE COLORES
void actualizarJuego(tJuego *juego)
{
    // Lógica para mostrar la secuencia
    if (juego->estado_juego == SECUENCIA)
    {
        int duracion_a_esperar;

        // Decidimos cuánto tiempo esperar
        if (juego->color_iluminado != -1)///Luz PRENDIDA
        {
            duracion_a_esperar = calcularDuracionPorNota(juego->config.duracion_inicial_ms, juego->nivel_actual);
        }
        else ///luz APAGADA
        {
            duracion_a_esperar = PAUSA_ENTRE_NOTAS;
        }

        Uint32 tiempo_actual = SDL_GetTicks();

        if (tiempo_actual > juego->tiempo_ultimo_cambio + duracion_a_esperar)
        {
            juego->tiempo_ultimo_cambio = tiempo_actual;

            if (juego->paso_secuencia >= juego->nivel_actual)
            {
                juego->estado_juego = JUGANDO;
                juego->paso_secuencia = 0;
                juego->color_iluminado = -1;
                return;
            }

            if (juego->color_iluminado == -1)
            {
                juego->color_iluminado = juego->secuencia[juego->paso_secuencia];
                if(juego->sonidos[juego->color_iluminado])
                    Mix_PlayChannel(-1, juego->sonidos[juego->color_iluminado], 0);

            }
            else
            {

                juego->color_iluminado = -1;
                juego->paso_secuencia++;
            }
        }
    }
    ///Pausa entre niveles
    else if (juego->estado_juego == NIVEL_COMPLETADO)
    {
        if (SDL_GetTicks() > juego->tiempo_ultimo_cambio + 1000)
        {
            juego->nivel_actual++;
            agregar_nuevo_color_secuencia(juego);
            juego->estado_juego = SECUENCIA;
            juego->paso_actual_jugador = 0;
            juego->tiempo_ultimo_cambio = SDL_GetTicks();
        }
    }
}


///Pantalla principal
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
        dest.y = 100;
        SDL_RenderCopy(juego->renderizar, juego->textura_imagen, NULL, &dest);
    }
    const char *opciones_menu[] = {"1. Jugar", "2. Configuracion", "Esc. Salir"};
    int y_inicial = 250; ///Donde arranco en Y

    for (int i = 0; i < OPCIONES; i++)
    {
        SDL_Surface *superficie = TTF_RenderText_Blended(juego->texto_fuente, opciones_menu[i], juego->texto_color);
        SDL_Texture *textura = SDL_CreateTextureFromSurface(juego->renderizar, superficie);

        SDL_Rect rectBoton;
        rectBoton.w = superficie->w;
        rectBoton.h = superficie->h;
        rectBoton.x = (PIXELES_HORIZONTALES - rectBoton.w) / 2; /// Centrado X
        rectBoton.y = y_inicial + (i * DISTANCIA_OPCIONES); /// Espacio entre opciones

        SDL_RenderCopy(juego->renderizar, textura, NULL, &rectBoton);

        // ¡Importante liberar los recursos temporales en cada iteración!
        SDL_FreeSurface(superficie);
        SDL_DestroyTexture(textura);
    }



    SDL_RenderPresent(juego->renderizar);
}




///PANTALLA DE CONFIGURACION
void mostrarMenuConfiguracion(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    /// Colores letras y fondo
    SDL_Color color_titulo = {255, 200, 0, 255};
    SDL_Color color_opcion = {255, 255, 255, 255};
    SDL_Color color_valor = {100, 255, 255, 255};
    SDL_Color color_guia = {150, 150, 150, 255};
    SDL_SetRenderDrawColor(juego->renderizar, 20, 20, 30, 255);
    SDL_Rect fondo = {0, 0, PIXELES_HORIZONTALES, PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &fondo);

    /// Posicion
    int y_actual = PIXELES_VERTICALES * 0.15;
    int y_incremento = PIXELES_VERTICALES / 10;
    int col_etiqueta_x = PIXELES_HORIZONTALES * 0.1;
    int col_valor_x = PIXELES_HORIZONTALES * 0.5;
    int col_guia_x = PIXELES_HORIZONTALES * 0.9;
    char buffer[128];

    ///TITULO
    dibujar_texto_centro(juego, "MENU DE CONFIGURACION", col_valor_x, y_actual, juego->texto_fuente, color_titulo);
    y_actual += y_incremento * 1.5;

    ///BOTONES
    dibujar_texto_izquierda(juego, "Cantidad de Botones:", col_etiqueta_x, y_actual, juego->texto_config, color_opcion);
    snprintf(buffer, sizeof(buffer), "%d", juego->config.num_botones);
    dibujar_texto_centro(juego, buffer, col_valor_x, y_actual, juego->texto_config, color_valor);
    dibujar_texto_derecha(juego, "(Teclas 3-8)", col_guia_x, y_actual, juego->texto_config, color_guia);
    y_actual += y_incremento;

    ///DURACION
    dibujar_texto_izquierda(juego, "Duracion Inicial (ms):", col_etiqueta_x, y_actual, juego->texto_config, color_opcion);
    snprintf(buffer, sizeof(buffer), "%d", juego->config.duracion_inicial_ms);
    dibujar_texto_centro(juego, buffer, col_valor_x, y_actual, juego->texto_config, color_valor);
    dibujar_texto_derecha(juego, "(Teclas E/H)", col_guia_x, y_actual, juego->texto_config, color_guia);
    y_actual += y_incremento;

    ///MODO
    dibujar_texto_izquierda(juego, "Modo de Juego:", col_etiqueta_x, y_actual, juego->texto_config, color_opcion);
    const char* modo_texto = (juego->config.modo == MODO_SCHONBERG) ? "Schonberg" : "Mozart";
    dibujar_texto_centro(juego, modo_texto, col_valor_x, y_actual, juego->texto_config, color_valor);
    dibujar_texto_derecha(juego, "(Tecla T)", col_guia_x, y_actual, juego->texto_config, color_guia);
    y_actual += y_incremento * 1.5;

    ///VOLVER
    dibujar_texto_centro(juego, "Presione 'M' para volver al menu principal", col_valor_x, y_actual, juego->texto_config, color_guia);

    SDL_RenderPresent(juego->renderizar);
}


// --- Pedir nombre del jugador (interacción simple por teclado)
void pedirNombreJugador(tJuego *juego, bool *corriendo)
{
    SDL_StartTextInput();
    SDL_RenderClear(juego->renderizar);

    const char *comentario = "Ingrese su nombre y presione ENTER";
    SDL_Surface *sComent = TTF_RenderText_Blended(juego->texto_fuente, comentario, juego->texto_color);
    SDL_Texture *tComent = SDL_CreateTextureFromSurface(juego->renderizar, sComent);
    SDL_Rect rComent =
    {
        40,
        PIXELES_VERTICALES/2 - 60,          // un poco más arriba que el prompt
        sComent->w,
        sComent->h
    };
    SDL_RenderCopy(juego->renderizar, tComent, NULL, &rComent);
    SDL_FreeSurface(sComent);
    SDL_DestroyTexture(tComent);

    // Mostrar prompt
    char prompt[128];
    snprintf(prompt, sizeof(prompt), "Nombre: %s", juego->nombre_jugador);

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
            *corriendo = false;
            return;
        }

        if (event.type == SDL_TEXTINPUT)
        {
            if (strlen(juego->nombre_jugador) + strlen(event.text.text) < sizeof(juego->nombre_jugador) - 1)
                strcat(juego->nombre_jugador, event.text.text);
        }

        else if(event.type == SDL_KEYDOWN)
        {
            SDL_Keycode key = event.key.keysym.sym;

            if(key == SDLK_ESCAPE)
            {
                *corriendo = false;
                SDL_StopTextInput();
                return;
            }
            else if(key == SDLK_RETURN)
            {
                if(strlen(juego->nombre_jugador) == 0)
                    strcpy(juego->nombre_jugador, "VACIO");
                // cuando confirma, arrancamos el juego
                palabra_mayus(juego->nombre_jugador);
                reiniciarJuego(juego);
                agregar_nuevo_color_secuencia(juego);
                juego->estado_juego = SECUENCIA;
                SDL_StopTextInput();
                return;
            }
            else if(key == SDLK_BACKSPACE)
            {
                size_t L = strlen(juego->nombre_jugador);
                if(L > 0) juego->nombre_jugador[L-1] = '\0';
            }
        }
    }
}

void actualizar_TOP(tJuego *juego)
{
    int nivel_logrado = juego->nivel_actual - 1; ///

    ///No tengo mismo nombre y mismo SCORE
    for(int i = 0; i < juego->cant_top_jugadores; i++)
    {
        if ( strcmp(juego->nombre_jugador,juego->top_jugadores[i].nombre) == 0
                && nivel_logrado == juego->top_jugadores[i].nivel_alcanzado)
            return; ///SALIMOS

    }

    ///Si la tabla no esta lleno o hay modificaciones dentro del TOP
    if(juego->cant_top_jugadores < TOP_JUGADORES || nivel_logrado > juego->top_jugadores[TOP_JUGADORES - 1].nivel_alcanzado)
    {
        tEstadistica nuevo_top;
        strcpy(nuevo_top.nombre,juego->nombre_jugador);
        nuevo_top.nivel_alcanzado = nivel_logrado;

        if(juego->cant_top_jugadores < TOP_JUGADORES)
        {
            juego->top_jugadores[juego->cant_top_jugadores] = nuevo_top;
            juego->cant_top_jugadores++;

        }
        else
        {
            juego->top_jugadores[TOP_JUGADORES - 1 ] = nuevo_top;
        }

        ///Ordenamos de nuevo
        ordenar_top(juego->top_jugadores, juego->cant_top_jugadores);
        guardar_estadisticas(juego);
    }

}


void mostrar_estadisticas(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    ///COLORES Y FONDO
    SDL_Color color_titulo = {255, 200, 0, 255};
    SDL_Color color_texto = {255, 255, 255, 255};
    SDL_Color color_valor = {100, 255, 255, 255};
    SDL_Color color_guia = {180, 180, 180, 255};
    SDL_SetRenderDrawColor(juego->renderizar, 20, 20, 30, 255);
    SDL_Rect fondo = {0, 0, PIXELES_HORIZONTALES, PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &fondo);

    int y_actual = PIXELES_VERTICALES * 0.1;
    char buffer[128];

    ///TITULO
    dibujar_texto_centro(juego, "MEJORES PUNTUACIONES", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_fuente, color_titulo);
    y_actual += PIXELES_VERTICALES / 8;

    /// Columnas
    int col_rank = PIXELES_HORIZONTALES * 0.2;
    int col_nombre = PIXELES_HORIZONTALES * 0.3;
    int col_score = PIXELES_HORIZONTALES * 0.8;

    for (int i = 0; i < juego->cant_top_jugadores; i++)
    {
        ///Ranking
        snprintf(buffer, sizeof(buffer), "%d.", i + 1);
        dibujar_texto_izquierda(juego, buffer, col_rank, y_actual, juego->texto_config, color_texto);
        ///Nombre
        dibujar_texto_izquierda(juego, juego->top_jugadores[i].nombre, col_nombre, y_actual, juego->texto_config, color_texto);
        ///Puntaje
        snprintf(buffer, sizeof(buffer), "%d", juego->top_jugadores[i].nivel_alcanzado);
        dibujar_texto_derecha(juego, buffer, col_score, y_actual, juego->texto_config, color_valor);

        y_actual += 40;

    }
    ///Parte inferior
    y_actual = PIXELES_VERTICALES * 0.85;
    dibujar_texto_centro(juego, "SPACE: Jugar de nuevo | ESC: Salir", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_guia);

    SDL_RenderPresent(juego->renderizar);

}/// Cargar melodía desde archivo (modo Mozart)
int cargarMelodiaDesdeArchivo(const char *ruta, tJuego *juego)
{
    FILE *melodia = fopen(ruta, "r");
    if (!melodia)
    {
        fprintf(stderr, "Error: No se pudo abrir el archivo de melodia '%s'\n", ruta);
        return ERROR_MELODIA;
    }

    /// Liberamos memoria
    if (juego->secuencia != NULL)
    {
        free(juego->secuencia);
    }

    // Empezamos con una capacidad inicial para la nueva melodía.
    juego->capacidad_secuencia = MAX_SEQ;
    juego->secuencia = malloc(juego->capacidad_secuencia * sizeof(int));
    if (!juego->secuencia)
    {
        fprintf(stderr, "Error: No se pudo reservar memoria inicial para la melodia.\n");
        fclose(melodia);
        return ERROR_MELODIA;
    }

    int contador = 0;
    int nota;

    while (fscanf(melodia, "%d", &nota) == 1)
    {
        ///Chequeo espacio
        if (contador >= juego->capacidad_secuencia)
        {
            size_t nueva_capacidad = juego->capacidad_secuencia * DOBLE_CAPACIDAD;
            int* temp = realloc(juego->secuencia, nueva_capacidad * sizeof(int));

            if (!temp)
            {
                fprintf(stderr, "Error al ampliar memoria para la melodia.\n");
                fclose(melodia);
                return contador;
            }

            juego->secuencia = temp;
            juego->capacidad_secuencia = nueva_capacidad;
            printf("DEBUG: Memoria de la melodia ampliada a %zu notas.\n", nueva_capacidad);
        }


        if (nota >= 0 && nota < juego->config.num_botones)
        {
            juego->secuencia[contador] = nota;
            contador++;
        }
    }

    fclose(melodia);
    return contador;
}


void cargar_estadisticas(tJuego *juego)
{
    FILE *estadisticas = fopen(ESTADISTICAS,"rb");

    ///Si no hay estadisticas
    if(!estadisticas)
    {
        juego->cant_top_jugadores = 0;
        for(int i = 0; i < TOP_JUGADORES; i++)
        {
            strcpy(juego->top_jugadores[i].nombre,"---");
            juego->top_jugadores[i].nivel_alcanzado = 0;
        }
        return;
    }

    ///Leo estadisticas
    juego->cant_top_jugadores = fread(juego->top_jugadores,sizeof(tEstadistica),TOP_JUGADORES,estadisticas);
    fclose(estadisticas);
}
void guardar_estadisticas(tJuego *juego)
{
    FILE *estadisticas = fopen(ESTADISTICAS,"wb");
    if(!estadisticas)
    {
        fprintf(stderr,"ERROR AL GUARDAR ESTADISTICAS\n");
        return;
    }
    fwrite(juego->top_jugadores,sizeof(tEstadistica),juego->cant_top_jugadores,estadisticas);
    fclose(estadisticas);

}

// --- calcular duración por nota dada la regla: por cada nota agregada se resta 3% (cumulativo)
// Ejemplo en enunciado: 1000, 970, 941, ...
int calcularDuracionPorNota(int duracion_inicial_ms, int cantidad_notas)
{
    double dur = (double)duracion_inicial_ms;
    for(int i=1; i<=(cantidad_notas-1); i++)
    {
        dur *= 0.97; // reduce 3% por nota adicional
    }
    return (int)(dur + 0.5);
}


void limpieza_juego(tJuego *juego, int Estatus_Salida)
{
    for(int i=0; i<4; i++)
    {
        if(juego->sonidos[i])
        {
            Mix_FreeChunk(juego->sonidos[i]);
            juego->sonidos[i] = NULL;
        }
    }
    Mix_CloseAudio();
    SDL_DestroyTexture(juego->textura_imagen);
    TTF_CloseFont(juego->texto_fuente);
    TTF_CloseFont(juego->texto_config);
    SDL_DestroyRenderer(juego->renderizar);
    SDL_DestroyWindow(juego->ventana);
    TTF_Quit();
    SDL_Quit();
    exit(Estatus_Salida);
}

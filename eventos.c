#include "juego.h"
#include "logica.h"
#include "estadisticas.h"
#include "graficos.h"

void manejarEventos(tJuego *juego, bool *corriendo)
{
    SDL_Event event;

    /// Procesa todos los eventos pendientes en la cola
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
                    juego->proximo_estado = SECUENCIA;
                    juego->estado_juego = PIDIENDO_NOMBRE;

                    strcpy(juego->nombre_jugador, "");
                    SDL_StartTextInput();
                    break;

                case SDL_SCANCODE_2:
                    juego->estado_juego = MENU_CONFIG;
                    break;

                case SDL_SCANCODE_3:
                    juego->proximo_estado = MODO_DESAFIO;
                    juego->estado_juego = PIDIENDO_NOMBRE;
                    strcpy(juego->nombre_jugador, "");
                    SDL_StartTextInput();
                    break;

                case SDL_SCANCODE_4:
                    juego->estado_juego = AYUDA;
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
                ///CAMBIAR MELODIA
                case SDL_SCANCODE_S:
                    if (strcmp(juego->config.ruta_melodia, RUTA_MOZART) == 0)
                    {
                        strcpy(juego->config.ruta_melodia, RUTA_DESAFIO);
                    }
                    else
                    {
                        strcpy(juego->config.ruta_melodia, RUTA_MOZART);
                    }
                    printf("Melodia seleccionada: %s\n", juego->config.ruta_melodia);
                    break;
                default:
                    break;
                }
                break; ///FIN MENU_CONFIG

            case VICTORIA:
                if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_SPACE)
                {
                    juego->estado_juego = INICIO;
                }
                break;

            case MODO_DESAFIO:
                if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_RETURN)
                {
                    guardar_melodia_desafio(juego);
                    juego->estado_juego = INICIO;
                }
                break;
            case ERROR_MOZART:
                if (event.key.keysym.scancode == SDL_SCANCODE_M)
                {
                    juego->estado_juego = INICIO;
                }
                break;

            case AYUDA:
                if (event.key.keysym.scancode == SDL_SCANCODE_M)
                {
                    juego->estado_juego = INICIO;
                }
                break;
            case FINALIZADO:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_SPACE:
                    strcpy(juego->nombre_jugador, "");
                    juego->estado_juego = PIDIENDO_NOMBRE;
                    SDL_StartTextInput();
                    break;

                case SDL_SCANCODE_M:
                    reiniciarJuego(juego);
                    break;
                default:
                    break;
                }
                break; /// Fin

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
            if (juego->estado_juego != JUGANDO && juego->estado_juego != MODO_DESAFIO)
                break; /// NO TIENE EN CUENTA EL CLIC

            int mouseX = event.button.x;
            int mouseY = event.button.y;
            int color_clickeado = detectarBotonClick(mouseX, mouseY, juego->config.num_botones);
            int color_correcto; ///PARA EL MODO_CHEAT
            if (color_clickeado == SIN_COLOR)
                break;
            juego->color_iluminado = color_clickeado;
            if(juego->sonidos[color_clickeado])
            {
                Mix_PlayChannel(-1, juego->sonidos[color_clickeado], 0);
            }
            dibujarTablero(juego);
            SDL_Delay(DURACION_FLASH_JUGADOR);
            juego->color_iluminado = SIN_COLOR;
            dibujarTablero(juego);

            if (juego->estado_juego == JUGANDO)
            {
                if(strcmp(juego->nombre_jugador,PALABRA_CHEAT)==0)
                {
                    int indice_inverso = juego->nivel_actual - 1 - juego->paso_actual_jugador;
                    color_correcto = juego->secuencia[indice_inverso];
                }
                else
                {
                    color_correcto = juego->secuencia[juego->paso_actual_jugador];
                }

                if (color_clickeado == color_correcto)
                {
                    juego->paso_actual_jugador++;
                    if (juego->paso_actual_jugador >= juego->nivel_actual)
                    {
                        if(juego->config.modo == MODO_SCHONBERG || strcmp(juego->nombre_jugador, PALABRA_CHEAT) == 0) ///MODO SCHONBERG
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
                                juego->estado_juego = VICTORIA;
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
                }
                else
                {
                    actualizar_TOP(juego);

                    juego->estado_juego = FINALIZADO;
                    juego->partidas_jugadas++;
                }

            }
            else if (juego->estado_juego == MODO_DESAFIO)
            {
                ///Aumento memoria si es necesario
                if (juego->nivel_actual >= juego->capacidad_secuencia)
                {
                    size_t nueva_capacidad = juego->capacidad_secuencia * DOBLE_CAPACIDAD;
                    int *aux = realloc(juego->secuencia, nueva_capacidad * sizeof(int));
                    if(!aux)
                    {
                        fprintf(stderr, "Error: No se pudo ampliar la memoria para la secuencia.\n");
                        return;
                    }
                    juego->secuencia = aux;
                    juego->capacidad_secuencia = nueva_capacidad;
                }

                /// Guardamos la nota y aumentamos el contador
                juego->secuencia[juego->nivel_actual] = color_clickeado;
                juego->nivel_actual++;
            }
            break;
        }

        }
    }
}


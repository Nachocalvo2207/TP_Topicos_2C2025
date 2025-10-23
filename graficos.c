#include "graficos.h"
#include "logica.h"

void dibujarTablero(tJuego *juego)
{
    SDL_SetRenderDrawColor(juego->renderizar, 0, 0, 0, 255); //Elejimos el color con el que queremos pintar
    SDL_RenderClear(juego->renderizar); //Pintamos toda la ventana con el color elejido

    int cx = PIXELES_HORIZONTALES / 2;
    int cy = PIXELES_VERTICALES / 2;
    int radioExterior = (PIXELES_VERTICALES < PIXELES_HORIZONTALES ?
                         PIXELES_VERTICALES : PIXELES_HORIZONTALES) / 2 - 10;
    int radioInterior = radioExterior / 2; // agujero central

    int r, g, b;

    // recorremos solo la zona que ocupa el círculo
    for(int y = cy - radioExterior; y <= cy + radioExterior; y++) //Itera verticalmente
    {
        for(int x = cx - radioExterior; x <= cx + radioExterior; x++) //Itera Horizontalmente
        {
            int dx = x - cx; //Cateto 1
            int dy = y - cy; //Cateto 2
            int dist2 = dx * dx + dy * dy; //Ecuacion de distancia del circulo x*x + y*y = r*r

            // fuera del círculo de juego saltar
            if(dist2 > radioExterior*radioExterior) continue;
            /// dentro del agujero central → pintamos negro
            if(dist2 < radioInterior*radioInterior)
            {
                SDL_SetRenderDrawColor(juego->renderizar, 0,0,0,255);
                SDL_RenderDrawPoint(juego->renderizar, x, y);
                continue;
            }

            /// calculamos ángulo
            double ang = atan2((double)dy, (double)dx); // [-π, π]

            if(ang < 0) ang += 2*M_PI; ///convertimos los angulos negativos a [0, 2π]

            double escalaBotones = 2.0 * M_PI / juego->config.num_botones;

            int indice_boton = (int)(ang / escalaBotones);

            if(juego->color_iluminado == indice_boton)
            {
                r = juego->lista_colores[indice_boton].r_brillante;
                g = juego->lista_colores[indice_boton].g_brillante;
                b = juego->lista_colores[indice_boton].b_brillante;
            }
            else
            {
                r = juego->lista_colores[indice_boton].r_oscuro;
                g = juego->lista_colores[indice_boton].g_oscuro;
                b = juego->lista_colores[indice_boton].b_oscuro;
            }

            SDL_SetRenderDrawColor(juego->renderizar, r, g, b, 255);
            SDL_RenderDrawPoint(juego->renderizar, x, y);
        }
    }

    SDL_RenderPresent(juego->renderizar);
}

void dibujar_texto_izquierda(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c)
{
    SDL_Surface* superficie = TTF_RenderUTF8_Blended(f, t, c);
    if (!superficie)
        return;
    SDL_Texture* textura = SDL_CreateTextureFromSurface(juego->renderizar, superficie);
    SDL_Rect r = {x, y, superficie->w, superficie->h};
    SDL_RenderCopy(juego->renderizar, textura, NULL, &r);
    SDL_FreeSurface(superficie);
    SDL_DestroyTexture(textura);
}

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
    dibujarTablero(juego);
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
    rect_Nivel.x = PIXELES_HORIZONTALES - rect_Nivel.w - 15; /// Coordenada X (con 15px de margen derecho)
    rect_Nivel.y = PIXELES_VERTICALES - rect_Nivel.h - 15;   /// Coordenada Y (con 15px de margen inferior)

    SDL_RenderCopy(juego->renderizar, textura_nivel, NULL, &rect_Nivel);

    SDL_FreeSurface(superficie_nivel);
    SDL_DestroyTexture(textura_nivel);


    SDL_RenderPresent(juego->renderizar);


}

void mostrarPantallaPresentacion(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);


    SDL_SetRenderDrawColor(juego->renderizar, 20, 20, 30, 255);
    SDL_Rect full = {0, 0, PIXELES_HORIZONTALES, PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &full);


    if(juego->textura_imagen)
    {
        SDL_Rect dest = juego->texto_rect;
        dest.y = 100;
        SDL_RenderCopy(juego->renderizar, juego->textura_imagen, NULL, &dest);
    }

    const char *opciones_menu[] = {"1. Jugar", "2. Configuracion", "3. Crear Desafio", "4. Ayuda", "Esc. Salir"};
    int y_inicial = 220;

    for (int i = 0; i < OPCIONES; i++)
    {
        SDL_Surface *superficie = TTF_RenderText_Blended(juego->texto_fuente, opciones_menu[i], juego->texto_color);
        SDL_Texture *textura = SDL_CreateTextureFromSurface(juego->renderizar, superficie);

        SDL_Rect rectBoton;
        rectBoton.w = superficie->w;
        rectBoton.h = superficie->h;
        rectBoton.x = (PIXELES_HORIZONTALES - rectBoton.w) / 2;
        rectBoton.y = y_inicial + (i * DISTANCIA_OPCIONES);

        SDL_RenderCopy(juego->renderizar, textura, NULL, &rectBoton);

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
///  TIPO DE MELODÍA
    dibujar_texto_izquierda(juego, "Melodia Activa:", col_etiqueta_x, y_actual, juego->texto_config, color_opcion);

    // Extraemos solo el nombre del archivo de la ruta completa
    const char* nombre_archivo = strrchr(juego->config.ruta_melodia, '/');
    if (nombre_archivo)
    {
        nombre_archivo++;
    }
    else
    {
        nombre_archivo = juego->config.ruta_melodia;
    }

    dibujar_texto_centro(juego, nombre_archivo, col_valor_x, y_actual, juego->texto_config, color_valor);
    dibujar_texto_derecha(juego, "(Tecla S)", col_guia_x, y_actual, juego->texto_config, color_guia);
    y_actual += y_incremento * 1.5; /// Aumentamos el espaciado

    ///VOLVER
    dibujar_texto_centro(juego, "Presione 'M' para volver al menu principal", col_valor_x, y_actual, juego->texto_config, color_guia);

    SDL_RenderPresent(juego->renderizar);
}
void mostrarPantallaAyuda(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    SDL_Color color_titulo = {255, 200, 0, 255};
    SDL_Color color_subtitulo = {100, 255, 255, 255};
    SDL_Color color_texto = {220, 220, 220, 255};
    SDL_Color color_guia = {150, 150, 150, 255};
    SDL_SetRenderDrawColor(juego->renderizar, 20, 20, 30, 255);
    SDL_Rect fondo = {0, 0, PIXELES_HORIZONTALES, PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &fondo);

    int y_actual = PIXELES_VERTICALES * 0.1;
    int col_izq = PIXELES_HORIZONTALES * 0.1;
    int col_der = PIXELES_HORIZONTALES * 0.4;

    /// Título
    dibujar_texto_centro(juego, "AYUDA DEL JUEGO", PIXELES_HORIZONTALES / 2, y_actual, juego->texto_fuente, color_titulo);
    y_actual += 80;

    /// Sección Modo Mozart
    dibujar_texto_izquierda(juego, "Modo Mozart:", col_izq, y_actual, juego->texto_config, color_subtitulo);
    y_actual += 40;
    dibujar_texto_izquierda(juego, "1. Crea un archivo llamado 'Melodia.txt' en carpeta origen", col_izq, y_actual, juego->texto_ayuda_fuente, color_texto);
    y_actual += 30;
    dibujar_texto_izquierda(juego, "2. Escribe una secuencia de numeros separados por espacios.", col_izq, y_actual, juego->texto_ayuda_fuente, color_texto);
    y_actual += 40;
    dibujar_texto_izquierda(juego, "Codigos de colores:", col_izq, y_actual, juego->texto_config, color_subtitulo);
    y_actual += 30;

    /// Lista de colores
    dibujar_texto_izquierda(juego, "0 = VERDE", col_izq, y_actual, juego->texto_config, color_texto);
    dibujar_texto_izquierda(juego, "4 = NARANJA", col_der, y_actual, juego->texto_config, color_texto);
    y_actual += 30;
    dibujar_texto_izquierda(juego, "1 = ROJO", col_izq, y_actual, juego->texto_config, color_texto);
    dibujar_texto_izquierda(juego, "5 = ROSA", col_der, y_actual, juego->texto_config, color_texto);
    y_actual += 30;
    dibujar_texto_izquierda(juego, "2 = AMARILLO", col_izq, y_actual, juego->texto_config, color_texto);
    dibujar_texto_izquierda(juego, "6 = VIOLETA", col_der, y_actual, juego->texto_config, color_texto);
    y_actual += 30;
    dibujar_texto_izquierda(juego, "3 = AZUL", col_izq, y_actual, juego->texto_config, color_texto);
    dibujar_texto_izquierda(juego, "7 = AQUAMARINO", col_der, y_actual, juego->texto_config, color_texto);
    y_actual += 100;

    /// Instrucción para volver
    dibujar_texto_centro(juego, "Presione 'M' para volver al menu principal", PIXELES_HORIZONTALES / 2, y_actual, juego->texto_config, color_guia);

    SDL_RenderPresent(juego->renderizar);
}

void mostrarPantallaErrorMozart(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    ///Colores y fondo
    SDL_Color color_titulo = {255, 50, 50, 255};   // Rojo para el error
    SDL_Color color_texto = {255, 255, 255, 255}; // Blanco
    SDL_Color color_guia = {180, 180, 180, 255};   // Gris
    SDL_SetRenderDrawColor(juego->renderizar, 40, 10, 10, 255); // Fondo rojo oscuro
    SDL_Rect fondo = {0, 0, PIXELES_HORIZONTALES, PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &fondo);

    int y_actual = PIXELES_VERTICALES * 0.3;

    dibujar_texto_centro(juego, "ERROR EN MODO MOZART", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_fuente, color_titulo);
    y_actual += 80;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "No se pudo abrir el archivo de melodia:");
    dibujar_texto_centro(juego, buffer, (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_texto);
    y_actual += 40;

    dibujar_texto_centro(juego, juego->config.ruta_melodia, (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_titulo);
    y_actual += 150;

    dibujar_texto_centro(juego, "Presione 'M' para volver al menu principal", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_guia);

    SDL_RenderPresent(juego->renderizar);
}

void mostrarPantallaVictoria(tJuego *juego)
{
    SDL_RenderClear(juego->renderizar);

    /// Colores y fondo
    SDL_Color color_titulo = {255, 215, 0, 255};   /// Dorado
    SDL_Color color_texto = {255, 255, 255, 255}; /// Blanco
    SDL_Color color_guia = {180, 180, 180, 255};   /// Gris
    SDL_SetRenderDrawColor(juego->renderizar, 10, 20, 40, 255); /// Azul oscuro
    SDL_Rect fondo = {0, 0, PIXELES_HORIZONTALES, PIXELES_VERTICALES};
    SDL_RenderFillRect(juego->renderizar, &fondo);

    int y_actual = PIXELES_VERTICALES * 0.2;
    char buffer[128];

    /// Titulo de Victoria
    dibujar_texto_centro(juego, "GANASTE!!", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_fuente, color_titulo);
    y_actual += 80;

    /// Mensaje de felicitaciones
    snprintf(buffer, sizeof(buffer), "¡Felicidades %s!", juego->nombre_jugador);
    dibujar_texto_centro(juego, buffer, (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_texto);
    y_actual += 50;

    snprintf(buffer, sizeof(buffer), "Completaste la melodia de MOZART");
    dibujar_texto_centro(juego, buffer, (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_texto);
    y_actual += 150;

    ///FIN
    dibujar_texto_centro(juego, "Presiona ESPACIO para volver al menu", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_guia);

    SDL_RenderPresent(juego->renderizar);
}


void mostrarPantallaError(tJuego *juego)
{
    SDL_SetRenderDrawColor(juego->renderizar, 180, 0, 0, 255);
    SDL_RenderClear(juego->renderizar);
    SDL_RenderPresent(juego->renderizar);
}


void pedirNombreJugador(tJuego *juego)
{
    SDL_SetRenderDrawColor(juego->renderizar, 20, 20, 30, 255);
    SDL_RenderClear(juego->renderizar);

    // Dibuja el texto "Ingrese su nombre..."
    const char *comentario = "Ingrese su nombre y presione ENTER";
    dibujar_texto_centro(juego, comentario, PIXELES_HORIZONTALES / 2, PIXELES_VERTICALES / 2 - 60, juego->texto_config, juego->texto_color);

    // Dibuja el nombre que se está escribiendo
    char prompt[128];
    snprintf(prompt, sizeof(prompt), "Nombre: %s_", juego->nombre_jugador); // Agregué un "_" para simular un cursor
    dibujar_texto_centro(juego, prompt, PIXELES_HORIZONTALES / 2, PIXELES_VERTICALES / 2, juego->texto_fuente, juego->texto_color);

    SDL_RenderPresent(juego->renderizar);
}



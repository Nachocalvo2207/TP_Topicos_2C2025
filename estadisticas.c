#include "estadisticas.h"
#include "graficos.h"

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
    dibujar_texto_centro(juego, "SPACE: Jugar de nuevo | M: Menu Principal | ESC: Salir", (PIXELES_HORIZONTALES / 2), y_actual, juego->texto_config, color_guia);
    SDL_RenderPresent(juego->renderizar);


}

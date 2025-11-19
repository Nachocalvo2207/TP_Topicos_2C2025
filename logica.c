#include "logica.h"


void reiniciarJuego(tJuego *juego)
{
    juego->estado_juego = INICIO;
    juego->nivel_actual = 1;
    juego->paso_actual_jugador = 0;
    juego->paso_secuencia = 0;
    juego->color_iluminado = SIN_COLOR;
    juego->tiempo_ultimo_cambio = 0;
}

void actualizarJuego(tJuego *juego)
{

    if (juego->estado_juego == SECUENCIA)
    {
        int duracion_a_esperar;


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

    if (juego->estado_juego == MOSTRANDO_ERROR)
    {

        if (SDL_GetTicks() > juego->tiempo_inicio_error + 1500)
        {
            juego->color_iluminado = SIN_COLOR;
            actualizar_TOP(juego);
            juego->partidas_jugadas++;
            juego->estado_juego = FINALIZADO;
        }
    }
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

int generar_tono(int limite)
{
    int num_tono = rand() % limite;
    return num_tono;
}

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

void guardar_melodia_desafio(tJuego *juego)
{
    FILE *archivo_desafio = fopen(RUTA_DESAFIO, "w"); //
    if (!archivo_desafio)
    {
        fprintf(stderr, "Error al crear el archivo de desafio.\n");
        return;
    }

    for (int i = 0; i < juego->nivel_actual; i++)
    {
        fprintf(archivo_desafio, "%d ", juego->secuencia[i]);
    }

    fclose(archivo_desafio);
}


int calcularDuracionPorNota(int duracion_inicial_ms, int cantidad_notas)
{
    double dur = (double)duracion_inicial_ms;
    for(int i=1; i<=(cantidad_notas-1); i++)
    {
        dur *= 0.97; /// reduce 3% por nota adicional
    }
    return (int)(dur + 0.5);
}

void palabra_mayus(char *palabra)
{
    while (*palabra)
    {
        *palabra = toupper((unsigned char)*palabra);
        palabra++;
    }
}

int detectarBotonClick(int x, int y, int N)
{
    int cx = PIXELES_HORIZONTALES / 2;
    int cy = PIXELES_VERTICALES / 2;
    int radioExterior = (PIXELES_VERTICALES < PIXELES_HORIZONTALES ?
                         PIXELES_VERTICALES : PIXELES_HORIZONTALES) / 2 - 10;
    int radioInterior = radioExterior / 2;

    int dx = x - cx;
    int dy = y - cy;
    double dist = sqrt(dx*dx + dy*dy); ///Distancia del mouse desde  el centro

    if(dist < radioInterior || dist > radioExterior)
        return SIN_COLOR;  /// clic fuera de la zona de colores

    double ang = atan2(dy, dx);
    if(ang < 0) ang += 2*M_PI; ///Si es negativo, le sumammos 2PI para que sea positivo

    double angulo_boton = 2*M_PI / N; ///Lo que cada boton ocupa
    int indice_boton = (int)(ang / angulo_boton);

    return indice_boton;
}

///Logica recuperatorio...
int cmp_tono_ascendente(const int *a, const int *b)
{
    return *a - *b; // Ordena de grave (menor) a agudo (mayor)
}

void intercambio_generico(void *a, void *b, size_t tam)
{
    char aux;
    char *ptr_a = (char *)a;
    char *ptr_b = (char *)b;

    // Intercambia byte por byte [cite: 2347, 5439]
    while (tam--) {
        aux = *ptr_a;
        *ptr_a = *ptr_b;
        *ptr_b = aux;
        ptr_a++;
        ptr_b++;
    }
}

// logica.c
void ordenar_secuencia_por_seleccion(tJuego *juego)
{
    int *base = juego->secuencia;
    int *fin = base + juego->nivel_actual;
    int *ptr_actual = base;

    int *pos_menor;
    int *ptr_recorrido;

    while (ptr_actual < fin - 1)
    {
        pos_menor = ptr_actual;
        ptr_recorrido = ptr_actual + 1;

        while (ptr_recorrido < fin)
        {

            if (cmp_tono_ascendente(ptr_recorrido, pos_menor) < 0)
            {
                pos_menor = ptr_recorrido;
            }
            ptr_recorrido++;
        }

        if (pos_menor != ptr_actual)
            intercambio_generico(ptr_actual, pos_menor, sizeof(int));

        ptr_actual++;
    }
}

// logica.c

void desordenar_secuencia(tJuego *juego)
{
    int *base = juego->secuencia;
    int *fin_mezcla = base + juego->nivel_actual;
    int *ptr_actual;
    int indice_aleatorio;

    for (ptr_actual = fin_mezcla - 1; ptr_actual > base; ptr_actual--)
    {
        size_t distancia = ptr_actual - base + 1;
        indice_aleatorio = rand() % distancia;
        int *ptr_aleatorio = base + indice_aleatorio;
        intercambio_generico(ptr_actual, ptr_aleatorio, sizeof(int));
    }
}

// logica.c

void mostrar_secuencia(tJuego *juego, const char *titulo)
{
    int *ptr_actual = juego->secuencia;
    int *fin = juego->secuencia + juego->nivel_actual;

    printf("\n--- %s (Nivel: %d) ---\n", titulo, juego->nivel_actual);

    while (ptr_actual < fin)
    {
        // Imprime el valor (tono) y avanza el puntero
        printf("%d ", *ptr_actual);
        ptr_actual++;
    }
    printf("\n-------------------------\n");
}

void forzar_reproduccion_secuencia(tJuego *juego)
{
    /// Asegura que el estado del juego sea la reproducción de la secuencia
    juego->estado_juego = SECUENCIA;
    juego->paso_secuencia = 0;
    juego->tiempo_ultimo_cambio = SDL_GetTicks();
    juego->paso_actual_jugador = 0;
    juego->color_iluminado = SIN_COLOR;
}

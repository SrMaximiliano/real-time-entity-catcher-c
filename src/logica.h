#ifndef DATA_H_
#define DATA_H_

#include "pokedex.h"
#include "pokemon.h"
#include "lista.h"
#include <time.h>
#include <string.h>
#include "../extra/engine.h"

typedef struct s_jugador jugador_t;
typedef struct s_juego juego_t;

/**
 * Se recibe la pokedex que va a a ser usada en el juego.
 * Si la pokedex esta vacia, o es NULL, se podra seguir jugando,
 * aunque no apareceran ningun pokemon.
 */
juego_t *logica_crear(pokedex_t *);

int logica(void *);

int logica_semilla_especifica(void *juego_void);

/**
 * Muestra el titulo, hora, blablabla.
 */
int logica_stats(void *juego_void);

void logica_destruir(juego_t *);

/**
 * Borra tambien la pokedex y el hash que se le paso.
 */
void logica_destruir_todo(juego_t *);

#endif /* DATA_H_ */
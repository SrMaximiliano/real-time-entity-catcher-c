#ifndef POKEDEX_H_
#define POKEDEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "hash.h"
#include "pokemon.h"

typedef struct pokedex pokedex_t;

/**
 * Recibe un hash con todos los posibles colores para cada clave.
 * Esto, por ejemplo, me permitiria hacer que tanto AMARILLO como YELLOW me devuelvan
 * ANSI_COLOR_YELLOW si es que el hash quiso admitir diferentes idiomas para los .csv con la pokedex.
 */
pokedex_t *pokedex_crear(hash_t *colores);

/**
 * Devuelve un puntero al hash de colores de la pokedex.
 * Esto permite modificarla para, por ejemplo, hacer que la pokedex acepte archivos en varios idiomas.
 * Lo unico que se tendria que hacer para eso, es agregar los pares clave-valor, 
 * siendo la clave el como se escribira el color en el archivo y el codigo del color como valor.
 */
hash_t *pokedex_hash_de_colores(pokedex_t *);
/**
 * Recibe un archivo por parametro y almacena los distintos pokemones en la pokedex.
 * Devuelve true si se pudo leer. False si ocurrio un error.
 */
bool pokedex_leer_csv(pokedex_t *, const char *);

/**
 * Muestra todos los pokemones  y sus  datos ordenados por pantalla.
 * Devuelve true si se pudieron imprimir todos los pokemons.
 */
int pokedex_mostrar(void *pokedex_void);

/**
 * Devuelve un pokemon random de la pokedex
 */
pokemon_t *pokemon_random(pokedex_t *);

/**
 *  Devuelve la cantidad de pokemons guardados.
 */
size_t pokedex_cantidad(pokedex_t *);

/**
 * Destrulle la memoria reservada para todos los pokemons.
 * Nunca quiero quedarme con la memoria reservada para los pokemons, asi que no existe pokedex_destruir.
 * No borro el hash de colores porque puede estar siendo usado por otros TDAS.
 */
void pokedex_destruir(pokedex_t *);

/**
 * Ahora si borro el hash de colores
 */
void pokedex_destruir_todo(pokedex_t *);

#endif /* POKEDEX_H_ */

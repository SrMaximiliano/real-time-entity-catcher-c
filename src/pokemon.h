#ifndef POKEMON_H_
#define POKEMON_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct pokemon pokemon_t;

pokemon_t *poke_crear();

void poke_modificar_nombreYsigno(pokemon_t *pokemon, char *nombre);

void poke_modificar_valor(pokemon_t *pokemon, size_t nuevo_valor);

void poke_modificar_color(pokemon_t *pokemon, char *color);

void poke_modificar_color_ANSI(pokemon_t *pokemon, char *color_ansi);

void poke_modificar_patron(pokemon_t *pokemon, char *patron);

void poke_modificar_posicion(pokemon_t *pokemon, int aumento_x, int aumento_y);

// Funcion para copiar los datos de un pokemon a otro
void poke_copiar(pokemon_t *poke_nuevo, pokemon_t *poke_a_copiar);

//A continuacion estan las funciones encargadas de devolver los valores del pokemon
char *poke_nombre(pokemon_t *);

size_t poke_valor(pokemon_t *);

char *poke_color(pokemon_t *);

char *poke_color_ANSI(pokemon_t *);

char *poke_patron(pokemon_t *);

char poke_signo(pokemon_t *);

char poke_letra_patron_actual(pokemon_t *pokemon);

int poke_x(pokemon_t *);

int poke_y(pokemon_t *);

void liberar_pokemon(void *mi_pokemon_void);
//Funcion para liberar la memoria reservada para el pokemon

#endif /* POKEMON_H_ */
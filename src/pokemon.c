#include "pokemon.h"
#include <string.h>

#define ALTURA 16
#define ANCHO 33

struct pokemon {
	char *nombre;
	size_t valor;
	char *color;
	char *color_ANSI;
	char *patron;
	int pos_patron_actual;
	char signo;
	int x;
	int y;
};

pokemon_t *poke_crear()
{
	pokemon_t *nuevo_poke = calloc(1, sizeof(pokemon_t));
	if (nuevo_poke == NULL) {
		return NULL;
	}
	int errors = 0;

	nuevo_poke->color = calloc(30, sizeof(char));
	if (nuevo_poke->color == NULL)
		errors++;
	nuevo_poke->color_ANSI = calloc(30, sizeof(char));
	if (nuevo_poke->color_ANSI == NULL)
		errors++;
	nuevo_poke->nombre = calloc(100, sizeof(char));
	if (nuevo_poke->nombre == NULL)
		errors++;
	nuevo_poke->patron = calloc(30, sizeof(char));
	if (errors > 0 || nuevo_poke->patron == NULL) {
		liberar_pokemon(nuevo_poke);
		return NULL;
	}
	nuevo_poke->signo = ' ';

	return nuevo_poke;
}

void poke_modificar_nombreYsigno(pokemon_t *pokemon, char *nombre)
{
	if (pokemon == NULL || nombre == NULL)
		return;
	strcpy(pokemon->nombre, nombre);
	pokemon->signo = nombre[0];
}

void poke_modificar_valor(pokemon_t *pokemon, size_t nuevo_valor)
{
	if (pokemon == NULL)
		return;
	pokemon->valor = nuevo_valor;
}

void poke_modificar_color(pokemon_t *pokemon, char *color)
{
	if (pokemon == NULL || color == NULL)
		return;
	strcpy(pokemon->color, color);
}

void poke_modificar_color_ANSI(pokemon_t *pokemon, char *color_ansi)
{
	if (pokemon == NULL || color_ansi == NULL)
		return;
	strcpy(pokemon->color_ANSI, color_ansi);
}

void poke_modificar_patron(pokemon_t *pokemon, char *patron)
{
	if (pokemon == NULL || patron == NULL)
		return;
	strcpy(pokemon->patron, patron);
	if (pokemon->patron[strlen(patron) - 1] == '\n')
		pokemon->patron[strlen(patron) - 1] = '\0';
}

int max_p(int a, int b)
{
	return a > b ? a : b;
}

int min_p(int a, int b)
{
	return a < b ? a : b;
}

void poke_modificar_posicion(pokemon_t *pokemon, int aumento_x, int aumento_y)
{
	if (pokemon == NULL)
		return;

	if (aumento_x == aumento_y && aumento_x == 0) {
		pokemon->x = 1;
		pokemon->y = 1;
		return;
	}

	pokemon->x = min_p(ANCHO, max_p(1, pokemon->x + aumento_x));
	pokemon->y = min_p(ALTURA, max_p(1, pokemon->y + aumento_y));
}

char *poke_nombre(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NULL;
	return pokemon->nombre;
}

size_t poke_valor(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return 0;

	return pokemon->valor;
}

char *poke_color(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NULL;

	return pokemon->color;
}

char *poke_color_ANSI(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return "NULL";

	return pokemon->color_ANSI;
}

char *poke_patron(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return NULL;

	return pokemon->patron;
}

char poke_signo(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return '\0';

	return pokemon->signo;
}

int poke_x(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return -1;
	return pokemon->x;
}
int poke_y(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return -1;
	return pokemon->y;
}

void poke_copiar(pokemon_t *poke_nuevo, pokemon_t *poke_a_copiar)
{
	if (poke_nuevo == NULL || poke_a_copiar == NULL)
		return;
	strcpy(poke_nuevo->color, poke_a_copiar->color);
	strcpy(poke_nuevo->color_ANSI, poke_a_copiar->color_ANSI);
	strcpy(poke_nuevo->nombre, poke_a_copiar->nombre);
	strcpy(poke_nuevo->patron, poke_a_copiar->patron);
	poke_nuevo->signo = poke_a_copiar->signo;
	poke_nuevo->valor = poke_a_copiar->valor;
	poke_nuevo->x = poke_a_copiar->x;
	poke_nuevo->y = poke_a_copiar->y;
	poke_nuevo->pos_patron_actual = poke_a_copiar->pos_patron_actual;
}

char poke_letra_patron_actual(pokemon_t *pokemon)
{
	if (pokemon == NULL)
		return '\0';
	if (pokemon->patron == NULL)
		return '\0';
	if (pokemon->pos_patron_actual == strlen(pokemon->patron) ||
	    strlen(pokemon->patron) == 0)
		pokemon->pos_patron_actual = 0;
	char letra_actual_patron =
		(pokemon->patron)[pokemon->pos_patron_actual];
	pokemon->pos_patron_actual++;
	return letra_actual_patron;
}

void liberar_pokemon(void *mi_pokemon_void)
{
	if (mi_pokemon_void == NULL)
		return;
	pokemon_t *pokemon = (pokemon_t *)mi_pokemon_void;
	free(pokemon->color);
	free(pokemon->color_ANSI);
	free(pokemon->nombre);
	free(pokemon->patron);
	free(pokemon);
	pokemon = NULL;
}
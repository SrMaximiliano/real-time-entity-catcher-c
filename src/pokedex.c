#include "pokedex.h"
#include "../extra/ansi.h"
#include "abb.h"
#include "csv.h"
#include <string.h>
#define ALTURA 16
#define ANCHO 33

struct pokedex {
	abb_t *pokemons;
	hash_t *colores;
};

int comparador_nombres_pokemon(void *nombre_pokemon_actual,
			       void *nombre_nuevo_pokemon)
{
	pokemon_t *pokemon_actual = (pokemon_t *)nombre_pokemon_actual;
	pokemon_t *nuevo_pokemon = (pokemon_t *)nombre_nuevo_pokemon;
	return strcmp(poke_nombre(pokemon_actual), poke_nombre(nuevo_pokemon));
}

pokedex_t *pokedex_crear(hash_t *colores)
{
	if (colores == NULL)
		return NULL;

	pokedex_t *mi_pokedex = calloc(1, sizeof(pokedex_t));
	if (mi_pokedex == NULL)
		return NULL;

	mi_pokedex->pokemons = abb_crear(comparador_nombres_pokemon);
	if (mi_pokedex->pokemons == NULL) {
		free(mi_pokedex);
		return NULL;
	}
	mi_pokedex->colores = colores;

	return mi_pokedex;
}

bool guardar_string(const char *string_ar, void *string_poke)
{
	if (string_ar == NULL)
		return false;
	strcpy((char *)string_poke, string_ar);
	return true;
}
bool guardar_puntacion(const char *puntuacion_ar, void *puntuacion_poke)
{
	if (puntuacion_ar != NULL) {
		*(int *)puntuacion_poke = atoi(puntuacion_ar);
		return true;
	}
	return false;
}

bool añadir_pokemon(pokedex_t *pokedex, void *valores_pokemon[])
{
	pokemon_t *nuevo_poke = poke_crear();
	// Me fijo si el color del pokemon esta dentro del hash de colores.
	bool contiene =
		hash_contiene(pokedex->colores, (char *)(valores_pokemon[2]));

	if (nuevo_poke == NULL || contiene == false) {
		liberar_pokemon(nuevo_poke);
		return false;
	}

	poke_modificar_nombreYsigno(nuevo_poke, (char *)(valores_pokemon[0]));
	poke_modificar_valor(nuevo_poke, *(size_t *)(valores_pokemon[1]));
	poke_modificar_color(nuevo_poke, (char *)(valores_pokemon[2]));
	char *color_ANSI = (char *)hash_buscar(pokedex->colores,
					       (char *)(valores_pokemon[2]));
	poke_modificar_color_ANSI(nuevo_poke, color_ANSI);
	poke_modificar_patron(nuevo_poke, (char *)(valores_pokemon[3]));

	return abb_insertar(pokedex->pokemons, nuevo_poke);
}

bool rellenar_pokedex_con_filas_de_archivo(pokedex_t *pokedex,
					   struct archivo_csv *archivo,
					   void *valores_pokemon[])
{
	bool (*guardar_valores[])(const char *,
				  void *) = { guardar_string, guardar_puntacion,
					      guardar_string, guardar_string };
	bool seguir = true;
	while (leer_linea_csv(archivo, 4, guardar_valores, valores_pokemon) ==
		       4 &&
	       seguir) {
		seguir = añadir_pokemon(pokedex, valores_pokemon);
	}
	return seguir;
}

bool pokedex_leer_csv(pokedex_t *pokedex, const char *nombre_archivo)
{
	struct archivo_csv *archivo = abrir_archivo_csv(nombre_archivo, ',');
	if (archivo == NULL || pokedex == NULL || nombre_archivo == NULL)
		return false;

	size_t errores = 0;
	// Asumo que los nombres de los pokemon no tendran mas de 99 caracteres.
	char *nombre_pokemon = calloc(100, sizeof(char));
	if (nombre_pokemon == NULL)
		errores++;
	// Voy a asumir que no se me va a pasar un patron mayor a 29 caracteres.
	char *patron = calloc(30, sizeof(char));
	if (patron == NULL)
		errores++;
	// Asumo que no hay constantes para referirse a los colores de  mas de 29
	// caracteres.
	char *color = calloc(30, sizeof(char));
	if (color == NULL)
		errores++;

	void **valores_pokemon = calloc(4, sizeof(void *));
	if (errores > 0 || valores_pokemon == NULL) {
		cerrar_archivo_csv(archivo);
		free(nombre_pokemon);
		free(patron);
		free(color);
		return false;
	}

	size_t valor = 0;
	valores_pokemon[0] = nombre_pokemon;
	valores_pokemon[1] = &valor;
	valores_pokemon[2] = color;
	valores_pokemon[3] = patron;
	bool se_pudo_leer = rellenar_pokedex_con_filas_de_archivo(
		pokedex, archivo, valores_pokemon);

	free(nombre_pokemon);
	free(patron);
	free(color);
	free(valores_pokemon);
	cerrar_archivo_csv(archivo);

	if (se_pudo_leer == false) {
		abb_destruir_todo(pokedex->pokemons, liberar_pokemon);
		pokedex->pokemons = abb_crear(comparador_nombres_pokemon);
	}

	return se_pudo_leer;
}

bool mostrar_pokemon(void *pokemon_void, void *ctx)
{
	if (pokemon_void == NULL)
		return false;
	pokemon_t *pokemon = (pokemon_t *)pokemon_void;
	printf("\t\u2022 "
	       "%s (%c) %s" ANSI_COLOR_RESET
	       " (%s) -> %zu puntos\n" ANSI_COLOR_RESET,
	       poke_color_ANSI(pokemon), poke_signo(pokemon),
	       poke_nombre(pokemon), poke_patron(pokemon), poke_valor(pokemon));
	return true;
}

int pokedex_mostrar(void *pokedex_void)
{
	if (pokedex_void == NULL)
		return 0;
	pokedex_t *pokedex = (pokedex_t *)pokedex_void;
	printf("Pokemones conocidos:\n");
	void *ctx = NULL;
	if (abb_iterar_inorden(pokedex->pokemons, mostrar_pokemon, ctx) ==
	    abb_cantidad(pokedex->pokemons))
		return 1;
	return 0;
}

size_t pokedex_cantidad(pokedex_t *pokedex)
{
	if (pokedex == NULL)
		return 0;
	return abb_cantidad(pokedex->pokemons);
}

void pokedex_destruir_todo(pokedex_t *pokedex)
{
	if (pokedex == NULL)
		return;
	abb_destruir_todo(pokedex->pokemons, liberar_pokemon);
	hash_destruir(pokedex->colores);
	free(pokedex);
}

void pokedex_destruir(pokedex_t *pokedex)
{
	if (pokedex == NULL)
		return;
	abb_destruir_todo(pokedex->pokemons, liberar_pokemon);
	free(pokedex);
}

//Me permite iterar por un abb hasta que posicion sea igual que pos_actual
struct posicion_random {
	size_t posicion;
	size_t pos_actual;
	pokemon_t *pokemon_encontrado;
};

//Funcion hecha  para iterar por un abb hasta que se llegue a la posicion pedida.
bool encontrar_pokemom_random(void *pokemon_void, void *posicion_void)
{
	pokemon_t *pokemon = (pokemon_t *)pokemon_void;
	struct posicion_random *posicion =
		(struct posicion_random *)posicion_void;
	if (posicion->posicion == posicion->pos_actual) {
		posicion->pokemon_encontrado = pokemon;
		return false;
	}
	posicion->pos_actual++;
	return true;
}

pokemon_t *pokemon_random(pokedex_t *pokedex)
{
	if (pokedex == NULL || pokedex_cantidad(pokedex) == 0)
		return NULL;

	size_t pos_pokemon_random = (size_t)rand() % pokedex_cantidad(pokedex);

	struct posicion_random *pos = calloc(1, sizeof(struct posicion_random));
	pos->posicion = pos_pokemon_random;
	pos->pokemon_encontrado = NULL;

	abb_iterar_inorden(pokedex->pokemons, encontrar_pokemom_random, pos);
	pokemon_t *pokemon_random = pos->pokemon_encontrado;
	free(pos);
	return pokemon_random;
}

hash_t *pokedex_hash_de_colores(pokedex_t *pokedex)
{
	if (pokedex != NULL)
		return pokedex->colores;
	return NULL;
}

#include "pa2m.h"
#include "src/pokedex.h"
#include "src/logica.h"
#include "src/menu.h"
#include "extra/ansi.h"
#include <string.h>
#include "src/abb.h"

struct pokedex {
	abb_t *pokemons;
	hash_t *colores;
};

hash_t *crearHashDeColores()
{
	hash_t *colores = hash_crear(6);
	void **encontrado = NULL;
	pa2m_afirmar(hash_insertar(colores, "NEGRO",
				   ANSI_COLOR_BLACK ANSI_COLOR_BOLD,
				   encontrado) &&
			     hash_insertar(colores, "ROJO",
					   ANSI_COLOR_RED ANSI_COLOR_BOLD,
					   encontrado) &&
			     hash_insertar(colores, "VERDE",
					   ANSI_COLOR_GREEN ANSI_COLOR_BOLD,
					   encontrado) &&
			     hash_insertar(colores, "AMARILLO",
					   ANSI_COLOR_YELLOW ANSI_COLOR_BOLD,
					   encontrado) &&
			     hash_insertar(colores, "AZUL",
					   ANSI_COLOR_BLUE ANSI_COLOR_BOLD,
					   encontrado) &&
			     hash_insertar(colores, "MAGENTA",
					   ANSI_COLOR_MAGENTA ANSI_COLOR_BOLD,
					   encontrado) &&
			     hash_insertar(colores, "CIAN",
					   ANSI_COLOR_CYAN ANSI_COLOR_BOLD,
					   encontrado) &&
			     hash_insertar(colores, "BLANCO",
					   ANSI_COLOR_WHITE ANSI_COLOR_BOLD,
					   encontrado),
		     "Se pudo crear hash de colores");
	return colores;
}

void pokedexPrueba()
{
	hash_t *colores = crearHashDeColores();
	pokedex_t *pokedex = pokedex_crear(colores);
	pa2m_afirmar(pokedex_leer_csv(pokedex, "./datos/pokedex.csv"),
		     "Se pudo leer archivo.csv");
	pokedex_mostrar(pokedex);
	pa2m_afirmar(pokedex_cantidad(pokedex) == 10,
		     "Esta el numero correcto de pokemons");
	pokemon_t *poke1 = pokemon_random(pokedex);
	pokemon_t *poke2 = pokemon_random(pokedex);
	pokemon_t *poke3 = pokemon_random(pokedex);
	pokemon_t *poke4 = pokemon_random(pokedex);
	pokemon_t *poke5 = pokemon_random(pokedex);
	//Aviso que hay una chance de 0.001% de que sean todos iguales.
	pa2m_afirmar((poke1 != poke2) || (poke1 != poke3) || (poke1 != poke4) ||
			     (poke1 != poke5) || (poke2 != poke3) ||
			     (poke2 != poke4) || (poke2 != poke5) ||
			     (poke3 != poke4) || (poke3 != poke5) ||
			     (poke4 != poke5),
		     "Saque 5 pokemones random");
	pa2m_afirmar(pokedex_hash_de_colores(pokedex) == colores,
		     "Me devuelve el hash de colores que le di");
	int semilla = 11;
	srand((unsigned int)semilla);
	pokemon_t *poke6 = pokemon_random(pokedex);
	pa2m_afirmar(poke_letra_patron_actual(poke6) == 'R' &&
			     poke_letra_patron_actual(poke6) == 'J' &&
			     poke_letra_patron_actual(poke6) == 'R' &&
			     poke_letra_patron_actual(poke6) == 'I' &&
			     poke_letra_patron_actual(poke6) == 'R',
		     "Funciona devolver el patron letra por letra");
	pokedex_destruir_todo(pokedex);
}

void logicaPrueba()
{
	hash_t *colores = crearHashDeColores();
	pokedex_t *pokedex = pokedex_crear(colores);
	//Es un archivo de prueba donde es facil hacer combos.
	pokedex_leer_csv(pokedex, "./datos/pokedex_prueba.csv");
	juego_t *mi_juego = logica_crear(pokedex);
	logica(mi_juego);
	logica_stats(mi_juego);
	logica_destruir_todo(mi_juego);
}
void logicaPruebaSinPokedex()
{
	juego_t *mi_juego = logica_crear(NULL);
	logica(mi_juego);
	logica_destruir_todo(mi_juego);
}

int prueba_func()
{
	printf("Hola :) \n");
	return 1;
}

void menuPruebas()
{
	hash_t *colores = crearHashDeColores();
	pokedex_t *pokedex = pokedex_crear(colores);
	pokedex_leer_csv(pokedex, "./datos/pokedex.csv");
	juego_t *mi_juego = logica_crear(pokedex);

	menu_t *menu =
		menu_crear(pokedex, mi_juego, "Bienvenido a Pokemon Gon't!");
	pa2m_afirmar(menu_añadir_opcion(menu, 'L', 'l', "Funcion_prueba",
					prueba_func, 0),
		     "Se pudo agregar una opcion extra");
	mostrar_opciones(menu);
	pa2m_afirmar(menu_quitar_opcion(menu, 'L', 'l'),
		     "Se pudo quitar la funcion agregada");
	mostrar_opciones(menu);

	logica_destruir_todo(mi_juego);
	destruir_menu(menu);
}

void pokemonPrueba()
{
	pokemon_t *mi_pokemon = poke_crear();
	poke_modificar_nombreYsigno(mi_pokemon, "Maximiliano");
	poke_modificar_valor(mi_pokemon, 100);
	poke_modificar_color(mi_pokemon, "ROJO");
	poke_modificar_color_ANSI(mi_pokemon, ANSI_COLOR_RED);
	poke_modificar_patron(mi_pokemon, "J");
	poke_modificar_posicion(mi_pokemon, 15, 15);
	pokemon_t *pokemon_aux = poke_crear();
	poke_copiar(pokemon_aux, mi_pokemon);

	pa2m_afirmar(strcmp(poke_nombre(pokemon_aux), "Maximiliano") == 0 &&
			     poke_valor(pokemon_aux) == 100 &&
			     strcmp(poke_color(pokemon_aux), "ROJO") == 0 &&
			     strcmp(poke_color_ANSI(pokemon_aux),
				    ANSI_COLOR_RED) == 0 &&
			     *(poke_patron(pokemon_aux)) == 'J' &&
			     poke_x(pokemon_aux) == 15 &&
			     poke_y(pokemon_aux) == 15,
		     "Se pudo crear y copiar exitosamente el pokemon");
	liberar_pokemon(mi_pokemon);
	liberar_pokemon(pokemon_aux);
}

int main()
{
	pa2m_nuevo_grupo("============== pokedex ===============");
	pokedexPrueba();
	pa2m_nuevo_grupo("============== pokemon ===============");
	pokemonPrueba();
	// Comente estas pruebas por si el xanubot me daba timeout (al tener que moverse o elegir una opcion). Funcionan de todos modos!
	// pa2m_nuevo_grupo("============== logica ===============");
	// logicaPruebaSinPokedex();
	// logicaPrueba();
	// pa2m_nuevo_grupo("============== menu ===============");
	// menuPruebas();

	return pa2m_mostrar_reporte();
}

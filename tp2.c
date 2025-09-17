#include "extra/ansi.h"
#include "extra/engine.h"
#include "src/logica.h"
#include "src/pokedex.h"
#include "src/menu.h"
#include <stdio.h>
#include <time.h>

hash_t *crearHashDeColores()
{
	hash_t *colores = hash_crear(6);
	void **encontrado = NULL;
	hash_insertar(colores, "NEGRO", ANSI_COLOR_BLACK ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "ROJO", ANSI_COLOR_RED ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "VERDE", ANSI_COLOR_GREEN ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "AMARILLO", ANSI_COLOR_YELLOW ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "AZUL", ANSI_COLOR_BLUE ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "MAGENTA", ANSI_COLOR_MAGENTA ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "CIAN", ANSI_COLOR_CYAN ANSI_COLOR_BOLD,
		      encontrado);
	hash_insertar(colores, "BLANCO", ANSI_COLOR_WHITE ANSI_COLOR_BOLD,
		      encontrado);
	return colores;
}

int main(int argc, char const *argv[])
{
	if (argc < 2) {
		printf("Error! necesitas pasar un archivo");
		return 1;
	}
	hash_t *colores = crearHashDeColores();
	pokedex_t *pokedex = pokedex_crear(colores);
	pokedex_leer_csv(pokedex, argv[1]);
	juego_t *mi_juego = logica_crear(pokedex);
	menu_t *menu =
		menu_crear(pokedex, mi_juego, "Bienvenido a Pokemon Gon't!");
	menu_añadir_opcion(menu, 'J', 'j', "Jugar", logica, 1);
	menu_añadir_opcion(menu, 'S', 's', "Jugar con semilla",
			   logica_semilla_especifica, 1);
	menu_añadir_opcion(menu, 'P', 'p', "Mostrar pokedex", pokedex_mostrar,
			   2);
	mostrar_opciones(menu);
	menu_destruir_todo(menu);

	return 0;
}

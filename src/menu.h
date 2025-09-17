#ifndef __MENU__
#define __MENU__

#include <stdlib.h>
#include <stdbool.h>
#include "hash.h"
#include "pokedex.h"
#include "pokemon.h"
#include "logica.h"

typedef struct menu menu_t;

/**
 * Se crea el menu recibiendo como parametros la pokedex o juego que pueda ser pedido por las opciones de este. 
 * El titulo es lo que se muestra arriba de las posibles opciones a elegir.
 */
menu_t *menu_crear(pokedex_t *pokedex, juego_t *juego, char *titulo);

/**
 * Se recibe como parametros: el menu al que se le quiere añadir la opcion,
 * la letras que se pueden usar para elegir la opcion,
 * el nombre de como va a aparecer la opcion,
 * la funcion que si devuelve un numero mayor a 0 abrira de vuelta el menu y que, en caso contrario, lo cerrara.
 * Y finalmente juego_o_poke que, si es 0, significa que la funcion pasada le da igual el parametro que se le pase, 
 * si es 1 pide que se le pase el juego_t* del menu y si es 2 pedira que se le pase a la funcion la pokedex del menu. 
 */
bool menu_añadir_opcion(menu_t *menu, char clave_mayus, char clave_minus,
			char *nombre, int (*f)(void *), int juego_o_poke);

/**
 *  Muestra el titulo del menu y debajo de este las opciones que fueron agregadas.
 * De default, mostrara siempre la opcion para salirse del menu.
 */
void mostrar_opciones(menu_t *menu);

/**
 * Permite quitar la opcion querida pasandole la clave tanto mayuscula, como minuscula que se paso.
 *  Si hubo algun error, se devolvera false y no se quitaran las opciones. Caso contrario, se devolvera true.
 */
bool menu_quitar_opcion(menu_t *menu, char clave_mayus, char clave_minus);

/**
 * Destruye la memoria reservada para el menu.
 */
void destruir_menu(menu_t *menu);

/**
 * Libera la memoria reservada para el menu y la memoria reservada para la pokedex y el juego que se le paso.
 */
void menu_destruir_todo(menu_t *menu);

#endif /* __MENU__ */

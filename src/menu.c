#include "menu.h"
#include "pokedex.h"
#include "../extra/ansi.h"
struct menu {
	hash_t *opciones;
	pokedex_t *pokedex;
	juego_t *juego;
	char *titulo;
};
typedef struct {
	char *nombre;
	char letra;
	int (*f)(void *);
	int juego_o_poke;
} elem_menu;

bool menu_añadir_opcion(menu_t *menu, char clave_mayus, char clave_minus,
			char *nombre, int (*f)(void *), int juego_o_poke)
{
	if (menu == NULL || f == NULL || juego_o_poke < 0 || juego_o_poke > 2)
		return false;

	elem_menu *elem_nuevo = calloc(1, sizeof(elem_menu));
	if (elem_nuevo == NULL)
		return false;
	char *opcion = calloc(2, sizeof(char));
	if (opcion == NULL) {
		free(elem_nuevo);
		return false;
	}
	opcion[0] = clave_mayus;
	opcion[1] = '\0';

	elem_nuevo->nombre = nombre;
	elem_nuevo->letra = clave_mayus;
	elem_nuevo->f = f;
	elem_nuevo->juego_o_poke = juego_o_poke;

	bool añadido_mayus =
		hash_insertar(menu->opciones, opcion, elem_nuevo, NULL);
	if (añadido_mayus == false)
		free(elem_nuevo);

	elem_menu *elem_nuevo2 = calloc(1, sizeof(elem_menu));
	if (elem_nuevo2 == NULL) {
		return false; // El primer elemento se eliminara con destruir_menu();
	}
	opcion[0] =
		clave_minus; //Como el hash guarda una copia de la clave, puedo modificarla sin preocuparme.
	elem_nuevo2->nombre = NULL;
	elem_nuevo2->letra = clave_mayus;
	elem_nuevo2->f = f;
	elem_nuevo2->juego_o_poke = juego_o_poke;

	bool añadido_minus =
		hash_insertar(menu->opciones, opcion, elem_nuevo2, NULL);
	if (añadido_minus == false)
		free(elem_nuevo2);

	free(opcion);
	return (añadido_mayus && añadido_minus);
}

int salir_menu(void *dato)
{
	if (dato == NULL)
		return 0;
	return 0;
}

menu_t *menu_crear(pokedex_t *pokedex, juego_t *juego, char *titulo)
{
	menu_t *menu = calloc(1, sizeof(menu_t));
	if (menu == NULL)
		return NULL;

	menu->opciones = hash_crear(3);
	if (menu->opciones == NULL) {
		free(menu);
		return NULL;
	}
	menu_añadir_opcion(menu, 'Q', 'q', "Salir", salir_menu, 0);
	menu->pokedex = pokedex;
	menu->juego = juego;
	menu->titulo = titulo;

	return menu;
}

/*
Funcion hecha para ser usada en un hash_iterar. 
Se encarga de solo printear las opciones cuyo nombre no son nulas. 
O sea, las opciones cuya clave son una letra mayuscula.
*/
bool printear(char *clave, void *opcion_menu_void, void *par_void)
{
	if (clave == NULL || opcion_menu_void == NULL || par_void == NULL)
		return false;

	elem_menu *opcion_menu = (elem_menu *)opcion_menu_void;
	if (opcion_menu->nombre == NULL)
		return true;

	printf("\n (%c) %s", opcion_menu->letra, opcion_menu->nombre);
	(*(int *)par_void)++;
	return true;
}

void mostrar_opciones(menu_t *menu)
{
	if (menu == NULL)
		return;
	char *opcion = calloc(2, sizeof(char));
	if (opcion == NULL)
		return;

	opcion[1] = '\0';

	int prueba;
	int ctx = 0;
	do {
		printf("%s \n", menu->titulo);
		hash_iterar(menu->opciones, printear, &ctx);
		printf("\n Ingrese una opcion:" ANSI_COLOR_BOLD
			       ANSI_COLOR_RESET);
		prueba = scanf(" %c", &(opcion[0]));
		while (getchar() != '\n')
			;
	} while (!(prueba == 1 && hash_contiene(menu->opciones, opcion)));

	elem_menu *opcion_elegida =
		(elem_menu *)hash_buscar(menu->opciones, opcion);
	free(opcion);
	if (opcion_elegida == NULL) {
		return;
	}

	int volver_al_menu = 0;

	if (opcion_elegida->juego_o_poke == 0)
		volver_al_menu = opcion_elegida->f(NULL);
	else if (opcion_elegida->juego_o_poke == 1)
		volver_al_menu = opcion_elegida->f(menu->juego);
	else
		volver_al_menu = opcion_elegida->f(menu->pokedex);

	if (volver_al_menu >= 1)
		mostrar_opciones(menu);
}

bool menu_quitar_opcion(menu_t *menu, char clave_mayus, char clave_minus)
{
	if (menu == NULL)
		return false;
	char *clave = calloc(2, sizeof(char));
	if (clave == NULL)
		return false;

	clave[0] = clave_mayus;
	clave[1] = '\0';

	elem_menu *op_mayus = (elem_menu *)hash_quitar(menu->opciones, clave);
	if (op_mayus == NULL) {
		free(clave);
		return false;
	}

	clave[0] = clave_minus;
	elem_menu *op_min = (elem_menu *)hash_quitar(menu->opciones, clave);
	if (op_min == NULL) {
		clave[0] = clave_mayus;
		hash_insertar(menu->opciones, clave, op_mayus, NULL);
		free(clave);
		return false;
	}

	free(op_mayus);
	free(op_min);
	free(clave);

	return true;
}

void destruir_menu(menu_t *menu)
{
	if (menu == NULL)
		return;
	hash_destruir_todo(menu->opciones, free);
	free(menu);
}

void menu_destruir_todo(menu_t *menu)
{
	if (menu == NULL)
		return;
	hash_destruir_todo(menu->opciones, free);
	logica_destruir(menu->juego);
	pokedex_destruir_todo(menu->pokedex);
	free(menu);
}
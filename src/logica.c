#include "logica.h"
#include "../extra/ansi.h"
/*Le sume 1 a la altura y al ancho debido a que en la posiciones 0 de x e y
 estan los bordes de la esquina superior izquierda del juego.
*/
#define ALTURA 16
#define ANCHO 33

struct s_jugador {
	int x;
	int y;
};

typedef struct {
	pokemon_t *poke1;
	pokemon_t *poke2;
	pokemon_t *poke3;
	pokemon_t *poke4;
	pokemon_t *poke5;
	pokemon_t *poke6;
	pokemon_t *poke7;
} pokemon_activos_t;

typedef struct {
	size_t iteraciones;
	size_t puntaje;
	int semilla;
	size_t pokemon_atrapados;
	size_t multiplicador_puntaje;
	size_t multiplicador_maximo;
	pokemon_t *ultimo_pokemon_atrapado;
} datos_t;

/*
Struct de combos creados para ir dentro de la lista de combos del del struct s_juego.
Guarda la cantidad de puntos y el multiplicado obtenido por el combo hasta el momento,
y guarda tambien el ultimo pokemon atrapado.
*/
typedef struct combo_s combo_t;
struct combo_s {
	size_t puntos_total;
	size_t multiplicador;
	pokemon_t *pokemon_atrapado;
	combo_t *siguiente;
	combo_t *anterior;
};

struct s_juego {
	jugador_t *player;
	pokedex_t *pokedex;
	Lista *lista_de_combos; // Una vez que un combo_t se rompe, lo pongo aca
	combo_t *combo_actual;
	combo_t *combo_mas_largo;
	datos_t *datos;
	pokemon_activos_t *pokemon_activos;
	hash_t *patrones_posibles;
};

//Struct hecho para saber que dibujar al dibujar la matriz.
typedef struct {
	char signo;
	char *color_ANSI;
} objeto_matriz_t;

//Struct hecho para poder guardar todos los patrones posibles de los pokemon en un hash.
typedef struct {
	void (*f)(pokemon_t *, int);
} elem_hash_patrones_t;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// (INICIO)   FUNCIONES PARA SABER COMO MOVER AL JUGADOR o A LOS ENEMIGOS DEPENDIENDO DE LA LETRA DEL PATRON (INICIO)///
int max(int a, int b)
{
	return a > b ? a : b;
}

int min(int a, int b)
{
	return a < b ? a : b;
}

void mover_jugador(jugador_t *jugador, int entrada)
{
	if (entrada == TECLA_DERECHA)
		jugador->x++;
	else if (entrada == TECLA_IZQUIERDA)
		jugador->x--;
	else if (entrada == TECLA_ARRIBA)
		jugador->y--;
	else if (entrada == TECLA_ABAJO)
		jugador->y++;
	jugador->x = min(
		ANCHO, max(1, jugador->x)); // Es ALTURA / ANCHO + 1, porque el
	// jugador empieza en 1, envez de 0.
	jugador->y = min(ALTURA, max(1, jugador->y));
}

void mover_norte(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;
	poke_modificar_posicion(pokemon, 0, -1);
}
void mover_sur(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;
	poke_modificar_posicion(pokemon, 0, 1);
}
void mover_este(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;
	poke_modificar_posicion(pokemon, 1, 0);
}
void mover_oeste(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;
	poke_modificar_posicion(pokemon, -1, 0);
}
void mover_igual(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;
	if (entrada == TECLA_ARRIBA)
		poke_modificar_posicion(pokemon, 0, -1);

	if (entrada == TECLA_ABAJO)
		poke_modificar_posicion(pokemon, 0, 1);

	if (entrada == TECLA_IZQUIERDA)
		poke_modificar_posicion(pokemon, -1, 0);

	if (entrada == TECLA_DERECHA)
		poke_modificar_posicion(pokemon, 1, 0);
}
void mover_invertido(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;
	if (entrada == TECLA_ARRIBA)
		poke_modificar_posicion(pokemon, 0, 1);

	if (entrada == TECLA_ABAJO)
		poke_modificar_posicion(pokemon, 0, -1);

	if (entrada == TECLA_IZQUIERDA)
		poke_modificar_posicion(pokemon, 1, 0);

	if (entrada == TECLA_DERECHA)
		poke_modificar_posicion(pokemon, -1, 0);
}
void mover_random(pokemon_t *pokemon, int entrada)
{
	if (entrada == 0)
		return;

	int mover_x = rand();
	int mover_y = rand();

	if (mover_x > mover_y) {
		mover_x = 1;
		mover_y = 0;
	} else {
		mover_x = 0;
		mover_y = 1;
	}

	if (rand() % 2 == 0) { // Si un numero random es par, se va a restar la
		// posicion. Sino, se sumara.
		mover_x *= -1;
		mover_y *= -1;
	}

	poke_modificar_posicion(pokemon, mover_x, mover_y);
}
/// (FIN)  FUNCIONES PARA SABER COMO MOVER A LOS ENEMIGOS DEPENDIENDO DE LA LETRA DEL PATRON O AL JUGADOR (FIN) ///

/*
Permite mover al pokemon que se le pase, siguiendo el patron de movimientos de cada pokemon.
*/
void mover_enemigo(hash_t *patrones_posibles, pokemon_t *pokemon, int entrada)
{
	char *patron_buscado = calloc(2, sizeof(char));
	patron_buscado[0] = poke_letra_patron_actual(pokemon);
	patron_buscado[1] = '\0';
	elem_hash_patrones_t *mover_func = (elem_hash_patrones_t *)(hash_buscar(
		patrones_posibles, patron_buscado));
	free(patron_buscado);
	if (mover_func != NULL) {
		mover_func->f(pokemon, entrada);
		return;
	}
}

/*
Devuelve un hash con todos los posibles movimientos para cada letra que puede haber en un patron.
*/
hash_t *patrones_posibles()
{
	hash_t *patrones = hash_crear(7);
	if (patrones == NULL)
		return NULL;

	int errores = 0;
	elem_hash_patrones_t *norte_f = calloc(1, sizeof(elem_hash_patrones_t));
	if (norte_f == NULL)
		errores++;

	elem_hash_patrones_t *sur_f = calloc(1, sizeof(elem_hash_patrones_t));
	if (sur_f == NULL)
		errores++;

	elem_hash_patrones_t *este_f = calloc(1, sizeof(elem_hash_patrones_t));
	if (este_f == NULL)
		errores++;

	elem_hash_patrones_t *oeste_f = calloc(1, sizeof(elem_hash_patrones_t));
	if (oeste_f == NULL)
		errores++;

	elem_hash_patrones_t *igual_f = calloc(1, sizeof(elem_hash_patrones_t));
	if (igual_f == NULL)
		errores++;

	elem_hash_patrones_t *invertido_f =
		calloc(1, sizeof(elem_hash_patrones_t));
	if (invertido_f == NULL)
		errores++;

	elem_hash_patrones_t *random_f =
		calloc(1, sizeof(elem_hash_patrones_t));
	if (random_f == NULL || errores > 0) {
		free(norte_f);
		free(sur_f);
		free(este_f);
		free(oeste_f);
		free(igual_f);
		free(invertido_f);
		free(random_f);
		return NULL;
	}

	norte_f->f = mover_norte;
	sur_f->f = mover_sur;
	este_f->f = mover_este;
	oeste_f->f = mover_oeste;
	igual_f->f = mover_igual;
	invertido_f->f = mover_invertido;
	random_f->f = mover_random;

	char *norte = "N";
	char *sur = "S";
	char *este = "E";
	char *oeste = "O";
	char *mismo = "J";
	char *invertido = "I";
	char *random = "R";
	void **encontrado = calloc(1, sizeof(void *));
	hash_insertar(patrones, norte, norte_f, encontrado);
	hash_insertar(patrones, sur, sur_f, encontrado);
	hash_insertar(patrones, este, este_f, encontrado);
	hash_insertar(patrones, oeste, oeste_f, encontrado);
	hash_insertar(patrones, mismo, igual_f, encontrado);
	hash_insertar(patrones, invertido, invertido_f, encontrado);
	hash_insertar(patrones, random, random_f, encontrado);
	free(encontrado);
	return patrones;
}

/*
Funcion para reservar la memoria necesaria para la logica del juego.
Utilizo la variable errores para saber si ocurrio un error al reservar la memoria y,
una vez que termino de reservar toda la memoria, la libero si es que hubo un error.
Hago esto para evitar tener que hacer una gran cantidad de returns.
*/
juego_t *logica_crear(pokedex_t *pokedex)
{
	juego_t *mi_juego = calloc(1, sizeof(juego_t));
	if (mi_juego == NULL)
		return NULL;

	int errores = 0;
	mi_juego->player = calloc(1, sizeof(jugador_t));
	if (mi_juego->player == NULL)
		errores++;

	mi_juego->lista_de_combos = lista_crear();
	if (mi_juego->lista_de_combos == NULL)
		errores++;

	mi_juego->datos = calloc(1, sizeof(datos_t));
	if (mi_juego->datos == NULL)
		errores++;

	mi_juego->pokemon_activos = calloc(1, sizeof(pokemon_activos_t));
	if (mi_juego->pokemon_activos == NULL)
		errores++;

	mi_juego->pokemon_activos->poke1 = poke_crear();
	if (mi_juego->pokemon_activos->poke1 == NULL)
		errores++;
	mi_juego->pokemon_activos->poke2 = poke_crear();
	if (mi_juego->pokemon_activos->poke2 == NULL)
		errores++;
	mi_juego->pokemon_activos->poke3 = poke_crear();
	if (mi_juego->pokemon_activos->poke3 == NULL)
		errores++;
	mi_juego->pokemon_activos->poke4 = poke_crear();
	if (mi_juego->pokemon_activos->poke4 == NULL)
		errores++;
	mi_juego->pokemon_activos->poke5 = poke_crear();
	if (mi_juego->pokemon_activos->poke5 == NULL)
		errores++;
	mi_juego->pokemon_activos->poke6 = poke_crear();
	if (mi_juego->pokemon_activos->poke6 == NULL)
		errores++;
	mi_juego->pokemon_activos->poke7 = poke_crear();
	if (mi_juego->pokemon_activos->poke7 == NULL)
		errores++;

	mi_juego->patrones_posibles = patrones_posibles();
	if (mi_juego->patrones_posibles == NULL)
		errores++;

	mi_juego->datos->ultimo_pokemon_atrapado = poke_crear();
	if (mi_juego->datos->ultimo_pokemon_atrapado == NULL || errores > 0) {
		hash_destruir_todo(mi_juego->patrones_posibles, free);
		liberar_pokemon(mi_juego->pokemon_activos->poke7);
		liberar_pokemon(mi_juego->pokemon_activos->poke6);
		liberar_pokemon(mi_juego->pokemon_activos->poke5);
		liberar_pokemon(mi_juego->pokemon_activos->poke4);
		liberar_pokemon(mi_juego->pokemon_activos->poke3);
		liberar_pokemon(mi_juego->pokemon_activos->poke2);
		liberar_pokemon(mi_juego->pokemon_activos->poke1);
		free(mi_juego->pokemon_activos);
		free(mi_juego->datos);
		free(mi_juego->lista_de_combos);
		free(mi_juego->player);
		free(mi_juego);
		return NULL;
	}

	mi_juego->datos->multiplicador_puntaje = 1;
	mi_juego->pokedex = pokedex;
	if (pokedex_cantidad(pokedex) == 0)
		mi_juego->pokedex = NULL;
	return mi_juego;
}

/*
Es la funcion encargada de darle un objeto_matriz_t a cada espacio de la matriz que uso como tablero.
Tambien se encarga de asignarle los muros a la matriz. 
*/
void dar_valores_a_tablero(objeto_matriz_t tablero[ALTURA + 2][ANCHO + 2],
			   jugador_t *player, pokemon_activos_t *enemigos)
{
	if (enemigos->poke1 != NULL) {
		(tablero[poke_y(enemigos->poke1)][poke_x(enemigos->poke1)])
			.signo = poke_signo(enemigos->poke1);
		(tablero[poke_y(enemigos->poke1)][poke_x(enemigos->poke1)])
			.color_ANSI = poke_color_ANSI(enemigos->poke1);
	}
	if (enemigos->poke2 != NULL) {
		tablero[poke_y(enemigos->poke2)][poke_x(enemigos->poke2)].signo =
			poke_signo(enemigos->poke2);
		tablero[poke_y(enemigos->poke2)][poke_x(enemigos->poke2)]
			.color_ANSI = poke_color_ANSI(enemigos->poke2);
	}
	if (enemigos->poke3 != NULL) {
		tablero[poke_y(enemigos->poke3)][poke_x(enemigos->poke3)].signo =
			poke_signo(enemigos->poke3);
		tablero[poke_y(enemigos->poke3)][poke_x(enemigos->poke3)]
			.color_ANSI = poke_color_ANSI(enemigos->poke3);
	}

	if (enemigos->poke4 != NULL) {
		tablero[poke_y(enemigos->poke4)][poke_x(enemigos->poke4)].signo =
			poke_signo(enemigos->poke4);
		tablero[poke_y(enemigos->poke4)][poke_x(enemigos->poke4)]
			.color_ANSI = poke_color_ANSI(enemigos->poke4);
	}
	if (enemigos->poke5 != NULL) {
		tablero[poke_y(enemigos->poke5)][poke_x(enemigos->poke5)].signo =
			poke_signo(enemigos->poke5);
		tablero[poke_y(enemigos->poke5)][poke_x(enemigos->poke5)]
			.color_ANSI = poke_color_ANSI(enemigos->poke5);
	}
	if (enemigos->poke6 != NULL) {
		tablero[poke_y(enemigos->poke6)][poke_x(enemigos->poke6)].signo =
			poke_signo(enemigos->poke6);
		tablero[poke_y(enemigos->poke6)][poke_x(enemigos->poke6)]
			.color_ANSI = poke_color_ANSI(enemigos->poke6);
	}
	if (enemigos->poke7 != NULL) {
		tablero[poke_y(enemigos->poke7)][poke_x(enemigos->poke7)].signo =
			poke_signo(enemigos->poke7);
		tablero[poke_y(enemigos->poke7)][poke_x(enemigos->poke7)]
			.color_ANSI = poke_color_ANSI(enemigos->poke7);
	}

	(tablero[player->y][player->x]).signo = '@';
	(tablero[player->y][player->x]).color_ANSI = ANSI_COLOR_WHITE;

	for (int y = 0; y < ALTURA + 2; y++) {
		(tablero[y][0]).signo = ' ';
		(tablero[y][0]).color_ANSI = ANSI_BG_WHITE;
		(tablero[y][ANCHO + 1]).signo = ' ';
		(tablero[y][ANCHO + 1]).color_ANSI = ANSI_BG_WHITE;
		if (y == 0 || y == ALTURA + 1) {
			for (int x = 0; x < ANCHO + 2; x++) {
				(tablero[y][x]).signo = ' ';
				(tablero[y][x]).color_ANSI = ANSI_BG_WHITE;
			}
		}
	}
}

/*
Se copian los valores de un pokemon random de la pokedex y se les da  una posicion random.
Si el valor del pokemon es 0, significa que fue atrapado o que no existe.
*/
void cargar_enemigos(pokemon_activos_t *enemigos, juego_t *juego)
{
	if (poke_valor(enemigos->poke1) == 0) {
		poke_copiar(enemigos->poke1, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke1, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
	if (poke_valor(enemigos->poke2) == 0) {
		poke_copiar(enemigos->poke2, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke2, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
	if (poke_valor(enemigos->poke3) == 0) {
		poke_copiar(enemigos->poke3, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke3, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
	if (poke_valor(enemigos->poke4) == 0) {
		poke_copiar(enemigos->poke4, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke4, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
	if (poke_valor(enemigos->poke5) == 0) {
		poke_copiar(enemigos->poke5, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke5, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
	if (poke_valor(enemigos->poke6) == 0) {
		poke_copiar(enemigos->poke6, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke6, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
	if (poke_valor(enemigos->poke7) == 0) {
		poke_copiar(enemigos->poke7, pokemon_random(juego->pokedex));
		poke_modificar_posicion(enemigos->poke7, 1 + rand() % (ANCHO),
					1 + rand() % (ALTURA));
	}
}

/*
Funcion para crear combo_t*s.
Si llanma a esta funcion dandole NULL en vez de un pokemont_t* cuando se quiere marcar el final de un combo.
Guarda la multiplicacion que se tenia cuando se atrapo a un pokemon y los puntos que se consiguieron en el combo.
*/
combo_t *crear_combo(pokemon_t *atrapado, juego_t *juego)
{
	combo_t *nuevo_combo = calloc(1, sizeof(combo_t));
	if (nuevo_combo == NULL)
		return NULL;

	nuevo_combo->multiplicador = juego->datos->multiplicador_puntaje;
	nuevo_combo->anterior = NULL;
	nuevo_combo->siguiente = NULL;

	if (atrapado == NULL) {
		nuevo_combo->pokemon_atrapado = NULL;

		if (juego->combo_actual != NULL) {
			nuevo_combo->puntos_total =
				juego->combo_actual->puntos_total;

		} else
			nuevo_combo->puntos_total = 0;

	} else {
		if (juego->combo_actual->pokemon_atrapado == NULL) {
			nuevo_combo->puntos_total = 0;
			nuevo_combo->anterior = NULL;
		} else
			nuevo_combo->anterior = juego->combo_actual;

		juego->combo_actual->siguiente = nuevo_combo;

		nuevo_combo->pokemon_atrapado = poke_crear();

		if (nuevo_combo->pokemon_atrapado == NULL) {
			free(nuevo_combo);
			return NULL;
		}

		poke_copiar(nuevo_combo->pokemon_atrapado, atrapado);
		nuevo_combo->puntos_total += juego->combo_actual->puntos_total +
					     poke_valor(atrapado);
	}
	return nuevo_combo;
}

/*
Funcion encargada de ver de si el ultimo pokemon que atrape me:
iniciaria el combo, seguiria el combo, o me lo terminaria, haciendo que empiece uno de nuevo.
*/
void seguir_combo(juego_t *juego, pokemon_t *pokemon_atrapado)
{
	if ((strcmp(poke_color(juego->datos->ultimo_pokemon_atrapado),
		    poke_color(pokemon_atrapado)) != 0) &&
	    (poke_signo(juego->datos->ultimo_pokemon_atrapado) !=
	     poke_signo(pokemon_atrapado))) {
		juego->combo_actual = crear_combo(
			NULL,
			juego); // Marco el final del combo con un combo con un pokemon NULL
		juego->datos->multiplicador_puntaje = 1;

		poke_copiar(juego->datos->ultimo_pokemon_atrapado,
			    pokemon_atrapado);

		juego->datos->puntaje += (poke_valor(pokemon_atrapado) * 1);

		if (lista_agregar_al_final(juego->lista_de_combos,
					   juego->combo_actual) != false) {
			juego->combo_actual =
				crear_combo(pokemon_atrapado, juego);

			juego->combo_actual->puntos_total =
				poke_valor(pokemon_atrapado);
			lista_agregar_al_final(juego->lista_de_combos,
					       juego->combo_actual);

			if (juego->combo_mas_largo == NULL)
				juego->combo_mas_largo = juego->combo_actual;
		}
	} else {
		juego->datos->puntaje += (poke_valor(pokemon_atrapado) *
					  juego->datos->multiplicador_puntaje);

		juego->datos->multiplicador_puntaje += 1;
		poke_copiar(juego->datos->ultimo_pokemon_atrapado,
			    pokemon_atrapado);

		juego->combo_actual = crear_combo(pokemon_atrapado, juego);
		lista_agregar_al_final(juego->lista_de_combos,
				       juego->combo_actual);

		if (juego->combo_actual->multiplicador >
		    juego->combo_mas_largo->multiplicador)
			juego->combo_mas_largo = juego->combo_actual;
	}
}

/*
Esta funcion se activa al final de cada iteracion para ver si algun pokemon comparte coordenadas x, y con el jugador.
Si es asi, se fijara cuanto hay que aumentar el puntaje y si hay que aumentar el multiplicador de puntaje, 
se le pondra un valor de 0 al pokemon atrapado (porque en mi implementacion eso significa que no existe basicamente).
*/
void atrapar_pokemon(juego_t *juego)
{
	int x = juego->player->x;
	int y = juego->player->y;
	pokemon_activos_t *enemigos = juego->pokemon_activos;
	if (poke_x(enemigos->poke1) == x && poke_y(enemigos->poke1) == y) {
		seguir_combo(juego, enemigos->poke1);
		poke_modificar_valor(enemigos->poke1, 0);
	}

	if (poke_x(enemigos->poke2) == x && poke_y(enemigos->poke2) == y) {
		seguir_combo(juego, enemigos->poke2);
		poke_modificar_valor(enemigos->poke2, 0);
	}

	if (poke_x(enemigos->poke3) == x && poke_y(enemigos->poke3) == y) {
		seguir_combo(juego, enemigos->poke3);
		poke_modificar_valor(enemigos->poke3, 0);
	}

	if (poke_x(enemigos->poke4) == x && poke_y(enemigos->poke4) == y) {
		seguir_combo(juego, enemigos->poke4);
		poke_modificar_valor(enemigos->poke4, 0);
	}

	if (poke_x(enemigos->poke5) == x && poke_y(enemigos->poke5) == y) {
		seguir_combo(juego, enemigos->poke5);
		poke_modificar_valor(enemigos->poke5, 0);
	}

	if (poke_x(enemigos->poke6) == x && poke_y(enemigos->poke6) == y) {
		seguir_combo(juego, enemigos->poke6);
		poke_modificar_valor(enemigos->poke6, 0);
	}

	if (poke_x(enemigos->poke7) == x && poke_y(enemigos->poke7) == y) {
		seguir_combo(juego, enemigos->poke7);
		poke_modificar_valor(enemigos->poke7, 0);
	}
}

//Funcion para modularizar el tener que printear cada ciclo los datos basicos arriba del tablero.
void mostrar_texto_superior(juego_t *juego)
{
	printf("Utilizar " ANSI_COLOR_CYAN ANSI_COLOR_BOLD
	       "⬆ ⬇ ⬅ ➡" ANSI_COLOR_RESET " para moverse\n");

	printf("Presionar " ANSI_COLOR_RED ANSI_COLOR_BOLD "Q" ANSI_COLOR_RESET
	       " para salir\n");

	printf("\n \xF0\x9F\x94\x81 iteraciones: %zu \xE2\x8F\xB1 tiempo: %zu \xF0\x9F\x8C\xB1 semilla: %d \xF0\x9F\x092\xB0 puntos: %zu ",
	       juego->datos->iteraciones, juego->datos->iteraciones / 5,
	       juego->datos->semilla, juego->datos->puntaje);
	if (poke_valor(juego->datos->ultimo_pokemon_atrapado) != 0)
		printf("(\xE2\x9C\x96 %zu)\n \n",
		       juego->datos->multiplicador_puntaje);
	else
		printf("\n \n"); //Tiene un segundo espacio al final, porque si no los iconos se terminan pegando a la tabla.
}

/*
Funcion principal del TDA, auque sea una funcion privada.
Logica y logica_semilla-especifica la llaman para que se encargue de llamar el resto de funciones.
*/
int jugar(int entrada, void *juego_void)
{
	if (juego_void == NULL)
		return 1;
	juego_t *juego = (juego_t *)juego_void;
	pokemon_activos_t *enemigos = juego->pokemon_activos;
	if (juego->pokedex != NULL) {
		cargar_enemigos(enemigos, juego);
	}

	borrar_pantalla();
	if (entrada == TECLA_ABAJO || entrada == TECLA_ARRIBA ||
	    entrada == TECLA_DERECHA || entrada == TECLA_IZQUIERDA) {
		mover_jugador(juego->player, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke1, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke2, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke3, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke4, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke5, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke6, entrada);
		mover_enemigo(juego->patrones_posibles,
			      juego->pokemon_activos->poke7, entrada);
	}
	mostrar_texto_superior(juego);
	juego->datos->iteraciones++;

	objeto_matriz_t tablero
		[ALTURA + 2]
		[ANCHO +
		 2]; // Es +2 debido a que en AlTURA Y ANCHO +1 estara el borde.
	memset(tablero, 0, sizeof(tablero));
	dar_valores_a_tablero(tablero, juego->player, enemigos);

	for (int y = 0; y < ALTURA + 2; y++) {
		for (int x = 0; x < ANCHO + 2; x++) {
			objeto_matriz_t casilla = tablero[y][x];
			if (casilla.signo != 0)
				printf("%s%c" ANSI_COLOR_RESET,
				       casilla.color_ANSI, casilla.signo);
			else
				printf(" ");
		}
		printf("\n");
	}
	if (juego->pokedex != NULL)
		printf("Enemigos actuales: %s%s (%c) %zu\xF0\x9F\x092\xB0" ANSI_COLOR_RESET
		       ", %s%s (%c) %zu\xF0\x9F\x092\xB0" ANSI_COLOR_RESET
		       ", %s%s (%c) %zu\xF0\x9F\x092\xB0" ANSI_COLOR_RESET
		       ", %s%s (%c) %zu\xF0\x9F\x092\xB0" ANSI_COLOR_RESET
		       ", %s%s (%c) %zu\xF0\x9F\x092\xB0" ANSI_COLOR_RESET
		       ", %s%s (%c) %zu\xF0\x9F\x092\xB0" ANSI_COLOR_RESET " y "
		       "%s%s (%c) %zu\xF0\x9F\x092\xB0\n" ANSI_COLOR_RESET,
		       poke_color_ANSI(enemigos->poke1),
		       poke_nombre(enemigos->poke1),
		       poke_signo(enemigos->poke1), poke_valor(enemigos->poke1),
		       poke_color_ANSI(enemigos->poke2),
		       poke_nombre(enemigos->poke2),
		       poke_signo(enemigos->poke2), poke_valor(enemigos->poke2),
		       poke_color_ANSI(enemigos->poke3),
		       poke_nombre(enemigos->poke3),
		       poke_signo(enemigos->poke3), poke_valor(enemigos->poke3),
		       poke_color_ANSI(enemigos->poke4),
		       poke_nombre(enemigos->poke4),
		       poke_signo(enemigos->poke4), poke_valor(enemigos->poke4),
		       poke_color_ANSI(enemigos->poke5),
		       poke_nombre(enemigos->poke5),
		       poke_signo(enemigos->poke5), poke_valor(enemigos->poke5),
		       poke_color_ANSI(enemigos->poke6),
		       poke_nombre(enemigos->poke6),
		       poke_signo(enemigos->poke6), poke_valor(enemigos->poke6),
		       poke_color_ANSI(enemigos->poke7),
		       poke_nombre(enemigos->poke7),
		       poke_signo(enemigos->poke7),
		       poke_valor(enemigos->poke7));
	if (poke_valor(juego->datos->ultimo_pokemon_atrapado) != 0)
		printf("Ultimo pokemon atrapado: %s%s (%c) %zu\xF0\x9F\x092\xB0 \n" ANSI_COLOR_RESET,
		       poke_color_ANSI(juego->datos->ultimo_pokemon_atrapado),
		       poke_nombre(juego->datos->ultimo_pokemon_atrapado),
		       poke_signo(juego->datos->ultimo_pokemon_atrapado),
		       poke_valor(juego->datos->ultimo_pokemon_atrapado));
	else
		printf("No atrapaste ningun pokemon.");

	if (entrada == TECLA_ABAJO || entrada == TECLA_ARRIBA ||
	    entrada == TECLA_DERECHA || entrada == TECLA_IZQUIERDA)
		atrapar_pokemon(juego);

	esconder_cursor();
	if ((juego->datos->iteraciones / 5) == 60 || entrada == 'q' ||
	    entrada == 'Q') {
		juego->combo_actual = crear_combo(NULL, juego);
		lista_agregar_al_final(juego->lista_de_combos,
				       juego->combo_actual);
		return true;
	}

	return false;
}

void datos_finales(juego_t *juego)
{
	if (juego->combo_mas_largo == NULL) {
		printf(ANSI_COLOR_BOLD
		       "\nlograste obtener 0 puntos con un multiplicador de 0!\n" ANSI_COLOR_RESET);
		return;
	}

	//Dejo mi puntero del combo mas largo, al inicio de dicho combo
	while (juego->combo_mas_largo->anterior != NULL)
		juego->combo_mas_largo = juego->combo_mas_largo->anterior;

	pokemon_t *combo_poke = juego->combo_mas_largo->pokemon_atrapado;
	printf("\nEl Combo mas largo fue: ");
	while (juego->combo_mas_largo->siguiente != NULL) {
		printf("%s%s (%c) " ANSI_COLOR_RESET "--> " ANSI_COLOR_BOLD,
		       poke_color_ANSI(combo_poke), poke_nombre(combo_poke),
		       poke_signo(combo_poke));
		juego->combo_mas_largo = juego->combo_mas_largo->siguiente;
	}
	printf("%s%s (%c)" ANSI_COLOR_RESET
	       " que dio el multiplicador maximo de: (\xE2\x9C\x96 %zu)\n" ANSI_COLOR_BOLD,
	       poke_color_ANSI(combo_poke), poke_nombre(combo_poke),
	       poke_signo(combo_poke), juego->combo_mas_largo->multiplicador);
	printf("En total se obtuvieron %zu \xF0\x9F\x092\xB0 puntos!\n FELICITACIONES" ANSI_COLOR_RESET,
	       juego->datos->puntaje);
}

int logica(void *juego_void)
{
	if (juego_void == NULL)
		return 0;

	juego_t *juego = (juego_t *)juego_void;
	srand((unsigned int)time(NULL));
	int semilla = rand();
	srand((unsigned int)semilla);
	juego->datos->semilla = semilla;

	game_loop(jugar, juego);
	datos_finales(juego);
	mostrar_cursor();
	return 0;
}

int logica_semilla_especifica(void *juego_void)
{
	if (juego_void == NULL)
		return 0;
	juego_t *juego = (juego_t *)juego_void;
	int semilla;
	printf("Por favor escriba el numero de la semilla a usar: ");
	if (scanf("%d", &semilla) == 1) {
		srand((unsigned int)semilla);
		juego->datos->semilla = semilla;
		game_loop(jugar, juego);
		datos_finales(juego);
	}
	return 0;
}

//Funcion hecha para ser usado en un iterador de lista.
bool printear_combo(void *combo_void, void *pokemon_iterados_void)
{
	combo_t *combo = (combo_t *)combo_void;

	if (combo->pokemon_atrapado == NULL) {
		int *pokemon_iterados = (int *)pokemon_iterados_void;
		if (*pokemon_iterados != 1)
			printf("Puntuacion total: %zu \xF0\x9F\x092\xB0 . Multiplicador de puntaje alcanzado: (\xE2\x9C\x96 %zu) ",
			       combo->puntos_total, combo->multiplicador);
		printf("\nCombo %d:", *pokemon_iterados);
		(*pokemon_iterados)++;
	} else {
		printf("--> %s%s (%c) " ANSI_COLOR_RESET,
		       poke_color_ANSI(combo->pokemon_atrapado),
		       poke_nombre(combo->pokemon_atrapado),
		       poke_signo(combo->pokemon_atrapado));
	}
	return true;
}

//Me devuelve todos los combos que hice, con su puntuacion por combo + el multiplicado.
int logica_stats(void *juego_void)
{
	if (juego_void == NULL)
		return 1;
	juego_t *juego = (juego_t *)juego_void;
	int poke_iterados = 1;
	lista_iterar_elementos(juego->lista_de_combos, printear_combo,
			       &poke_iterados);
	printf(" vacio :(\n");
	return 1;
}

void destructor_lista_combos(void *combo_void)
{
	combo_t *combo = (combo_t *)combo_void;
	liberar_pokemon(combo->pokemon_atrapado);
	free(combo);
}

void logica_destruir_todo(juego_t *juego)
{
	if (juego == NULL)
		return;
	free(juego->player);
	liberar_pokemon(juego->pokemon_activos->poke1);
	liberar_pokemon(juego->pokemon_activos->poke2);
	liberar_pokemon(juego->pokemon_activos->poke3);
	liberar_pokemon(juego->pokemon_activos->poke4);
	liberar_pokemon(juego->pokemon_activos->poke5);
	liberar_pokemon(juego->pokemon_activos->poke6);
	liberar_pokemon(juego->pokemon_activos->poke7);
	liberar_pokemon(juego->datos->ultimo_pokemon_atrapado);
	free(juego->pokemon_activos);
	hash_destruir_todo(juego->patrones_posibles, free);
	lista_destruir_todo(juego->lista_de_combos, destructor_lista_combos);
	free(juego->datos);
	pokedex_destruir_todo(juego->pokedex);
	free(juego);
}

void logica_destruir(juego_t *juego)
{
	if (juego == NULL)
		return;
	free(juego->player);
	liberar_pokemon(juego->pokemon_activos->poke1);
	liberar_pokemon(juego->pokemon_activos->poke2);
	liberar_pokemon(juego->pokemon_activos->poke3);
	liberar_pokemon(juego->pokemon_activos->poke4);
	liberar_pokemon(juego->pokemon_activos->poke5);
	liberar_pokemon(juego->pokemon_activos->poke6);
	liberar_pokemon(juego->pokemon_activos->poke7);
	liberar_pokemon(juego->datos->ultimo_pokemon_atrapado);
	free(juego->pokemon_activos);
	hash_destruir_todo(juego->patrones_posibles, free);
	lista_destruir_todo(juego->lista_de_combos, destructor_lista_combos);
	free(juego->datos);
	free(juego);
}
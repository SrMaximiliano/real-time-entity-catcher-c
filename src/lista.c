#include "lista.h"
#include <stdio.h>
typedef struct s_nodo {
	void *elemento_actual;
	struct s_nodo *nodo_siguiente;

} Nodo;
struct lista {
	Nodo *primer_nodo;
	Nodo *ultimo_nodo;
	size_t cantidad_elementos;
};

struct lista_iterador {
	Nodo *nodo_actual;
};

Lista *lista_crear()
{
	Lista *mi_lista = calloc(1, sizeof(Lista));
	if (mi_lista == NULL) {
		free(mi_lista);
		return NULL;
	}
	mi_lista->cantidad_elementos = 0;
	mi_lista->primer_nodo = NULL;
	mi_lista->ultimo_nodo = NULL;
	return mi_lista;
}

size_t lista_cantidad_elementos(Lista *mi_lista)
{
	size_t cantidad_elem = 0;
	if (mi_lista != NULL)
		cantidad_elem = mi_lista->cantidad_elementos;
	return cantidad_elem;
}
bool lista_agregar_primer_elemento(Lista *lista, Nodo *primer_nodo)
{
	lista->primer_nodo = primer_nodo;
	lista->ultimo_nodo = primer_nodo;
	primer_nodo->nodo_siguiente = NULL;
	return true;
}
bool lista_agregar_elemento_principio(Lista *lista, Nodo *nuevo_nodo)
{
	nuevo_nodo->nodo_siguiente = lista->primer_nodo;
	lista->primer_nodo = nuevo_nodo;
	return true;
}

bool lista_agregar_elemento_en_el_medio(Lista *lista, Nodo *nuevo_nodo)
{
	nuevo_nodo->nodo_siguiente = lista->primer_nodo->nodo_siguiente;
	lista->primer_nodo->nodo_siguiente = nuevo_nodo;
	return true;
}

bool lista_agregar_al_final(Lista *lista, void *cosa)
{
	bool se_pudo_agregar = false;

	if (lista != NULL) {
		Nodo *nuevo_elem = calloc(1, sizeof(Nodo));

		if (nuevo_elem != NULL) {
			nuevo_elem->elemento_actual = cosa;
			nuevo_elem->nodo_siguiente = NULL;

			// Caso 1: Se usa para agregar el primer elemento.
			if (lista->cantidad_elementos == 0) {
				lista->primer_nodo = nuevo_elem;
				lista->ultimo_nodo = nuevo_elem;
				// Caso 2: Se usa para agregar al final de una lista no vacia.
			} else {
				lista->ultimo_nodo->nodo_siguiente = nuevo_elem;
				lista->ultimo_nodo = nuevo_elem;
			}
			lista->cantidad_elementos++;
			se_pudo_agregar = true;
		} else {
			free(nuevo_elem);
		}
	}

	return se_pudo_agregar;
}

void lista_dejar_en_nodo_anterior(Lista *lista, size_t posicion)
{
	size_t posicion_lista;
	for (posicion_lista = 0; posicion_lista < posicion - 1;
	     posicion_lista++) {
		lista->primer_nodo = lista->primer_nodo->nodo_siguiente;
	}
}

bool lista_agregar_elemento(Lista *lista, size_t posicion, void *cosa)
{
	if (lista != NULL) {
		if (posicion <= lista->cantidad_elementos) {
			bool se_pudo_agregar = false;
			// Caso 1: Accidentalmente se uso esta funcion para agregar al final.
			if (posicion == lista->cantidad_elementos) {
				se_pudo_agregar =
					lista_agregar_al_final(lista, cosa);
			} else {
				Nodo *nuevo_elem = calloc(1, sizeof(Nodo));

				if (nuevo_elem != NULL) {
					nuevo_elem->elemento_actual = cosa;

					// Caso 2: Se agrega el primer elemento:
					if (lista->cantidad_elementos == 0) {
						se_pudo_agregar =
							lista_agregar_primer_elemento(
								lista,
								nuevo_elem);
						// Caso 3: Se agrega el elemento en el primer lugar.
					} else if (posicion == 0) {
						se_pudo_agregar =
							lista_agregar_elemento_principio(
								lista,
								nuevo_elem);
					} else {
						// Me guarda la direccion del primer nodo
						Nodo *primer_nodo =
							lista->primer_nodo;

						// Me deja en la posicion anterior a la que va a ir el nuevo nodo.
						lista_dejar_en_nodo_anterior(
							lista, posicion);

						// Caso 4: Se agrega un elemento en cualquier otro lugar que no sea
						// el final:
						se_pudo_agregar =
							lista_agregar_elemento_en_el_medio(
								lista,
								nuevo_elem);

						// La lista vuelve a tener como primer nodo al original
						lista->primer_nodo =
							primer_nodo;
					}
					if (se_pudo_agregar)
						lista->cantidad_elementos++;
				} else {
					free(nuevo_elem);
				}
			}
			return se_pudo_agregar;
		}
	}
	return false;
}

bool comparar_funcion_con_elementos_lista(Lista *lista,
					  int (*comparador)(void *, void *),
					  void *buscado)
{
	size_t posicion_lista = 0;
	while (posicion_lista < lista->cantidad_elementos) {
		if (comparador(buscado, lista->primer_nodo->elemento_actual) ==
		    0) {
			return true;
		} else {
			lista->primer_nodo = lista->primer_nodo->nodo_siguiente;
			posicion_lista++;
		}
	}
	return false;
}

void *lista_buscar_elemento(Lista *lista, void *buscado,
			    int (*comparador)(void *, void *))
{
	if (lista != NULL && comparador != NULL) {
		// Caso 1: Hay un unico elemento.
		if (lista->cantidad_elementos == 1) {
			if (comparador(buscado,
				       lista->primer_nodo->elemento_actual) ==
			    0) {
				return lista->primer_nodo->elemento_actual;
			}
		} else { // Caso 2: Hay varios elementos en la lista.

			// Me guarda la direccion del primer nodo.
			Nodo *primer_nodo = lista->primer_nodo;
			bool encontrado = false;

			// Si hay mas de un elemento, se fijara en cada nodo si esta el elemento
			// buscado.
			encontrado = comparar_funcion_con_elementos_lista(
				lista, comparador, buscado);

			if (encontrado) {
				void *elemento_a_devolver =
					lista->primer_nodo->elemento_actual;
				lista->primer_nodo = primer_nodo;
				// Si es NULL, devolvera NULL.
				return elemento_a_devolver;
			} else {
				lista->primer_nodo = primer_nodo;
			}
		}
	}

	return NULL;
}

bool lista_quitar_elemento_en_primera_posicion(Lista *lista,
					       void **elemento_quitado)
{
	if (elemento_quitado != NULL)
		*elemento_quitado = lista->primer_nodo->elemento_actual;

	if (lista->cantidad_elementos > 1) {
		Nodo *a_borrar = lista->primer_nodo;
		lista->primer_nodo = lista->primer_nodo->nodo_siguiente;
		free(a_borrar);

		// Caso 1,5: La lista solo tiene un elemento.
	} else {
		free(lista->primer_nodo);
		lista->primer_nodo = NULL;
		lista->ultimo_nodo = NULL;
	}

	lista->cantidad_elementos--;
	return true;
}

void lista_quitar_ultimo_elemento(Lista *lista)
{
	free(lista->primer_nodo->nodo_siguiente);
	lista->ultimo_nodo = lista->primer_nodo;
	lista->primer_nodo->nodo_siguiente = NULL;
}

void lista_eliminar_elemento_en_el_medio(Lista *lista)
{
	Nodo *auxiliar = lista->primer_nodo->nodo_siguiente;
	lista->primer_nodo->nodo_siguiente =
		lista->primer_nodo->nodo_siguiente->nodo_siguiente;
	free(auxiliar);
}

bool lista_quitar_elemento(Lista *lista, size_t posicion,
			   void **elemento_quitado)
{
	if (lista != NULL) {
		if (posicion < lista->cantidad_elementos) {
			// Caso 1: Se quiere borrar el primer elemento.
			if (posicion == 0) {
				return lista_quitar_elemento_en_primera_posicion(
					lista, elemento_quitado);

			} else {
				// Me guarda la direccion del primer nodo.
				Nodo *primer_nodo_copia = lista->primer_nodo;

				// Me deja en el nodo anterior al que quiero eliminar:
				lista_dejar_en_nodo_anterior(lista, posicion);
				if (elemento_quitado != NULL)
					*elemento_quitado =
						lista->primer_nodo
							->nodo_siguiente
							->elemento_actual;

				// Caso 2: se quiere eliminar el ultimo elemento
				if (posicion == lista->cantidad_elementos - 1) {
					lista_quitar_ultimo_elemento(lista);

					// Caso 3: Se quiere eliminar cualquier otro elemento
				} else {
					lista_eliminar_elemento_en_el_medio(
						lista);
				}

				lista->primer_nodo = primer_nodo_copia;

				lista->cantidad_elementos--;
				return true;
			}
		}
	}
	if (elemento_quitado != NULL)
		*elemento_quitado = NULL;
	return false;
}
void lista_destruir(Lista *lista)
{
	if (lista != NULL) {
		if (lista->primer_nodo != NULL) {
			Nodo *a_borrar = lista->primer_nodo;
			// Se va liberando la memoria uno por uno de cada modulo
			while (lista->primer_nodo->nodo_siguiente != NULL) {
				lista->primer_nodo =
					lista->primer_nodo->nodo_siguiente;
				free(a_borrar);
				a_borrar = lista->primer_nodo;
			}
			// Se libera la memoria del ultimo modulo.
			free(a_borrar);
		}
		free(lista);
	}
}

void lista_destruir_todo(Lista *lista, void (*destructor)(void *))
{
	if (lista != NULL) {
		if ((destructor == NULL) | (lista->primer_nodo == NULL)) {
			lista_destruir(lista);
		} else {
			Nodo *a_borrar = lista->primer_nodo;
			// Se va liberando la memoria uno por uno de cada modulo
			while (lista->primer_nodo->nodo_siguiente != NULL) {
				lista->primer_nodo =
					lista->primer_nodo->nodo_siguiente;
				destructor(a_borrar->elemento_actual);
				free(a_borrar);
				a_borrar = lista->primer_nodo;
			}
			// Se libera la memoria del ultimo modulo.
			destructor(a_borrar->elemento_actual);
			free(a_borrar);
			free(lista);
		}
	}
}

bool lista_obtener_elemento(Lista *lista, size_t posicion,
			    void **elemento_encontrado)
{
	if (lista != NULL) {
		if (posicion < lista->cantidad_elementos) {
			// Me guarda la direccion de memoria del primer nodo.
			Nodo *primer_nodo = lista->primer_nodo;

			// Me deja en el nodo que esta en la posicion pedida.
			if (posicion > 0)
				lista_dejar_en_nodo_anterior(lista,
							     posicion + 1);

			if (elemento_encontrado != NULL) {
				*elemento_encontrado =
					lista->primer_nodo->elemento_actual;
			}

			lista->primer_nodo = primer_nodo;
			return true;
		}
	}
	return false;
}

void aplicar_funcion_bool_a_cada_elemento(Lista *lista,
					  bool (*f)(void *, void *), void *ctx,
					  size_t *elementos_iterados)
{
	*elementos_iterados = 0;
	bool continuar = true;
	while (*elementos_iterados < lista->cantidad_elementos && continuar) {
		continuar = f(lista->primer_nodo->elemento_actual, ctx);
		if (lista->primer_nodo->nodo_siguiente != NULL) {
			lista->primer_nodo = lista->primer_nodo->nodo_siguiente;
		}
		(*elementos_iterados)++;
	}
}

size_t lista_iterar_elementos(Lista *lista, bool (*f)(void *, void *),
			      void *ctx)
{
	if (lista != NULL) {
		Nodo *primer_nodo = lista->primer_nodo;

		size_t elementos_iterados = 0;
		aplicar_funcion_bool_a_cada_elemento(lista, f, ctx,
						     &elementos_iterados);

		lista->primer_nodo = primer_nodo;

		return elementos_iterados;
	}
	return 0;
}

Lista_iterador *lista_iterador_crear(Lista *lista)
{
	if (lista != NULL) {
		Lista_iterador *mi_iterador = calloc(1, sizeof(Lista_iterador));
		if (mi_iterador != NULL) {
			mi_iterador->nodo_actual = lista->primer_nodo;
			return mi_iterador;
		}
		free(mi_iterador);
	}
	return NULL;
}

bool lista_iterador_hay_siguiente(Lista_iterador *mi_iterador)
{
	bool existe_siguiente = false;
	if (mi_iterador != NULL) {
		if (mi_iterador->nodo_actual != NULL) {
			existe_siguiente = true;
		}
	}
	return existe_siguiente;
}

void lista_iterador_avanzar(Lista_iterador *mi_iterador)
{
	if (mi_iterador != NULL) {
		if (mi_iterador->nodo_actual != NULL) {
			mi_iterador->nodo_actual =
				mi_iterador->nodo_actual->nodo_siguiente;
		}
	}
}

void *lista_iterador_obtener_elemento_actual(Lista_iterador *mi_iterador)
{
	if (mi_iterador != NULL) {
		if (mi_iterador->nodo_actual != NULL)
			return mi_iterador->nodo_actual->elemento_actual;
	}
	return NULL;
}

void lista_iterador_destruir(Lista_iterador *mi_iterador)
{
	free(mi_iterador);
}
#include "abb.h"
#include "abb_estructura_privada.h"
#include <stdio.h>

typedef enum { INORDEN, PREORDEN, POSTORDEN } TIPO_RECORRIDO;

abb_t *abb_crear(int (*comparador)(void *, void *))
{
	if (comparador != NULL) {
		abb_t *nuevo_abb = calloc(1, sizeof(abb_t));

		if (nuevo_abb != NULL) {
			nuevo_abb->raiz = NULL;
			nuevo_abb->nodos = 0;
			nuevo_abb->comparador = comparador;

			return nuevo_abb;
		}
		free(nuevo_abb);
	}
	return NULL;
}
/**
 * Ambas funciones para destruir el abb llaman a esta funcion al tener procesos
 * muy similares. liberacion_recursiva_nodos se encarga de liberar la memoria
 * asiganada a cada nodo del abb de forma recursiva. Si el destructor es
 * distinto de NULL, se llamara a la funcion destructor para liberar la memoria
 * del elemento.
 */
void liberacion_recursiva_nodos(nodo_t *raiz, void (*destructor)(void *))
{
	if (raiz->der == NULL && raiz->izq == NULL) {
		if (destructor != NULL)
			destructor(raiz->elemento);
		free(raiz);
		return;
	}

	if (raiz->izq != NULL)
		liberacion_recursiva_nodos(raiz->izq, destructor);

	if (raiz->der != NULL)
		liberacion_recursiva_nodos(raiz->der, destructor);

	if (destructor != NULL)
		destructor(raiz->elemento);
	free(raiz);
}

void abb_destruir(abb_t *abb)
{
	if (abb != NULL) {
		if (abb->nodos > 0) {
			liberacion_recursiva_nodos(abb->raiz, NULL);
		}
		free(abb);
	}
}

void abb_destruir_todo(abb_t *abb, void (*destructor)(void *))
{
	if (abb != NULL) {
		if (destructor == NULL)
			abb_destruir(abb);
		else {
			if (abb->nodos > 0)
				liberacion_recursiva_nodos(abb->raiz,
							   destructor);
			free(abb);
		}
	}
}
/*Se viaja a travez de los diferentes nodos del abb de forma recursivamente
tomando en cuenta de que si el nuevo elemento es menor o mayor que el padre, se
ira al hijo izquierdo o el derecho. Una vez que se llegue a un nodo NULL, se
inserta ahi el nuevo elemento.
*/
void insertar_elemento_recursivamente(nodo_t *raiz, nodo_t *nuevo_elemem,
				      int (*comparador)(void *, void *),
				      bool *se_pudo_insertar)
{
	if (comparador(raiz->elemento, nuevo_elemem->elemento) < 0) {
		if (raiz->der == NULL) {
			raiz->der = nuevo_elemem;

			*se_pudo_insertar = true;
		} else {
			insertar_elemento_recursivamente(raiz->der,
							 nuevo_elemem,
							 comparador,
							 se_pudo_insertar);
		}
	} else {
		if (raiz->izq == NULL) {
			raiz->izq = nuevo_elemem;
			*se_pudo_insertar = true;
		} else {
			insertar_elemento_recursivamente(raiz->izq,
							 nuevo_elemem,
							 comparador,
							 se_pudo_insertar);
		}
	}
}
/**
 * Si el abb esta vacio,  se reservara memoria para la raiz, inicializandola con
 * el calloc y se pondra a sus hijos como NULL. Si ese no es el caso, reservo e
 * inicializo memoria para el nuevo elemento y llamo a la funcion
 * insertar_elemento_recursivo la cual va a buscar la posicion en la que deberia
 * de ir el nodo y lo pondra ahi.
 */
bool abb_insertar(abb_t *abb, void *elemento)
{
	bool se_pudo_insertar = false;

	if (abb != NULL) {
		if (abb->nodos == 0) {
			abb->raiz = calloc(1, sizeof(nodo_t));

			if (abb->raiz == NULL) {
				free(abb->raiz);
				return false;
			}

			abb->raiz->elemento = elemento;
			abb->raiz->der = NULL;
			abb->raiz->izq = NULL;
			se_pudo_insertar = true;
		} else {
			nodo_t *nuevo_elemento = calloc(1, sizeof(nodo_t));

			if (nuevo_elemento != NULL) {
				nuevo_elemento->elemento = elemento;
				nuevo_elemento->der = NULL;
				nuevo_elemento->izq = NULL;

				insertar_elemento_recursivamente(
					abb->raiz, nuevo_elemento,
					abb->comparador, &se_pudo_insertar);
			} else {
				free(nuevo_elemento);
			}
		}
	}

	if (se_pudo_insertar)
		abb->nodos++;
	return se_pudo_insertar;
}

/*Se busca a travez del abb teniendo en cuenta de que los elementos menores a
un nodo iran a la izquierda, mientras que los mayores a la derecha.
El caso de recorte es si se llega a una raiz NULL o si si encuentra el
elemento*/
void *busqueda_recursiva(nodo_t *raiz, void *elemento,
			 int (*comparador)(void *, void *))
{
	if (raiz == NULL) {
		return NULL;
	} else {
		int comparacion = comparador(raiz->elemento, elemento);

		if (comparacion == 0)
			return raiz->elemento;
		else if (comparacion > 0)
			return busqueda_recursiva(raiz->izq, elemento,
						  comparador);
		else
			return busqueda_recursiva(raiz->der, elemento,
						  comparador);
	}
}
/*Si el elemento buscado es NULL, directamente devuelvo NULL ya que
independientemente de que lo encuentre o no, tendria que devolver eso. Si ni el
arbol, ni el elemento son NULL, llamo a la funcion busqueda_recursiva que
buscara el elemento de forma recursiva. */
void *abb_obtener(abb_t *abb, void *elemento)
{
	if (abb != NULL && elemento != NULL)
		return busqueda_recursiva(abb->raiz, elemento, abb->comparador);

	return NULL;
}
/*
Si el abb no es nulo, de devuelve el campo "nodos" el cual tiene la cantidad de
elementos del abb.
*/
size_t abb_cantidad(abb_t *abb)
{
	size_t cantidad = 0;
	if (abb != NULL)
		cantidad = abb->nodos;

	return cantidad;
}

/*
Esta funcion me devuelve el nodo predecesor al nodo que se va a eliminar.
Tambien, se devuelve mediante referencia el padre de dicho nodo si es que
existe. No se devuelve si el padre del predecesor es el nodo a borrar (que solo
pasa si es la raiz).
*/
nodo_t *buscar_nodo_predecesor_recursivamente(nodo_t *raiz,
					      nodo_t **padre_de_predecesor,
					      bool *padre_tiene_que_ser_borrado)
{
	if (raiz->der != NULL) {
		if (raiz->der->der == NULL) {
			*padre_de_predecesor = raiz;
			*padre_tiene_que_ser_borrado = false;
			return raiz->der;
		} else
			return buscar_nodo_predecesor_recursivamente(
				raiz->der, padre_de_predecesor,
				padre_tiene_que_ser_borrado);
	}
	return raiz;
}
/*
Funcion que se usa cuando se necesita eliminar un nodo con 2 hijos.
Con la funcion auxiliar buscar_nodo_predecesor_recursivamente se busca tanto el
predecesor como su padre, aunque primero asumo que el padre es el nodo a
eliminar. Si es que el padre del predecesor no es el nodo a borrar, entonces
este apuntara como nodo derecho al izquierdo del predecesor y, como nodo
izquierdo, el nodo izquierdo del nodo a borrar. Posteriormente, el nodo derecho
del predecesor apuntara al que originalmente el nodo derecho del nodo a borrar
apuntaba y se liberara la memoria de dicho nodo. Se finaliza devolviendo el
predecesor.
*/
nodo_t *quitar_nodo_con_2_hijos(nodo_t *nodo_a_eliminar, bool *se_elimino)
{
	nodo_t *padre_de_predecesor = nodo_a_eliminar;
	bool padre_tiene_que_ser_borrado = true;
	nodo_t *predecesor = buscar_nodo_predecesor_recursivamente(
		nodo_a_eliminar->izq, &padre_de_predecesor,
		&padre_tiene_que_ser_borrado);

	if (!padre_tiene_que_ser_borrado) {
		padre_de_predecesor->der = predecesor->izq;
		predecesor->izq = nodo_a_eliminar->izq;
	}
	predecesor->der = nodo_a_eliminar->der;
	*se_elimino = true;
	free(nodo_a_eliminar);
	return predecesor;
}

/*
funcion auxiliar que borra el nodo a eliminar. El hijo derecho tomara si lugar,
y, si no tiene hijo derecho, el izquierdo. Si no tiene hijos (es nodo hoja),
entonces se devolvera NULL
*/
nodo_t *quitar_nodo_con_0o1_hijo(nodo_t *nodo_a_eliminar, bool *se_elimino)
{
	nodo_t *hijo_no_nulo = nodo_a_eliminar->der;
	if (hijo_no_nulo == NULL)
		hijo_no_nulo = nodo_a_eliminar->izq;
	free(nodo_a_eliminar);
	*se_elimino = true;
	return hijo_no_nulo;
}
/*
Busca por todo el arbol hasta encontrar un nodo cuyo elemento sea igual al
buscado. Dependiendo de su cantidad de hijos, se llamara a la funcion
correspondiente para eliminarlo y que asegure que el arbol siga funcionando. Si
no se encuentra, simplemente se devuelve, NULL, caso contrario, se devuelve el
nodo. Los 3 casos posibles que pueden aparecer en esta funcion son: Caso 1: No
se encuentra el elemento a eliminar dentro del arbol. Caso 2: Encuentro el
elemento a eliminar, pero el nodo en el que esta tiene 2 hijos. Caso 3: Se
encuentra el elemento a eliminar, pero el nodo en el que esta tiene 0 o 1 hijo.
*/
nodo_t *quitar_elem_recursivamente(nodo_t *raiz, void *elemento,
				   int (*comparador)(void *, void *),
				   bool *se_elimino, void **encontrado)
{
	// Caso 1:
	if (raiz == NULL) {
		*se_elimino = false;
		if (encontrado != NULL) {
			*encontrado = NULL;
		}
		return NULL;
	} else {
		int comparacion = comparador(raiz->elemento, elemento);

		if (comparacion == 0) {
			if (encontrado != NULL) {
				*encontrado = raiz->elemento;
			}

			// Caso 2:
			if (raiz->der != NULL && raiz->izq != NULL) {
				nodo_t *raiz_a_retornar =
					quitar_nodo_con_2_hijos(raiz,
								se_elimino);
				return raiz_a_retornar;

				// Caso 3:
			} else {
				return quitar_nodo_con_0o1_hijo(raiz,
								se_elimino);
			}
		} else if (comparacion > 0) {
			raiz->izq = quitar_elem_recursivamente(
				raiz->izq, elemento, comparador, se_elimino,
				encontrado);
		} else {
			raiz->der = quitar_elem_recursivamente(
				raiz->der, elemento, comparador, se_elimino,
				encontrado);
		}
		return raiz;
	}
}

/*
Funcion encargada del caso en el que se quiera eliminar el unico nodo de un
arbol.
*/
bool eliminar_unico_nodo(abb_t *abb, void *buscado, void **encontrado)
{
	if (abb->comparador(abb->raiz->elemento, buscado) == 0) {
		*encontrado = abb->raiz->elemento;
		free(abb->raiz);
		abb->raiz = NULL;
		return true;
	}
	return false;
}
/*
Si el abb no es nulo y no esta vacio, se enviara a alguna de las 2 funciones
auxiliares para eliminar el nodo.
*/
bool abb_quitar(abb_t *abb, void *buscado, void **encontrado)
{
	bool se_elimino_elemento = false;

	if (abb != NULL) {
		if (abb->nodos > 0) {
			if (abb->nodos == 1)
				se_elimino_elemento = eliminar_unico_nodo(
					abb, buscado, encontrado);
			else {
				abb->raiz = quitar_elem_recursivamente(
					abb->raiz, buscado, abb->comparador,
					&se_elimino_elemento, encontrado);
			}

			if (se_elimino_elemento)
				abb->nodos--;
		}
	}

	return se_elimino_elemento;
}

/*
Las siguientes 3 funciones son las encargadas de recorrer el arbol en sus 3
ordenes respectivos. Se recorre de forma recursiva y aplican la funcion f a cada
nodo del arbol.
*/

size_t recorrer_inorden(nodo_t *raiz, bool (*f)(void *, void *), void *ctx,
			bool *continuar)
{
	if (raiz == NULL || !*continuar)
		return 0;

	size_t cantidad = 0;

	cantidad += recorrer_inorden(raiz->izq, f, ctx, continuar);
	if (!*continuar)
		return cantidad;

	*continuar = f(raiz->elemento, ctx);
	cantidad++;
	if (!*continuar)
		return cantidad;

	cantidad += recorrer_inorden(raiz->der, f, ctx, continuar);

	return cantidad;
}

size_t recorrer_preorden(nodo_t *raiz, bool (*f)(void *, void *), void *ctx,
			 bool *continuar)
{
	if (raiz == NULL || !*continuar)
		return 0;

	size_t cantidad = 0;

	*continuar = f(raiz->elemento, ctx);
	cantidad++;
	if (!*continuar)
		return cantidad;

	cantidad += recorrer_preorden(raiz->izq, f, ctx, continuar);
	if (!*continuar)
		return cantidad;

	cantidad += recorrer_preorden(raiz->der, f, ctx, continuar);

	return cantidad;
}

size_t recorrer_postorden(nodo_t *raiz, bool (*f)(void *, void *), void *ctx,
			  bool *continuar)
{
	if (raiz == NULL || !*continuar)
		return 0;

	size_t cantidad = 0;

	cantidad += recorrer_postorden(raiz->izq, f, ctx, continuar);
	if (!*continuar)
		return cantidad;

	cantidad += recorrer_postorden(raiz->der, f, ctx, continuar);
	if (!*continuar)
		return cantidad;

	*continuar = f(raiz->elemento, ctx);
	cantidad++;

	return cantidad;
}

size_t abb_iterar_inorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (abb == NULL || f == NULL) {
		return 0;
	}
	bool continuar = true;
	return recorrer_inorden(abb->raiz, f, ctx, &continuar);
}

size_t abb_iterar_preorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (abb == NULL || f == NULL) {
		return 0;
	}
	bool continuar = true;
	return recorrer_preorden(abb->raiz, f, ctx, &continuar);
}

size_t abb_iterar_postorden(abb_t *abb, bool (*f)(void *, void *), void *ctx)
{
	if (abb == NULL || f == NULL) {
		return 0;
	}
	bool continuar = true;
	return recorrer_postorden(abb->raiz, f, ctx, &continuar);
}

/*
Las proximas 3 funciones auxiliares se encargan de rellenar el vector pasado por
parametro segun el orden respectivo de dicha funcion. Se rellean de forma
recursiva.
*/
void rellenar_vector_inorden(nodo_t *raiz, void **vector, size_t tamaño,
			     size_t *posicion_actual)
{
	if (raiz == NULL || tamaño <= *posicion_actual)
		return;

	rellenar_vector_inorden(raiz->izq, vector, tamaño, posicion_actual);

	if (tamaño <= *posicion_actual)
		return;

	vector[*posicion_actual] = raiz->elemento;
	(*posicion_actual)++;

	rellenar_vector_inorden(raiz->der, vector, tamaño, posicion_actual);
}

void rellenar_vector_preorden(nodo_t *raiz, void **vector, size_t tamaño,
			      size_t *posicion_actual)
{
	if (raiz == NULL || tamaño <= *posicion_actual)
		return;

	vector[*posicion_actual] = raiz->elemento;
	(*posicion_actual)++;

	rellenar_vector_preorden(raiz->izq, vector, tamaño, posicion_actual);
	rellenar_vector_preorden(raiz->der, vector, tamaño, posicion_actual);
}

void rellenar_vector_postorden(nodo_t *raiz, void **vector, size_t tamaño,
			       size_t *posicion_actual)
{
	if (raiz == NULL || tamaño <= *posicion_actual)
		return;

	rellenar_vector_postorden(raiz->izq, vector, tamaño, posicion_actual);
	rellenar_vector_postorden(raiz->der, vector, tamaño, posicion_actual);

	if (tamaño <= *posicion_actual)
		return;

	vector[*posicion_actual] = raiz->elemento;
	(*posicion_actual)++;
}

size_t abb_vectorizar_inorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (abb == NULL || vector == NULL || tamaño == 0) {
		return 0;
	}

	size_t posicion_actual = 0;
	rellenar_vector_inorden(abb->raiz, vector, tamaño, &posicion_actual);
	return posicion_actual;
}

size_t abb_vectorizar_preorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (abb == NULL || vector == NULL || tamaño == 0) {
		return 0;
	}

	size_t posicion_actual = 0;
	rellenar_vector_preorden(abb->raiz, vector, tamaño, &posicion_actual);
	return posicion_actual;
}

size_t abb_vectorizar_postorden(abb_t *abb, void **vector, size_t tamaño)
{
	if (abb == NULL || vector == NULL || tamaño == 0) {
		return 0;
	}
	size_t posicion_actual = 0;
	rellenar_vector_postorden(abb->raiz, vector, tamaño, &posicion_actual);
	return posicion_actual;
}
#include "hash.h"
#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash {
	size_t capacidad;
	size_t cant_elem;
	Lista **vector;
	size_t (*hash_func)(const char *, size_t);
};
typedef struct {
	char *clave;
	void *elemento;
} Par_t;

typedef struct {
	Par_t **pares;
	size_t cant_pares;
} Tabla_t;

unsigned long hashear(const char *clave, size_t capacidad)
{
	unsigned long hash = 5381;
	unsigned char i;

	while ((i = (unsigned char)*clave)) {
		clave++;
		hash = ((hash << 5) + hash) + i;
	}

	return hash % capacidad;
}

hash_t *hash_crear(size_t capacidad_inicial)
{
	hash_t *nuevo_hash = calloc(1, sizeof(hash_t));
	if (nuevo_hash == NULL)
		return NULL;

	if (capacidad_inicial < 3)
		capacidad_inicial = 3;

	nuevo_hash->vector = calloc(capacidad_inicial, sizeof(Lista *));
	if (nuevo_hash->vector == NULL) {
		free(nuevo_hash);
		return NULL;
	}

	for (int x = 0; x < capacidad_inicial; x++) {
		nuevo_hash->vector[x] = lista_crear();

		if (nuevo_hash->vector[x] == NULL) {
			for (int i = 0; i < x; i++) {
				lista_destruir(nuevo_hash->vector[x]);
			}
			free(nuevo_hash->vector);
			free(nuevo_hash);
			return NULL;
		}
	}

	nuevo_hash->capacidad = capacidad_inicial;
	nuevo_hash->cant_elem = 0;
	nuevo_hash->hash_func = hashear;
	return nuevo_hash;
}

size_t hash_cantidad(hash_t *hash)
{
	if (hash == NULL)
		return 0;
	else
		return hash->cant_elem;
}
// Funcion de comparacion para lista_buscar_elemento que me dira si encuentra la
// clave.
int clave_en_lista(void *clave_busc_void, void *par_clave_valor)
{
	Par_t *par = (Par_t *)par_clave_valor;
	return strcmp(par->clave, (char *)clave_busc_void);
}

// Funcion para lista_iterar_elementos que me dira la posicion del elemento.
bool posicion_par_a_borrar(void *par_clave_valor, void *clave_busc_void)
{
	Par_t *par = (Par_t *)par_clave_valor;
	if (strcmp(par->clave, (char *)clave_busc_void) != 0)
		return true;
	return false;
}

/*
Me devuelve la lista en la que esta la clave que busco y su posicion.
*/
Lista *hash_busqueda_elem_y_pos(hash_t *hash, char *clave, size_t *pos)
{
	size_t posicion_vector = hash->hash_func(clave, hash->capacidad);
	Lista *lista_de_hash = hash->vector[posicion_vector];

	if (lista_buscar_elemento(lista_de_hash, clave, clave_en_lista) !=
	    NULL) {
		*pos = lista_iterar_elementos(lista_de_hash,
					      posicion_par_a_borrar, clave) -
		       1;
		return lista_de_hash;
	}
	return NULL;
}

void *hash_buscar(hash_t *hash, char *clave)
{
	if (hash == NULL || clave == NULL)
		return NULL;

	size_t posicion_vector = hash->hash_func(clave, hash->capacidad);
	Lista *lista_de_hash = hash->vector[posicion_vector];
	Par_t *encontrado = (Par_t *)lista_buscar_elemento(lista_de_hash, clave,
							   clave_en_lista);

	if (encontrado != NULL)
		return encontrado->elemento;
	return NULL;
}

bool hash_contiene(hash_t *hash, char *clave)
{
	if (hash == NULL || clave == NULL)
		return false;
	size_t pos = 0;
	if (hash_busqueda_elem_y_pos(hash, clave, &pos) != NULL)
		return true;
	return false;
}

/*
Funcion hecha para pasarselo a lista_iterar_elemento .
Me rellena mi estructura Tabla_t con todos los pares clave-valor que tiene el
hash.
*/
bool rellenar_tabla(void *par_void, void *pares_hash_void)
{
	Tabla_t *pares_hash = (Tabla_t *)pares_hash_void;
	Par_t *par = (Par_t *)par_void;
	pares_hash->pares[pares_hash->cant_pares] = par;
	pares_hash->cant_pares++;

	return true;
}

/*Funcion para rellenar estructura tabla_t con pares clave-valor*/
Tabla_t *funcion_rellenar_tabla_t(hash_t *hash)
{
	Tabla_t *pares_hash = calloc(1, sizeof(Tabla_t));
	pares_hash->pares = calloc(hash->cant_elem, sizeof(Par_t *));
	pares_hash->cant_pares = 0;
	size_t i = 0;
	size_t elementos = 0;

	while (i < hash->capacidad && elementos <= hash->cant_elem) {
		elementos += lista_iterar_elementos(hash->vector[i],
						    rellenar_tabla, pares_hash);
		i++;
	}

	return pares_hash;
}

size_t hash_iterar(hash_t *hash, bool (*f)(char *, void *, void *), void *ctx)
{
	if (hash == NULL || f == NULL)
		return 0;
	Tabla_t *pares_hash = funcion_rellenar_tabla_t(hash);

	size_t i = 0;
	bool continuar = true;
	while (i < hash->cant_elem && continuar) {
		continuar = f(pares_hash->pares[i]->clave,
			      pares_hash->pares[i]->elemento, ctx);
		i++;
	}
	free(pares_hash->pares);
	free(pares_hash);

	return i;
}

void *hash_quitar(hash_t *hash, char *clave)
{
	if (hash == NULL || clave == NULL)
		return NULL;

	size_t pos = 0;
	Lista *lista_de_hash = hash_busqueda_elem_y_pos(hash, clave, &pos);
	void **elemento_encontrado = calloc(1, sizeof(void *));
	void *devolver = NULL;

	if (lista_de_hash != NULL) {
		lista_quitar_elemento(lista_de_hash, pos, elemento_encontrado);
		Par_t *par_encontrado = (Par_t *)(*elemento_encontrado);
		devolver = par_encontrado->elemento;
		free(par_encontrado->clave);
		free(par_encontrado);

		hash->cant_elem--;
	}

	free(elemento_encontrado);
	return devolver;
}

/*
Funciones para los hash_destruir.
*/
void destruir_dic(void *par_clave_valor)
{
	free(((Par_t *)par_clave_valor)->clave);
	free(par_clave_valor);
}

// funcion hecha para modularizar, ya que la termine usando mucho.
void liberar_vector_de_listas(Lista **vectorDeListas, size_t capacidad,
			      void (*f)(void *))
{
	for (int i = 0; i < capacidad; i++)
		lista_destruir_todo(vectorDeListas[i], f);
	free(vectorDeListas);
}

void hash_destruir(hash_t *hash)
{
	if (hash == NULL)
		return;
	liberar_vector_de_listas(hash->vector, hash->capacidad, destruir_dic);
	free(hash);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (hash == NULL)
		return;

	if (destructor == NULL) {
		hash_destruir(hash);
		return;
	}

	Tabla_t *pares_hash = funcion_rellenar_tabla_t(hash);

	for (int i = 0; i < hash->cant_elem; i++) {
		destructor(pares_hash->pares[i]->elemento);
	}
	free(pares_hash->pares);
	free(pares_hash);
	hash_destruir(hash);
}

bool se_rehashea(hash_t *hash, Lista *lista)
{
	float factor_de_carga = 0.75;
	float capacidad = (float)(hash->capacidad);
	float cantidad_elem = (float)(hash->cant_elem);

	if ((factor_de_carga <= cantidad_elem / capacidad) ||
	    lista_cantidad_elementos(lista) >= 4)
		return true;

	return false;
}

void pasar_elementos_a_nuevo_vector(hash_t *hash, Lista **new_vec,
				    Tabla_t *pares_en_hash)
{
	// Relleno la tabla con los pares clave-valor del hash.
	size_t iteraciones = 0;
	int x = 0;
	while (x < hash->capacidad) {
		iteraciones += lista_iterar_elementos(
			hash->vector[x], rellenar_tabla, pares_en_hash);
		x++;
	}

	// Añado los pares al nuevo vector.
	if (iteraciones == hash->cant_elem) {
		for (int i = 0; i < hash->cant_elem; i++) {
			size_t posicion =
				hash->hash_func(pares_en_hash->pares[i]->clave,
						hash->capacidad * 2);
			lista_agregar_al_final(new_vec[posicion],
					       pares_en_hash->pares[i]);
		}
	}
}

Lista **agrandar_capacidad(hash_t *hash)
{
	// Creo el nuevo vector de listas.
	Lista **new_vec = calloc(hash->capacidad * 2, sizeof(Lista *));
	if (new_vec == NULL)
		return hash->vector;

	for (size_t x = 0; x < hash->capacidad * 2; x++) {
		new_vec[x] = lista_crear();

		if (new_vec[x] == NULL) {
			liberar_vector_de_listas(new_vec, x, NULL);
			return hash->vector;
		}
	}

	// Creo una tabla para almacenar todos los pares clave-valor
	Tabla_t *pares_en_hash = calloc(hash->capacidad, sizeof(Tabla_t));
	if (pares_en_hash == NULL) {
		liberar_vector_de_listas(new_vec, hash->capacidad * 2, NULL);
		return hash->vector;
	}
	pares_en_hash->pares = calloc(hash->cant_elem, sizeof(Par_t *));
	if (pares_en_hash->pares == NULL) {
		free(pares_en_hash);
		liberar_vector_de_listas(new_vec, hash->capacidad * 2, NULL);
		return hash->vector;
	}

	// paso los elementos del vector original al nuevo vector con el doble de
	// capcidad.
	pasar_elementos_a_nuevo_vector(hash, new_vec, pares_en_hash);

	// Libero el vector original y la tabla (ahora que ya se la pase a la lista).
	liberar_vector_de_listas(hash->vector, hash->capacidad, NULL);
	free(pares_en_hash->pares);
	free(pares_en_hash);

	hash->capacidad *= 2;
	return new_vec;
}

Par_t *crear_par_a_insertar(char *clave, void *valor)
{
	char *copia_clave = malloc(sizeof(char) * strlen(clave) + 1);
	if (copia_clave == NULL)
		return NULL;
	strcpy(copia_clave, clave);

	Par_t *clave_valor = calloc(1, sizeof(Par_t));
	if (clave == NULL) {
		free(copia_clave);
		return NULL;
	}
	clave_valor->clave = copia_clave;
	clave_valor->elemento = valor;
	return clave_valor;
}

bool quitar_elem_en_hash(hash_t *hash, Lista *lista_de_hash, size_t pos,
			 void **encontrado, Par_t *clave_valor)
{
	// Creo un auxiliar para liberar la memoria par clave-valor en caso de que
	// encontrado sea NULL.
	void **encontrado_en_lista = calloc(1, sizeof(void *));
	if (encontrado_en_lista == NULL)
		return false;
	bool se_quito_par =
		lista_quitar_elemento(lista_de_hash, pos, encontrado_en_lista);

	if (se_quito_par == false) {
		free(clave_valor->clave);
		free(clave_valor);
		free(encontrado_en_lista);
		return false;
	}

	hash->cant_elem--;
	// Libero la memoria de la copia de la clave y del Par_t*, devolviendo solo el
	// valor de la clave.
	if (encontrado != NULL) {
		Par_t *par_encontrado = (Par_t *)*encontrado_en_lista;
		*encontrado = par_encontrado->elemento;
		free(par_encontrado->clave);
		free(par_encontrado);

	} else {
		Par_t *par_encontrado = (Par_t *)*encontrado_en_lista;
		free(par_encontrado->clave);
		free(par_encontrado);
	}

	free(encontrado_en_lista);
	return true;
}

bool hash_insertar(hash_t *hash, char *clave, void *valor, void **encontrado)
{
	if (hash == NULL || clave == NULL)
		return false;

	if (encontrado != NULL)
		*encontrado = NULL;
	size_t posicion_vector = hash->hash_func(clave, hash->capacidad);

	// Me fijo si hay que rehasehar y creo un nuevo vector con el doble de
	// capacidad si es asi.
	if (se_rehashea(hash, hash->vector[posicion_vector])) {
		Lista **new_vector = agrandar_capacidad(hash);
		if (new_vector == NULL)
			return false;
		hash->vector = new_vector;
		// Vuelvo a buscar en que lista del vector de listas ira la clave, ahora que
		// se duplico la capacidad.
		posicion_vector = hash->hash_func(clave, hash->capacidad);
	}

	// guardo las variables a agregar en el hash.
	Par_t *clave_valor = crear_par_a_insertar(clave, valor);
	if (clave_valor == NULL)
		return false;

	// Si la clave ya estaba en el hash, me devolvera en que lista y en que
	// posicion pare removerla. Si no esta, me devolvera NULL. Si se me devuelve
	// NULL, me fijo en que lista deberia ir la clave.
	size_t pos_en_lista = 0;
	Lista *lista_de_hash =
		hash_busqueda_elem_y_pos(hash, clave, &pos_en_lista);

	if (lista_de_hash != NULL) {
		// Si estaba en el hash, quito el par clave-valor del hash.
		if (quitar_elem_en_hash(hash, lista_de_hash, pos_en_lista,
					encontrado, clave_valor) == false)
			return false;
	} else
		lista_de_hash = hash->vector[posicion_vector];

	// Finalmente agrego al hash el nuevo par clave-valor.
	bool se_pudo_agregar =
		lista_agregar_al_final(lista_de_hash, clave_valor);
	if (se_pudo_agregar) {
		hash->cant_elem++;
		return true;
	}

	free(clave_valor->clave);
	free(clave_valor);
	return false;
}
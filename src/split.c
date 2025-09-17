#include "split.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Me devuelve la cantidad de separadores y sus posiciones
void posiciones_and_cantidad_separadadores(const char *string,
					   int posiciones_separador[],
					   int *separadores_cantidad,
					   char separador)
{
	int posicion_string_actual = 0;
	int x = 1;

	while (string[posicion_string_actual] != '\0') {
		if (string[posicion_string_actual] == separador) {
			posiciones_separador[x] = posicion_string_actual;
			x++;
			(*separadores_cantidad) += 1;
		}
		posicion_string_actual++;
	}

	if (posiciones_separador[x - 1] != strlen(string)) {
		posiciones_separador[x] = (int)strlen(string);
	}
}

// Rellena el los strings de divido->string con las palabras de *string
// Tuve que llamarlo asi por que si no me daba error de LINTER
// Y me medio rendi. Su nombre original es void llenar_string()
void rellenar_string_de_struct_Partes(struct Partes *dividido,
				      int *posiciones_separadores,
				      const char *string)
{
	int i = 0;
	while (i < dividido->cantidad) {
		int desde = posiciones_separadores[i];
		int hasta = posiciones_separadores[i + 1];

		dividido->string[i] =
			calloc((size_t)(hasta - desde) + 1, sizeof(char));
		if (dividido->string[i] == NULL) {
			for (int j = 0; j < i; j++) {
				free(dividido->string[j]);
			}
			free(dividido->string);
			free(dividido);
			return;

		} else {
			strncpy(dividido->string[i], string + desde + 1,
				(size_t)(hasta - desde - 1));
			dividido->string[i][hasta - desde] = '\0';
			i++;
		}
	}
}

struct Partes *dividir_string(const char *string, char separador)
{
	struct Partes *dividido = calloc(1, sizeof(struct Partes));
	if (dividido == NULL) {
		return NULL;
	} else {
		dividido->cantidad = 0;
		dividido->string = NULL;

		if (string != NULL) {
			int largo_string = (int)strlen(string);
			int posiciones_separador[largo_string + 1];
			for (int x = 0; x < largo_string + 1; x++) {
				posiciones_separador[x] = -1;
			}
			int separadores_cantidad = 0;
			posiciones_and_cantidad_separadadores(
				string, posiciones_separador,
				&separadores_cantidad, separador);

			dividido->cantidad = (size_t)(separadores_cantidad + 1);
			dividido->string =
				calloc(dividido->cantidad, sizeof(char *));
			if (dividido->string == NULL) {
				free(dividido);
				return NULL;
			}

			rellenar_string_de_struct_Partes(
				dividido, posiciones_separador, string);
		}

		return dividido;
	}
}

void liberar_partes(struct Partes *dividido)
{
	if (dividido != NULL) {
		for (size_t i = 0; i < dividido->cantidad; i++) {
			free(dividido->string[i]);
		}
		free(dividido->string);
		free(dividido);
	}
}
#include "csv.h"
#include "split.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct archivo_csv {
	FILE *archivo;
	char separador;
	struct Partes *fila;
};

struct archivo_csv *abrir_archivo_csv(const char *nombre_archivo,
				      char separador)
{
	struct archivo_csv *mi_archivo = calloc(1, sizeof(struct archivo_csv));
	if (mi_archivo != NULL) {
		mi_archivo->archivo = fopen(nombre_archivo, "r");
		if (mi_archivo->archivo == NULL) {
			free(mi_archivo);
			return NULL;
		}
		mi_archivo->separador = separador;
	}
	return mi_archivo;
}

size_t leer_linea_csv(struct archivo_csv *archivo, size_t columnas,
		      bool (*funciones[])(const char *, void *), void *ctx[])
{
	size_t columnas_leidas = 0;

	if (archivo != NULL) {
		// Asumo que la fila no tendra 499 caracteres.
		char fila_csv[500] = { 0 };
		// Me da toda la fila del archivo.csv como string
		if (fgets(fila_csv, sizeof(fila_csv) / sizeof(char),
			  archivo->archivo) != NULL) {
			// divide el string en varias partes dependiendo de donde este el
			// separador
			archivo->fila =
				dividir_string(fila_csv, archivo->separador);
			if (archivo->fila != NULL &&
			    columnas <= archivo->fila->cantidad) {
				for (columnas_leidas = 0;
				     columnas_leidas < columnas;
				     columnas_leidas++) {
					if (funciones[columnas_leidas] ==
					    NULL) {
						liberar_partes(archivo->fila);
						return columnas_leidas;
					} else {
						funciones[columnas_leidas](
							archivo->fila->string
								[columnas_leidas],
							ctx[columnas_leidas]);
					}
				}
			}
			liberar_partes(archivo->fila);
		}
	}
	return columnas_leidas++;
}
void cerrar_archivo_csv(struct archivo_csv *archivo)
{
	fclose(archivo->archivo);
	free(archivo);
}
#ifndef FUNCIONES_ESTUDIANTE_H_INCLUDED
#define FUNCIONES_ESTUDIANTE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estructuras.h"
#include "constantes.h"

// funciones padres
char** obtenerArgumentosModificaciones(char** argv, int cantModificaciones);
int cargarMetadatos(FILE* archivo, t_metadata* metadatos);
int modificarImagen(const char* nombreArchivo, char** modificaciones, int cantModificaciones);

// funciones de modificacion
int aplicarNegativo(FILE* archivoEntrada, const t_metadata* metadatos);
int aplicarEscalaDeGrises(FILE* archivoEntrada, const t_metadata* metadatos);
int aplicarContraste(FILE* archivoEntrada, const t_metadata* metadatos, double factorContraste, const char* nombreArchivoSalida);
int aplicarTonalidad(FILE* archivoEntrada, const t_metadata* metadatos, int colorAIncrementar, const char* nombreArchivoSalida);
int aplicarRecortar(FILE* archivoEntrada, const t_metadata* metadatos);
int aplicarRotar(FILE* archivoEntrada, const t_metadata* metadatos, bool rotarDerecha, const char* nombreArchivoSalida);
int aplicarEspejo(FILE* archivoEntrada, const t_metadata* metadatos);

// funciones de ayuda
int copiarEncabezado(FILE* archivoSalida, const t_metadata* metadatos);
void leerEscribirRelleno(FILE* archivoEntrada, FILE* archivoSalida, int relleno);

int solucion(int argc, char* argv[]);

#endif // FUNCIONES_ESTUDIANTE_H_INCLUDED

#ifndef FUNCIONES_ESTUDIANTE_H_INCLUDED
#define FUNCIONES_ESTUDIANTE_H_INCLUDED

#include "estructuras.h"
#include "constantes.h"

char** getModificationsArgv(int argc, char* argv[], int* count);
unsigned char* loadBMP(const char* filename, t_metadata* metadata);
void saveBMP(const char* filename, t_metadata* metadata, unsigned char* data);
void applyNegative(t_metadata* metadata, unsigned char* data);
void solucion(int argc, char* argv[]);

#endif // FUNCIONES_ESTUDIANTE_H_INCLUDED

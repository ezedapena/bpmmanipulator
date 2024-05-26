/*
    Integrantes del grupo. En caso de ser un grupo de dos integrantes, no completar el último campo.
    Si alguno de los integrantes del grupo dejara la materia, completar de todos modos sus datos, aclarando que no entrega.
    -----------------
    Apellido: Dapena
    Nombre: Ezequiel
    DNI: 42.725.436
    Entrega:
    -----------------
    Apellido: Brance
    Nombre: Alan Emanuel
    DNI: 36.847.607
    Entrega:
    -----------------
    Apellido:
    Nombre:
    DNI:
    Entrega:
    -----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include "constantes.h"
#include "funciones_estudiante.h"

char** obtenerArgumentosModificaciones(int argc, char* argv[], int* cantidad)
{
    *cantidad = argc - 2;
    char** modificaciones = (char**)malloc((*cantidad) * sizeof(char*));

    for (int i = 1; i < argc - 1; i++)
    {
        modificaciones[i - 1] = argv[i];
    }

    return modificaciones;
}

unsigned char* cargarBMP(const char* nombreArchivo, t_metadata* metadatos)
{
    FILE* archivo = fopen(nombreArchivo, "rb"); // abrir en modo binario
    if (!archivo)
    {
        printf("No se pudo abrir el archivo %s.\n", nombreArchivo);
        return NULL;
    }

    // Leer el encabezado del archivo BMP
    unsigned short tipoArchivo;
    fread(&tipoArchivo, sizeof(unsigned short), 1, archivo);
    if (tipoArchivo != 0x4D42)   // 'BM' en little-endian
    {
        printf("El archivo no es un BMP válido.\n");
        fclose(archivo);
        return NULL;
    }

    // Leer metadatos relevantes
    fread(&metadatos->tamArchivo, sizeof(unsigned int), 1, archivo); // almacenamos en metadatos->tamArchivo el tamaño del archivo BMP
    fseek(archivo, 4, SEEK_CUR); // Saltar campos reservados (4 bytes)
    fread(&metadatos->tamEncabezado, sizeof(unsigned int), 1, archivo); // almacenamos en metadatos->tamEncabezado el tamaño del offset

    // Leer encabezado de información BMP
    unsigned int tamEncabezadoInfo;
    fread(&tamEncabezadoInfo, sizeof(unsigned int), 1, archivo); // tamaño del encabezado de información
    if (tamEncabezadoInfo != 40)
    {
        printf("El encabezado de información BMP no tiene el tamaño esperado.\n");
        fclose(archivo);
        return NULL;
    }

    fread(&metadatos->ancho, sizeof(unsigned int), 1, archivo);
    fread(&metadatos->alto, sizeof(unsigned int), 1, archivo);

    unsigned short planos;
    fread(&planos, sizeof(unsigned short), 1, archivo);
    if (planos != 1)
    {
        printf("El archivo BMP no tiene el número esperado de planos de color.\n");
        fclose(archivo);
        return NULL;
    }

    fread(&metadatos->profundidad, sizeof(unsigned short), 1, archivo);
    if (metadatos->profundidad != 24)
    {
        printf("El archivo BMP no es de 24 bits.\n");
        fclose(archivo);
        return NULL;
    }

    // Saltar campos restantes del encabezado de información que no necesitamos (24 bytes)
    fseek(archivo, 24, SEEK_CUR);

    // Leer datos de la imagen
    unsigned int tamImagen = metadatos->tamArchivo - metadatos->tamEncabezado;
    unsigned char* datos = (unsigned char*)malloc(tamImagen);
    if (datos == NULL)
    {
        printf("Error al asignar memoria para los datos de la imagen.\n");
        fclose(archivo);
        return NULL;
    }

    fseek(archivo, metadatos->tamEncabezado, SEEK_SET); // SEEK_SET indica que la referencia es la posición actual del puntero del archivo
    fread(datos, 1, tamImagen, archivo);
    fclose(archivo);

    printf("Archivo %s cargado correctamente.\n", nombreArchivo);
    printf("Tamaño del archivo: %u bytes\n", metadatos->tamArchivo);
    printf("Ancho de la imagen: %u pixels\n", metadatos->ancho);
    printf("Alto de la imagen: %u pixels\n", metadatos->alto);

    return datos;
}

void guardarBMP(const char* nombreArchivo, t_metadata* metadatos, unsigned char* datos)
{
    FILE* archivo = fopen(nombreArchivo, "wb");
    if (!archivo)
    {
        printf("No se pudo crear el archivo %s.\n", nombreArchivo);
        return;
    }

    // Escribir el tipo de archivo BMP ('BM' en little-endian)
    unsigned short tipoArchivo = 0x4D42;
    fwrite(&tipoArchivo, sizeof(unsigned short), 1, archivo);

    // Escribir el tamaño total del archivo BMP
    fwrite(&metadatos->tamArchivo, sizeof(unsigned int), 1, archivo);

    // Escribir los campos reservados (4 bytes, en este caso todos ceros)
    unsigned int reservado = 0;
    fwrite(&reservado, sizeof(unsigned int), 1, archivo);

    // Escribir el tamaño del offset donde comienzan los datos de la imagen
    fwrite(&metadatos->tamEncabezado, sizeof(unsigned int), 1, archivo);

    // Escribir el tamaño del encabezado de información (40 bytes)
    unsigned int tamEncabezadoInfo = 40;
    fwrite(&tamEncabezadoInfo, sizeof(unsigned int), 1, archivo);

    // Escribir el ancho de la imagen
    fwrite(&metadatos->ancho, sizeof(unsigned int), 1, archivo);

    // Escribir el alto de la imagen
    fwrite(&metadatos->alto, sizeof(unsigned int), 1, archivo);

    // Escribir el número de planos de color (1 en BMP)
    unsigned short planos = 1;
    fwrite(&planos, sizeof(unsigned short), 1, archivo);

    // Escribir la profundidad de color (24 bits para BMP)
    fwrite(&metadatos->profundidad, sizeof(unsigned short), 1, archivo);

    // Escribir el método de compresión (0 para sin compresión)
    unsigned int compresion = 0;
    fwrite(&compresion, sizeof(unsigned int), 1, archivo);

    // Escribir el tamaño de los datos de la imagen
    unsigned int tamImagen = metadatos->tamArchivo - metadatos->tamEncabezado;
    fwrite(&tamImagen, sizeof(unsigned int), 1, archivo);

    // Escribir la resolución horizontal (en píxeles por metro, 0 por defecto)
    unsigned int resolucion = 0;
    fwrite(&resolucion, sizeof(unsigned int), 1, archivo); // X pixels por metro

    // Escribir la resolución vertical (en píxeles por metro, 0 por defecto)
    fwrite(&resolucion, sizeof(unsigned int), 1, archivo); // Y pixels por metro

    // Escribir el número de colores usados en la paleta (0 si no hay paleta)
    unsigned int coloresUsados = 0;
    fwrite(&coloresUsados, sizeof(unsigned int), 1, archivo);

    // Escribir el número de colores importantes (0 significa todos son importantes)
    unsigned int coloresImportantes = 0;
    fwrite(&coloresImportantes, sizeof(unsigned int), 1, archivo);

    // Escribir los datos de la imagen
    fwrite(datos, 1, tamImagen, archivo);
    fclose(archivo);

    printf("Archivo %s guardado correctamente.\n", nombreArchivo);
}


void aplicarNegativo(t_metadata* metadatos, unsigned char* datos)
{
    int ancho = metadatos->ancho;
    int alto = metadatos->alto;
    int relleno = (4 - (ancho * 3) % 4) % 4;

    for (int y = 0; y < alto; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            int indice = (y * (ancho * 3 + relleno)) + (x * 3);
            datos[indice] = 255 - datos[indice];       // Azul
            datos[indice + 1] = 255 - datos[indice + 1]; // Verde
            datos[indice + 2] = 255 - datos[indice + 2]; // Rojo
        }
    }
}

void solucion(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Uso: %s <modificacion> <archivo.bmp>\n", argv[0]);
        return;
    }

    int cantModificaciones = 0;
    char** modificaciones = obtenerArgumentosModificaciones(argc, argv, &cantModificaciones);
    const char* nombreArchivo = argv[argc - 1];

    printf("Argumentos:\n");
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    printf("Modificaciones:\n");
    for (int i = 0; i < cantModificaciones; i++)
    {
        printf("modificaciones[%d] = %s\n", i, modificaciones[i]);
    }

    t_metadata metadatos;
    unsigned char* datos = cargarBMP(nombreArchivo, &metadatos);
    if (!datos)
    {
        printf("Error al cargar los datos de la imagen.\n");
        free(modificaciones);
        return;
    }

    for (int i = 0; i < cantModificaciones; i++)
    {
        if (strcmp(modificaciones[i], "--negativo") == 0)
        {
            aplicarNegativo(&metadatos, datos);
            guardarBMP("estudiante_negativo.bmp", &metadatos, datos);
            printf("Imagen en negativo guardada como estudiante_negativo.bmp\n");
        }
        // Agrega otras modificaciones aquí
    }

    free(datos);
    free(modificaciones);
}


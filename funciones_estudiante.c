/*
    Integrantes del grupo. En caso de ser un grupo de dos integrantes, no completar el último campo.
    Si alguno de los integrantes del grupo dejara la materia, completar de todos modos sus datos, aclarando que no entrega.
    -----------------
    Apellido: Dapena
    Nombre: Ezequiel
    DNI: 42.725.436
    Entrega: Si
    -----------------
    Apellido: Brance
    Nombre: Alan Emanuel
    DNI: 36.847.607
    Entrega: Si
    -----------------
    Apellido:
    Nombre:
    DNI:
    Entrega:
    -----------------
*/

#include "funciones_estudiante.h"

int copiarEncabezado(FILE* archivoSalida, const t_metadata* metadatos)
{
    // Crear un buffer para el encabezado
    unsigned char* bufferEncabezado = (unsigned char*)malloc(metadatos->tamEncabezado);
    if (!bufferEncabezado)
    {
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    // Copiar los datos del encabezado desde los metadatos
    fseek(archivoSalida, 0, SEEK_SET);

    // Encabezado del archivo BMP
    unsigned short tipoArchivo = 0x4D42;  // 'BM' en little-endian
    fwrite(&tipoArchivo, sizeof(unsigned short), 1, archivoSalida);
    fwrite(&metadatos->tamArchivo, sizeof(unsigned int), 1, archivoSalida);
    unsigned int reservado = 0;
    fwrite(&reservado, sizeof(unsigned int), 1, archivoSalida);
    fwrite(&metadatos->tamEncabezado, sizeof(unsigned int), 1, archivoSalida);

    // Encabezado de información BMP
    unsigned int tamEncabezadoInfo = 40;
    fwrite(&tamEncabezadoInfo, sizeof(unsigned int), 1, archivoSalida);
    fwrite(&metadatos->ancho, sizeof(unsigned int), 1, archivoSalida);
    fwrite(&metadatos->alto, sizeof(unsigned int), 1, archivoSalida);
    unsigned short planos = 1;
    fwrite(&planos, sizeof(unsigned short), 1, archivoSalida);
    fwrite(&metadatos->profundidad, sizeof(unsigned short), 1, archivoSalida);
    unsigned int compresion = 0;
    fwrite(&compresion, sizeof(unsigned int), 1, archivoSalida);
    unsigned int tamImagen = metadatos->tamArchivo - metadatos->tamEncabezado;
    fwrite(&tamImagen, sizeof(unsigned int), 1, archivoSalida);
    unsigned int resolucionHorizontal = 2835;  // 72 DPI
    fwrite(&resolucionHorizontal, sizeof(unsigned int), 1, archivoSalida);
    unsigned int resolucionVertical = 2835;  // 72 DPI
    fwrite(&resolucionVertical, sizeof(unsigned int), 1, archivoSalida);
    unsigned int coloresUsados = 0;
    fwrite(&coloresUsados, sizeof(unsigned int), 1, archivoSalida);
    unsigned int coloresImportantes = 0;
    fwrite(&coloresImportantes, sizeof(unsigned int), 1, archivoSalida);

    free(bufferEncabezado);
    return TODO_OK;
}

void leerEscribirRelleno(FILE* archivoEntrada, FILE* archivoSalida, int relleno)
{
    unsigned char rellenoBuffer[3] = {0};
    fread(rellenoBuffer, 1, relleno, archivoEntrada);
    fwrite(rellenoBuffer, 1, relleno, archivoSalida);
}

int aplicarNegativo(FILE* archivoEntrada, const t_metadata* metadatos)
{
    FILE* archivoSalida = fopen("estudiante_negativo.bmp", "wb");
    if (!archivoSalida)
    {
        printf("No se pudo crear el archivo de salida.\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    int resultado = copiarEncabezado(archivoSalida, metadatos);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    int ancho = metadatos->ancho;
    int alto = metadatos->alto;
    int relleno = (4 - (ancho * 3) % 4) % 4;
    t_pixel pixel;

    for (int y = 0; y < alto; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            pixel.pixel[ROJO] = 255 - pixel.pixel[ROJO];
            pixel.pixel[VERDE] = 255 - pixel.pixel[VERDE];
            pixel.pixel[AZUL] = 255 - pixel.pixel[AZUL];
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        leerEscribirRelleno(archivoEntrada, archivoSalida, relleno);
    }

    fclose(archivoSalida);

    printf("Imagen en negativo guardada como estudiante_negativo.bmp\n");
    return TODO_OK;
}

int aplicarEscalaDeGrises(FILE* archivoEntrada, const t_metadata* metadatos)
{
    FILE* archivoSalida = fopen("estudiante_escala_de_grises.bmp", "wb");
    if (!archivoSalida)
    {
        printf("No se pudo crear el archivo de salida.\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    int resultado = copiarEncabezado(archivoSalida, metadatos);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    int ancho = metadatos->ancho;
    int alto = metadatos->alto;
    int relleno = (4 - (ancho * 3) % 4) % 4;
    t_pixel pixel;

    for (int y = 0; y < alto; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            unsigned char gris = (unsigned char)(0.3 * pixel.pixel[2] + 0.59 * pixel.pixel[1] + 0.11 * pixel.pixel[0]);

            pixel.pixel[ROJO] = gris;
            pixel.pixel[VERDE] = gris;
            pixel.pixel[AZUL] = gris;

            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        leerEscribirRelleno(archivoEntrada, archivoSalida, relleno);
    }

    fclose(archivoSalida);

    printf("Imagen en escala de grises guardada como estudiante_escala_de_grises.bmp\n");
    return TODO_OK;
}

int aplicarContraste(FILE* archivoEntrada, const t_metadata* metadatos, double factorContraste, const char* nombreArchivoSalida)
{
    FILE* archivoSalida = fopen(nombreArchivoSalida, "wb");
    if (!archivoSalida)
    {
        printf("No se pudo crear el archivo de salida: %s.\n", nombreArchivoSalida);
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    int resultado = copiarEncabezado(archivoSalida, metadatos);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    int ancho = metadatos->ancho;
    int alto = metadatos->alto;
    int relleno = (4 - (ancho * 3) % 4) % 4;
    t_pixel pixel;

    unsigned long brilloTotal = 0;
    unsigned char brillo;

    for (int y = 0; y < alto; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            brillo = (unsigned char)(0.3 * pixel.pixel[2] + 0.59 * pixel.pixel[1] + 0.11 * pixel.pixel[0]);
            brilloTotal += brillo;
        }
        fseek(archivoEntrada, relleno, SEEK_CUR);
    }

    unsigned char brilloMedio = brilloTotal / (ancho * alto);

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    for (int y = 0; y < alto; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);

            for (int i = 0; i < 3; i++)
            {
                int nuevoValor = (int)((pixel.pixel[i] - brilloMedio) * factorContraste + brilloMedio);
                if (nuevoValor < 0) nuevoValor = 0;
                if (nuevoValor > 255) nuevoValor = 255;
                pixel.pixel[i] = (unsigned char)nuevoValor;
            }
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        leerEscribirRelleno(archivoEntrada, archivoSalida, relleno);
    }

    fclose(archivoSalida);

    printf("Imagen con contraste ajustado guardada como %s\n", nombreArchivoSalida);
    return TODO_OK;
}

int aplicarTonalidad(FILE* archivoEntrada, const t_metadata* metadatos, int colorAIncrementar, const char* nombreArchivoSalida)
{
    FILE* archivoSalida = fopen(nombreArchivoSalida, "wb");
    if (!archivoSalida)
    {
        printf("No se pudo crear el archivo de salida: %s.\n", nombreArchivoSalida);
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    int resultado = copiarEncabezado(archivoSalida, metadatos);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    int ancho = metadatos->ancho;
    int alto = metadatos->alto;
    int relleno = (4 - (ancho * 3) % 4) % 4;
    t_pixel pixel;

    for (int y = 0; y < alto; y++)
    {
        for (int x = 0; x < ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);

            // Incrementar la tonalidad del color
            int nuevoValor = (int)(pixel.pixel[colorAIncrementar] * 1.5);
            if (nuevoValor > 255) nuevoValor = 255;
            pixel.pixel[colorAIncrementar] = (unsigned char)nuevoValor;

            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        leerEscribirRelleno(archivoEntrada, archivoSalida, relleno);
    }

    fclose(archivoSalida);

    printf("Imagen con tonalidad ajustada guardada como %s\n", nombreArchivoSalida);
    return TODO_OK;
}

int aplicarRecortar(FILE* archivoEntrada, const t_metadata* metadatos)
{
    FILE* archivoSalida = fopen("estudiante_recortar.bmp", "wb");
    if (!archivoSalida)
    {
        puts("No se pudo crear el archivo de salida: estudiante_recortar.bmp\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    t_metadata metadatosRecorte = *metadatos;

    metadatosRecorte.alto /= 2;
    metadatosRecorte.ancho /= 2;

    // Actualizar el tamaño de la imagen en los metadatos recortados
    unsigned int tamImagenRecortada = metadatosRecorte.ancho * metadatosRecorte.alto * 3;
    unsigned int rellenoRecorte = (4 - (metadatosRecorte.ancho * 3) % 4) % 4;
    tamImagenRecortada += rellenoRecorte * metadatosRecorte.alto;

    metadatosRecorte.tamArchivo = metadatosRecorte.tamEncabezado + tamImagenRecortada;

    int resultado = copiarEncabezado(archivoSalida, &metadatosRecorte);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    int anchoOriginal = metadatos->ancho;
    int altoOriginal = metadatos->alto;
    int rellenoOriginal = (4 - (anchoOriginal * 3) % 4) % 4;
    t_pixel pixel;

    for (int y = altoOriginal - metadatosRecorte.alto; y < altoOriginal; y++)
    {
        fseek(archivoEntrada, metadatos->tamEncabezado + y * (anchoOriginal * 3 + rellenoOriginal), SEEK_SET);

        for (int x = 0; x < metadatosRecorte.ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        for (int r = 0; r < rellenoRecorte; r++)
        {
            unsigned char relleno = 0;
            fwrite(&relleno, sizeof(unsigned char), 1, archivoSalida);
        }
    }

    fclose(archivoSalida);

    puts("Imagen recortada guardada como estudiante_recortar.bmp\n");
    return TODO_OK;
}

int aplicarRotar(FILE* archivoEntrada, const t_metadata* metadatos, bool rotarDerecha, const char* nombreArchivoSalida)
{
    FILE* archivoSalida = fopen(nombreArchivoSalida, "wb");
    if (!archivoSalida)
    {
        printf("No se pudo crear el archivo de salida: %s.\n", nombreArchivoSalida);
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    t_metadata metadatosRotacion = *metadatos;

    // Intercambiar alto y ancho
    metadatosRotacion.alto = metadatos->ancho;
    metadatosRotacion.ancho = metadatos->alto;

    // Calcular nuevo relleno
    int rellenoOriginal = (4 - (metadatos->ancho * 3) % 4) % 4;
    int rellenoRotado = (4 - (metadatosRotacion.ancho * 3) % 4) % 4;
    metadatosRotacion.tamArchivo = metadatosRotacion.tamEncabezado + (metadatosRotacion.ancho * 3 + rellenoRotado) * metadatosRotacion.alto;

    int resultado = copiarEncabezado(archivoSalida, &metadatosRotacion);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    // Crear una imagen temporal
    unsigned char* imagenTemp = (unsigned char*)malloc((metadatos->ancho * 3 + rellenoOriginal) * metadatos->alto);
    if (!imagenTemp)
    {
        fclose(archivoSalida);
        printf("No se pudo asignar memoria para la imagen temporal.\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    // Leer toda la imagen original a imagen temporal
    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);
    fread(imagenTemp, 1, (metadatos->ancho * 3 + rellenoOriginal) * metadatos->alto, archivoEntrada);

    // Rotar la imagen
    for (int y = 0; y < metadatosRotacion.alto; y++)
    {
        for (int x = 0; x < metadatosRotacion.ancho; x++)
        {
            int srcX, srcY;

            if (rotarDerecha)
            {
                srcX = metadatos->ancho - 1 - y;
                srcY = x;
            }
            else
            {
                srcX = y;
                srcY = metadatos->alto - 1 - x;
            }

            unsigned char* pixel = imagenTemp + (srcY * (metadatos->ancho * 3 + rellenoOriginal)) + (srcX * 3);
            if (fwrite(pixel, sizeof(unsigned char), 3, archivoSalida) != 3)
            {
                printf("Error al escribir en el archivo de salida.\n");
                free(imagenTemp);
                fclose(archivoSalida);
                return NO_SE_PUEDE_CREAR_ARCHIVO;
            }
        }
        for (int r = 0; r < rellenoRotado; r++)
        {
            unsigned char relleno = 0;
            if (fwrite(&relleno, sizeof(unsigned char), 1, archivoSalida) != 1)
            {
                printf("Error al escribir relleno en el archivo de salida.\n");
                free(imagenTemp);
                fclose(archivoSalida);
                return NO_SE_PUEDE_CREAR_ARCHIVO;
            }
        }
    }

    free(imagenTemp);
    fclose(archivoSalida);

    printf("Imagen rotada guardada como %s\n", nombreArchivoSalida);
    return TODO_OK;
}

int aplicarEspejo(FILE* archivoEntrada, const t_metadata* metadatos)
{
    FILE* archivoSalida = fopen("estudiante_comodin_espejo.bmp", "wb");
    if (!archivoSalida)
    {
        puts("No se pudo crear el archivo de salida: estudiante_comodin_espejo.bmp\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    int resultado = copiarEncabezado(archivoSalida, metadatos);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    int relleno = (4 - (metadatos->ancho * 3) % 4) % 4;
    // Crear una imagen temporal
    unsigned char* imagenTemp = (unsigned char*)malloc((metadatos->ancho * 3 + relleno) * metadatos->alto);
    if (!imagenTemp)
    {
        fclose(archivoSalida);
        puts("No se pudo asignar memoria para la imagen temporal.\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    // Leer toda la imagen original a imagen temporal
    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);
    fread(imagenTemp, 1, (metadatos->ancho * 3 + relleno) * metadatos->alto, archivoEntrada);

    for (int y = 0; y < metadatos->alto; y++)
    {
        for (int x = metadatos->ancho - 1; x >= 0; x--)
        {
            unsigned char* pixel = imagenTemp + (y * (metadatos->ancho * 3 + relleno)) + (x * 3);
            fwrite(pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        // Escribir el relleno después de cada fila
        for (int r = 0; r < relleno; r++)
        {
            unsigned char rellenoByte = 0;
            fwrite(&rellenoByte, sizeof(unsigned char), 1, archivoSalida);
        }
    }

    free(imagenTemp);
    fclose(archivoSalida);

    printf("Imagen en espejo guardada como estudiante_comodin_espejo.bmp\n");
    return TODO_OK;
}

int aplicarAchicar(FILE* archivoEntrada, const t_metadata* metadatos)
{
    FILE* archivoSalida = fopen("estudiante_achicar.bmp", "wb");
    if (!archivoSalida)
    {
        puts("No se pudo crear el archivo de salida: estudiante_achicar.bmp\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    t_metadata metadatosAchicados = *metadatos;

    // Reducción de tamaño a la mitad
    metadatosAchicados.alto /= 2;
    metadatosAchicados.ancho /= 2;

    // Actualizar el tamaño de la imagen en los metadatos reducidos
    unsigned int tamImagenAchicada = metadatosAchicados.ancho * metadatosAchicados.alto * 3;
    unsigned int rellenoAchicado = (4 - (metadatosAchicados.ancho * 3) % 4) % 4;
    tamImagenAchicada += rellenoAchicado * metadatosAchicados.alto;

    metadatosAchicados.tamArchivo = metadatosAchicados.tamEncabezado + tamImagenAchicada;

    int resultado = copiarEncabezado(archivoSalida, &metadatosAchicados);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    int anchoOriginal = metadatos->ancho;
    int rellenoOriginal = (4 - (anchoOriginal * 3) % 4) % 4;
    t_pixel pixel;

    for (int y = 0; y < metadatosAchicados.alto; y++)
    {
        for (int x = 0; x < metadatosAchicados.ancho; x ++)
        {
            // Leer el píxel actual y escribirlo en la imagen achicada
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);

            // Saltar un píxel en la fila original
            fseek(archivoEntrada, 3, SEEK_CUR);
        }

        // Saltar el relleno en la fila original
        fseek(archivoEntrada, rellenoOriginal, SEEK_CUR);
        // Saltar una fila completa (anchoOriginal * 3 + rellenoOriginal)
        fseek(archivoEntrada, anchoOriginal * 3 + rellenoOriginal, SEEK_CUR);

        // Escribir el relleno para la fila achicada
        for (int r = 0; r < rellenoAchicado; r++)
        {
            unsigned char relleno = 0;
            fwrite(&relleno, sizeof(unsigned char), 1, archivoSalida);
        }
    }


    fclose(archivoSalida);

    puts("Imagen achicada guardada como estudiante_achicar.bmp\n");
    return TODO_OK;
}

int aplicarConcatenar(FILE* archivoEntrada, const t_metadata* metadatos, FILE* archivoConcatenar, const t_metadata* metadatosConcatenar)
{
    FILE* archivoSalida = fopen("estudiante_concatenar.bmp", "wb");
    if (!archivoSalida)
    {
        puts("No se pudo crear el archivo de salida: estudiante_concatenar.bmp\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    // Determinar el ancho mayor y usarlo para los metadatos de la imagen concatenada
    t_metadata metadatosConcatenados = metadatos->ancho > metadatosConcatenar->ancho ? *metadatos : *metadatosConcatenar;
    int altoNuevaImg = metadatos->alto + metadatosConcatenar->alto;
    metadatosConcatenados.alto = altoNuevaImg;

    // Calcular nuevo tamaño de la imagen concatenada
    unsigned int tamNuevaImg = metadatosConcatenados.ancho * metadatosConcatenados.alto * 3;
    unsigned int rellenoNuevaImg = (4 - (metadatosConcatenados.ancho * 3) % 4) % 4;
    tamNuevaImg += rellenoNuevaImg * metadatosConcatenados.alto;
    metadatosConcatenados.tamArchivo = metadatosConcatenados.tamEncabezado + tamNuevaImg;

    // Copiar el encabezado de la nueva imagen concatenada
    int resultado = copiarEncabezado(archivoSalida, &metadatosConcatenados);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);
    fseek(archivoConcatenar, metadatosConcatenar->tamEncabezado, SEEK_SET);

    t_pixel pixel;

    // Procesar la primera imagen ( abajo )
    for (int y = 0; y < metadatos->alto; y++)
    {
        for (int x = 0; x < metadatos->ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        // Rellenar si el ancho de la primera imagen es menor que el ancho concatenado
        for (int x = metadatos->ancho; x < metadatosConcatenados.ancho; x++)
        {
            pixel.pixel[ROJO] = 0;
            pixel.pixel[VERDE] = 0;
            pixel.pixel[AZUL] = 255;
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        leerEscribirRelleno(archivoEntrada, archivoSalida, rellenoNuevaImg);
    }

    // Procesar la segunda imagen ( arriba )
    for (int y = 0; y < metadatosConcatenar->alto; y++)
    {
        for (int x = 0; x < metadatosConcatenar->ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoConcatenar);
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        // Rellenar si el ancho de la segunda imagen es menor que el ancho concatenado
        for (int x = metadatosConcatenar->ancho; x < metadatosConcatenados.ancho; x++)
        {
            pixel.pixel[ROJO] = 0;
            pixel.pixel[VERDE] = 0;
            pixel.pixel[AZUL] = 255;
            fwrite(&pixel.pixel, sizeof(unsigned char), 3, archivoSalida);
        }
        leerEscribirRelleno(archivoConcatenar, archivoSalida, rellenoNuevaImg);
    }

    fclose(archivoSalida);

    puts("Imagen concatenada guardada como estudiante_concatenar.bmp\n");
    return TODO_OK;
}

int aplicarMonocromo(FILE* archivoEntrada, const t_metadata* metadatos)
{
    FILE* archivoSalida = fopen("estudiante_monocromo.bmp", "wb");
    if (!archivoSalida)
    {
        puts("No se pudo crear el archivo de salida: estudiante_monocromo.bmp\n");
        return NO_SE_PUEDE_CREAR_ARCHIVO;
    }

    t_metadata metadatosMonocromo = *metadatos;
    metadatosMonocromo.profundidad = 1;  // Cambiar la profundidad a 1 bit

    // Calcular el relleno original
    unsigned int rellenoOriginal = (4 - (metadatos->ancho * 3) % 4) % 4;

    // Calcular nuevo tamaño de la imagen monocromática
    unsigned int anchoBytes = (metadatosMonocromo.ancho + 7) / 8;  // Redondear hacia arriba
    unsigned int rellenoMonocromo = (4 - (anchoBytes % 4)) % 4;
    unsigned int tamImagenMonocromo = (anchoBytes + rellenoMonocromo) * metadatosMonocromo.alto;

    metadatosMonocromo.tamArchivo = metadatosMonocromo.tamEncabezado + tamImagenMonocromo + 2 * sizeof(unsigned int);

    // Copiar el encabezado de la nueva imagen monocromática
    int resultado = copiarEncabezado(archivoSalida, &metadatosMonocromo);
    if (resultado != TODO_OK)
    {
        fclose(archivoSalida);
        return resultado;
    }

    // Escribir la paleta de colores para la imagen de 1 bit
    unsigned int colorNegro = 0x00000000;
    unsigned int colorBlanco = 0x00FFFFFF;
    fwrite(&colorNegro, sizeof(unsigned int), 1, archivoSalida);
    fwrite(&colorBlanco, sizeof(unsigned int), 1, archivoSalida);

    fseek(archivoEntrada, metadatos->tamEncabezado, SEEK_SET);

    t_pixel pixel;
    unsigned char byteMonocromo = 0;
    int bitPos = 0;

    for (int y = 0; y < metadatos->alto; y++)
    {
        for (int x = 0; x < metadatos->ancho; x++)
        {
            fread(&pixel.pixel, sizeof(unsigned char), 3, archivoEntrada);
            unsigned char promedio = (pixel.pixel[ROJO] + pixel.pixel[VERDE] + pixel.pixel[AZUL]) / 3;

            if (promedio >= 128)
                byteMonocromo |= (1 << (7 - bitPos));  // Establecer el bit correspondiente

            bitPos++;

            if (bitPos == 8)
            {
                fwrite(&byteMonocromo, sizeof(unsigned char), 1, archivoSalida);
                byteMonocromo = 0;
                bitPos = 0;
            }
        }

        // Escribir cualquier byte incompleto
        if (bitPos != 0)
        {
            fwrite(&byteMonocromo, sizeof(unsigned char), 1, archivoSalida);
            byteMonocromo = 0;
            bitPos = 0;
        }

        // Escribir relleno
        for (int r = 0; r < rellenoMonocromo; r++)
        {
            unsigned char rellenoByte = 0;
            fwrite(&rellenoByte, sizeof(unsigned char), 1, archivoSalida);
        }

        // Saltar relleno original
        fseek(archivoEntrada, rellenoOriginal, SEEK_CUR);
    }

    fclose(archivoSalida);

    printf("Imagen monocromática guardada como estudiante_monocromo.bmp\n");
    return TODO_OK;
}



int cargarMetadatos(FILE* archivo, t_metadata* metadatos)
{
    unsigned short tipoArchivo;
    fread(&tipoArchivo, sizeof(unsigned short), 1, archivo);
    if (tipoArchivo != 0x4D42)   // 'BM' en little-endian
    {
        printf("El archivo no es un BMP válido.\n");
        return ARCHIVO_NO_ENCONTRADO;
    }

    fread(&metadatos->tamArchivo, sizeof(unsigned int), 1, archivo);
    fseek(archivo, 4, SEEK_CUR);
    fread(&metadatos->tamEncabezado, sizeof(unsigned int), 1, archivo);

    unsigned int tamEncabezadoInfo;
    fread(&tamEncabezadoInfo, sizeof(unsigned int), 1, archivo);
    if (tamEncabezadoInfo != 40)
    {
        printf("El encabezado de información BMP no tiene el tamaño esperado.\n");
        return ARCHIVO_NO_ENCONTRADO;
    }

    fread(&metadatos->ancho, sizeof(unsigned int), 1, archivo);
    fread(&metadatos->alto, sizeof(unsigned int), 1, archivo);

    unsigned short planos;
    fread(&planos, sizeof(unsigned short), 1, archivo);
    if (planos != 1)
    {
        printf("El archivo BMP no tiene el número esperado de planos de color.\n");
        return ARCHIVO_NO_ENCONTRADO;
    }

    fread(&metadatos->profundidad, sizeof(unsigned short), 1, archivo);
    if (metadatos->profundidad != 24)
    {
        printf("El archivo BMP no es de 24 bits.\n");
        return ARCHIVO_NO_ENCONTRADO;
    }

    printf("Archivo cargado correctamente.\n");
    printf("Tamaño del archivo: %u bytes\n", metadatos->tamArchivo);
    printf("Ancho de la imagen: %u pixels\n", metadatos->ancho);
    printf("Alto de la imagen: %u pixels\n", metadatos->alto);

    return TODO_OK;
}

int modificarImagen(char** imagenes,int cantImagenes, char** modificaciones, int cantModificaciones)
{
    FILE* archivoEntrada = fopen(imagenes[0], "rb");
    if (!archivoEntrada)
    {
        printf("No se pudo abrir el archivo %s.\n", imagenes[0]);
        return ARCHIVO_NO_ENCONTRADO;
    }

    t_metadata metadatos;
    int resultado = cargarMetadatos(archivoEntrada, &metadatos);
    if (resultado != TODO_OK)
    {
        fclose(archivoEntrada);
        return resultado;
    }

    for (int i = 0; i < cantModificaciones; i++)
    {
        if (strcmp(modificaciones[i], "--negativo") == 0)
        {
            resultado = aplicarNegativo(archivoEntrada, &metadatos);
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar negativo: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--escala-de-grises") == 0)
        {
            resultado = aplicarEscalaDeGrises(archivoEntrada, &metadatos);
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar escala de grises: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--aumentar-contraste") == 0)
        {
            resultado = aplicarContraste(archivoEntrada, &metadatos, 1.25, "estudiante_aumentar_contraste.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar aumento de contraste: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--reducir-contraste") == 0)
        {
            resultado = aplicarContraste(archivoEntrada, &metadatos, 0.75, "estudiante_reducir_contraste.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar reducción de contraste: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--tonalidad-azul") == 0)
        {
            resultado = aplicarTonalidad(archivoEntrada, &metadatos, AZUL, "estudiante_tonalidad_azul.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar tonalidad azul: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--tonalidad-verde") == 0)
        {
            resultado = aplicarTonalidad(archivoEntrada, &metadatos, VERDE, "estudiante_tonalidad_verde.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar tonalidad verde: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--tonalidad-roja") == 0)
        {
            resultado = aplicarTonalidad(archivoEntrada, &metadatos, ROJO, "estudiante_tonalidad_rojo.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar tonalidad roja: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--recortar") == 0)
        {
            resultado = aplicarRecortar(archivoEntrada, &metadatos);
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar recorte: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--rotar-derecha") == 0)
        {
            resultado = aplicarRotar(archivoEntrada, &metadatos, true, "estudiante_rotar_derecha.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar rotacion derecha: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--rotar-izquierda") == 0)
        {
            resultado = aplicarRotar(archivoEntrada, &metadatos, false, "estudiante_rotar_izquierda.bmp");
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar rotacion izquierda: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--comodin") == 0)
        {
            resultado = aplicarEspejo(archivoEntrada, &metadatos);
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar espejo ( comodin ): %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--achicar") == 0)
        {
            resultado = aplicarAchicar(archivoEntrada, &metadatos);
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar achicar: %d\n", resultado);
            }
            continue;
        }

        if (strcmp(modificaciones[i], "--concatenar") == 0)
        {

            FILE* archivoConcatenar = fopen(imagenes[1], "rb");
            if (!archivoConcatenar)
            {
                printf("No se pudo abrir el archivo %s.\n", imagenes[1]);
                continue;
            }

            t_metadata metadatosConcatenar;
            int resultadoConcat = cargarMetadatos(archivoConcatenar, &metadatosConcatenar);
            if (resultadoConcat != TODO_OK)
            {
                fclose(archivoConcatenar);
                continue;
            }

            resultado = aplicarConcatenar(archivoEntrada, &metadatos, archivoConcatenar, &metadatosConcatenar);
            if (resultado != TODO_OK)
            {
                printf("Error al concatenar las imagenes: %d\n", resultado);
            }
            fclose(archivoConcatenar);
            continue;
        }

        if (strcmp(modificaciones[i], "--monocromo") == 0)
        {
            resultado = aplicarMonocromo(archivoEntrada, &metadatos);
            if (resultado != TODO_OK)
            {
                printf("Error al aplicar monocromo: %d\n", resultado);
            }
            continue;
        }


        printf("No existe funcion definida bajo el parametro %s\n", modificaciones[i]);
    }

    fclose(archivoEntrada);
    return TODO_OK;
}

void obtenerArgumentos(char** argv, int cantArgumentos, char** modificaciones, int* cantModificaciones, char** imagenes, int* cantImagenes)
{
    int imgCount = 0, modCount = 0;

    for (int i = 1; i <= cantArgumentos; i++)
    {
        // Validar si el argumento empieza con "--"
        if (strncmp(argv[i], "--", 2) == 0)
        {
            modificaciones[modCount++] = argv[i];
        }

        // Validar si el argumento termina con ".bmp"
        int len = strlen(argv[i]);
        if (len > 4 && strcmp(argv[i] + len - 4, ".bmp") == 0)
        {
            imagenes[imgCount++] = argv[i];
        }
    }

    *cantModificaciones = modCount;
    *cantImagenes = imgCount;
}

int solucion(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Uso: %s <modificacion> <archivo.bmp>\n", argv[0]);
        return ARCHIVO_NO_ENCONTRADO;
    }

    int cantArgumentos = argc - 1;
    int cantModificaciones, cantImagenes = 0;

    char** modificaciones = (char**)malloc(cantArgumentos * sizeof(char*));
    char** imagenes = (char**)malloc(cantArgumentos * sizeof(char*));

    obtenerArgumentos(argv, cantArgumentos, modificaciones, &cantModificaciones, imagenes, &cantImagenes);

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

    int resultado = modificarImagen(imagenes, cantImagenes, modificaciones, cantModificaciones);

    free(imagenes);
    free(modificaciones);
    return resultado;
}


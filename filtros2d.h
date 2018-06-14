/*============================================================================*/
/* FILTRAGEM ESPACIAL                                                         */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e fun��es para filtragem espacial. */
/*============================================================================*/

#ifndef __FILTROS2D_H
#define __FILTROS2D_H

/*============================================================================*/

#include "imagem.h"
#include "geometria.h"

/*============================================================================*/

// Gen�ricos.
void filtro1D (Imagem* in, Imagem* out, float* coef, int n, int vertical);
void filtro2D (Imagem* in, Imagem* out, float** coef, int altura, int largura, int transposta);

// Suaviza��o e realce.
void blur (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer);
void filtroGaussiano (Imagem* in, Imagem* out, float sigmax, float sigmay, Imagem* buffer);
void unsharpMasking (Imagem* in, Imagem* out, float sigma, float threshold, float mult, Imagem* buffer);
void filtroMediana8bpp (Imagem* in, Imagem* out, int altura, int largura);
void filtroMedianaBinario (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer);

// Morfologia.
void maxLocal (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer);
void minLocal (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer);
Imagem* criaKernelCircular (int largura);
void dilata (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out);
void erode (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out);
void abertura (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out, Imagem* buffer);
void fechamento (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out, Imagem* buffer);

// Gradientes.
void filtroSobel (Imagem* in, Imagem* out, int tamanho, int vertical, int escalado);
void computaGradientes (Imagem* in, int tamanho_sobel, Imagem* dx, Imagem* dy, Imagem* mag, Imagem* ori);


void detectorCanny (Imagem* img, int tamanho_sobel, float t_inferior, float t_superior, int usa_proporcao, Imagem* out);
void _cannyFloodHisterese (Imagem* in, int channel, int row, int col, float threshold, Imagem* out);
void _cannyIsolaMaximosLocais (Imagem* mag, Imagem* ori);
/*============================================================================*/
#endif /* __FILTROS2D_H */

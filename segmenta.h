/*============================================================================*/
/* SEGMENTA��O                                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e fun��es para segmenta��o. */
/*============================================================================*/

#ifndef __SEGMENTA_H
#define __SEGMENTA_H

/*============================================================================*/

#include "imagem.h"
#include "geometria.h"

/*============================================================================*/

typedef struct
{
    float label;
    Retangulo roi;
    int n_pixels;

} ComponenteConexo;

/*----------------------------------------------------------------------------*/

void binariza (Imagem* in, Imagem* out, float threshold);
void binarizaAdapt (Imagem* in, Imagem* out, int largura, float threshold, Imagem* buffer);
float thresholdOtsu (Imagem* img);

int rotulaFloodFill (Imagem* img, ComponenteConexo** componentes, int largura_min, int altura_min, int n_pixels_min, int idx);
void floodFill (Imagem* img, Coordenada* pilha, ComponenteConexo* componente);
int rotulaUnionFind (Imagem* img, ComponenteConexo** componentes, int largura_min, int altura_min, int n_pixels_min);

/*============================================================================*/
#endif /* __IMAGEM_H */

/*============================================================================*/
/* Exemplo: segmenta��o de uma imagem em escala de cinza.                     */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu                                                     */
/* Universidade Tecnol�gica Federal do Paran�                                 */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

/*============================================================================*/

#define ALTURA_MIN 5
#define LARGURA_MIN 5
#define N_PIXELS_MIN 30

/*============================================================================*/

void chamferFunc(Imagem *canny, Imagem *chamfer);

void recurssChamfer(Imagem *canny, Imagem *chamfer, float value, int y, int x);

void redDetector(Imagem *img, Imagem *img_out);

void redMapping(Imagem *in, Imagem *out);

/*============================================================================*/

int main()
{
	//int i, j; //, p;
	//float r, g, b, h, s, l;

	char *files[13] = {"./img/placa01.bmp", "./img/placa02.bmp", "./img/placa03.bmp", "./img/placa04.bmp",
					   "./img/placa05.bmp", "./img/placa06.bmp", "./img/placa07.bmp", "./img/placa08.bmp",
					   "./img/placa09.bmp", "./img/placa10.bmp", "./img/placa11.bmp"
					   , "./img/placa12.bmp", "./img/placa13.bmp"};

	char fileName[25];

	for (int idx = 0; idx < 13; idx++)
	{

		Imagem *img = abreImagem(files[idx], 3);
		if (!img)
		{
			printf("Erro abrindo a imagem.\n");
			exit(1);
		}
		Imagem *canny = criaImagem(img->largura, img->altura, 3);

		Imagem *chamfer = criaImagem(img->largura, img->altura, 1);

		Imagem *red = criaImagem(img->largura, img->altura, 3);
		
		Imagem *redMap = criaImagem(img->largura, img->altura, 3);

		redDetector(img, red);

		sprintf(fileName, "./resultados/%d-red.bmp", idx+1);
		salvaImagem(red, fileName);		

		detectorCanny(img, 3, 0.01, 0.4, 1, canny);

		sprintf(fileName, "./resultados/%d-canny.bmp", idx+1);
		salvaImagem(canny, fileName);

		redMapping(img, redMap);
		sprintf(fileName, "./resultados/%d-redMap.bmp", idx+1);
		ComponenteConexo *componentes;
		rotulaFloodFill(redMap, &componentes, LARGURA_MIN, ALTURA_MIN, N_PIXELS_MIN);
		salvaImagem(redMap, fileName);		

		/*for (i = 0; i < chamfer->altura; i++)
		{
			for (j = 0; j < chamfer->largura; j++)
			{
				chamfer->dados[0][i][j] = 100000000;
			}
		}

		Imagem *tst = abreImagem("saida.bmp", 3);
		chamferFunc(canny, tst);

		salvaImagem(chamfer, "chamfer_teste.bmp");*/

		destroiImagem(canny);
		destroiImagem(chamfer);
		destroiImagem(img);
		destroiImagem(red);
		destroiImagem(redMap);
	}
	return (0);
}

void chamferFunc(Imagem *canny, Imagem *chamfer)
{
	int i, j; //, c;

	for (i = 0; i < canny->altura; i++)
	{
		for (j = 0; j < canny->largura; j++)
		{
			//printf("\nCANNY \ni: %d\nj: %d\ncanny: %f\n", i, j, canny->dados[0][i][j]);
			if (canny->dados[0][i][j] == 1)
			{
				printf("\ni: %d\nj: %d\n", i, j);
				chamfer->dados[0][i][j] = 0;
				recurssChamfer(canny, chamfer, 0.3, i, j);
			}
		}
	}
}

void recurssChamfer(Imagem *canny, Imagem *chamfer, float value, int y, int x)
{

	if (value < 6.0)
	{

		chamfer->dados[0][y][x] = value;

		if (x > 0)
		{
			if (chamfer->dados[0][y][x - 1] > value)
			{
				if (canny->dados[0][y][x - 1] != 1)
				{
					value = value + 0.01;
					//printf("\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
					recurssChamfer(canny, chamfer, value, y, x - 1);
				}
			}
		}

		if (y > 0)
		{
			if (chamfer->dados[0][y - 1][x] > value)
			{
				if (canny->dados[0][y - 1][x] != 1)
				{
					value = value + 0.01;
					//printf("\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
					recurssChamfer(canny, chamfer, value, y - 1, x);
				}
			}
		} 

		if (x + 1 < canny->largura - 1)
		{
			if (chamfer->dados[0][y][x + 1] > value)
			{
				if (canny->dados[0][y][x + 1] != 1)
				{
					value = value + 0.01;
					//printf("\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
					recurssChamfer(canny, chamfer, value, y, x + 1);
				}
			}
		}

		if (y + 1 < canny->altura - 1)
		{
			if (chamfer->dados[0][y + 1][x] > value)
			{
				if (canny->dados[0][y + 1][x] != 1)
				{
					value = value + 0.01;
					//printf("\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
					recurssChamfer(canny, chamfer, value, y + 1, x);
				}
			}
		}
	}
}



void redDetector(Imagem *img, Imagem *img_out){

	int i, j;
	float r, g, b;

	for (i = 0; i < img->altura; i++)
		{
			for (j = 0; j < img->largura; j++)
			{
				r = img->dados[0][i][j];
				g = img->dados[1][i][j];
				b = img->dados[2][i][j];

				if ((r - g > 0.1 && r - b > 0.1))
				{
					img_out->dados[0][i][j] = img->dados[0][i][j];
					img_out->dados[1][i][j] = img->dados[1][i][j];
					img_out->dados[2][i][j] = img->dados[2][i][j];
				}
				else
				{
					img_out->dados[0][i][j] = -1;
					img_out->dados[1][i][j] = -1;
					img_out->dados[2][i][j] = -1;
				}
			}
		}
}

void redMapping(Imagem *in, Imagem *out)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf("ERRO: binariza: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit(1);
    }

    int j, k;
	float r, g, b;
    for (j = 0; j < in->altura; j++){
        for (k = 0; k < in->largura; k++){

			r = in->dados[0][j][k];
			g = in->dados[1][j][k];
			b = in->dados[2][j][k];

            if ((r - g > 0.1 && r - b > 0.1f)) {
                out->dados[0][j][k] = 1.0f;
				out->dados[1][j][k] = 1.0f;
				out->dados[2][j][k] = 1.0f;
			} else {
                out->dados[0][j][k] = 0.0f;
				out->dados[1][j][k] = 0.0f;
				out->dados[2][j][k] = 0.0f;
			}
		}
	}
}
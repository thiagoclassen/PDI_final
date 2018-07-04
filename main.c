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

#define THRESHOLD 1.0
#define ALTURA_MIN 30
#define LARGURA_MIN 30
#define N_PIXELS_MIN 30

/*============================================================================*/

typedef struct {
	Coordenada top;
	Coordenada bot;
	Coordenada left;
	Coordenada right;
} Frame;

void chamferFunc(Imagem *canny, Imagem *chamfer);

int recurssChamfer(Imagem *canny, Imagem *chamfer, float value, int y, int x);

void redDetector(Imagem *img, Imagem *img_out);

void redMapping(Imagem *in, Imagem *out);

int checkPlaca(Imagem *img);

/*============================================================================*/

int main()
{
	int elem, idx=5, qtde, i, j;
	//float r, g, b, h, s, l;

	char *files[13] = {"./img/placa01.bmp", "./img/placa02.bmp", "./img/placa03.bmp", "./img/placa04.bmp",
					   "./img/placa05.bmp", "./img/placa06.bmp", "./img/placa07.bmp", "./img/placa08.bmp",
					   "./img/placa09.bmp", "./img/placa10.bmp", "./img/placa11.bmp"
					   , "./img/placa12.bmp", "./img/placa13.bmp"};

	char fileName[25], elementName[35];

	for (int idx = 0; idx < 13; idx++) {

		Imagem *img = abreImagem(files[idx], 3);
		//Imagem *img = abreImagem("./img/placa01.bmp", 3);
		if (!img)
		{
			printf("Erro abrindo a imagem.\n");
			exit(1);
		}
		Coordenada coo;

		coo.x = 2;
		coo.y = 2;

		//Imagem *kernel = criaImagem(3, 3, 1);

		//Imagem *dilatada = criaImagem(img->largura, img->altura, 3);

		//Imagem *canny = criaImagem(img->largura, img->altura, 3);Descomentar  quando for fazer o chamfer

		//Imagem *chamfer = criaImagem(img->largura, img->altura, 1); Descomentar quando for fazer o chamfer

		Imagem *red = criaImagem(img->largura, img->altura, 3);

		Imagem *redMap = criaImagem(img->largura, img->altura, 1);

		redDetector(img, red);

		sprintf(fileName, "./resultados/%d-red.bmp", idx+1);
		salvaImagem(red, fileName);


		/*detectorCanny(img, 3, 0.01, 0.4, 1, canny);

		sprintf(fileName, "./resultados/%d-canny.bmp", idx+1);
		salvaImagem(canny, fileName);

		dilata(canny, kernel, coo, dilatada);
		salvaImagem(dilatada, "Dilatada");*/


		redMapping(img, redMap);
		sprintf(fileName, "./resultados/%d-redMap1.bmp", idx+1);
		salvaImagem(redMap, fileName);
		sprintf(fileName, "./resultados/%d-redMap2.bmp", idx+1);
		ComponenteConexo *componentes;
		qtde = rotulaFloodFill(redMap, &componentes, LARGURA_MIN, ALTURA_MIN, N_PIXELS_MIN, idx+1);
		salvaImagem(redMap, fileName);

		//printf("\n%d - %d elementos\n", idx+1,qtde);

		for(i=1;i<=qtde;i++){
			sprintf(elementName, "./resultados/%d-element%d.bmp", idx+1,i);
			Imagem *element = abreImagem(elementName, 3);
			if(checkPlaca(element)){
				printf("\n %d-element%d é praca!! \n", idx+1,i);
			} else {
				printf("\n %d-element%d não é praca!! (eu acho) \n",  idx+1,i);
			}
			//Imagem *elementCanny = criaImagem(element->largura, element->altura, 3);
			//detectorCanny(element, 5, 0.01, 0.4, 1, elementCanny);
			//sprintf(fileName, "./resultados/%d-element%dcanny.bmp", idx+1,i);
			//salvaImagem(elementCanny, fileName);
			//dilata(canny, kernel, coo, dilatada);
			destroiImagem(element);
			//destroiImagem(elementCanny);
		}

		

		/*for (i = 0; i < chamfer->altura; i++)
		{
			for (j = 0; j < chamfer->largura; j++)
			{
				chamfer->dados[0][i][j] = 100000000;
			}
		}

		Imagem *tst = abreImagem("saida.bmp", 3);
		chamferFunc(canny, chamfer);

		salvaImagem(chamfer, "chamfer_teste.bmp");*/

		//destroiImagem(canny);
		//destroiImagem(chamfer);
		destroiImagem(img);
		destroiImagem(red);
		destroiImagem(redMap);
	}

	return (0);
}

int checkPlaca(Imagem *img){

	Imagem *gabarito = abreImagem("./gabarito.bmp", 3);

	Imagem *resize = criaImagem(gabarito->largura, gabarito->altura, 3);
	
	redimensionaBilinear(img, resize);

	int count = 0;

	for(int j = 0; j<resize->altura; j++){
		for(int i = 0; i < resize->largura; i++){
			if(gabarito->dados[0][j][i] > 0 && resize->dados[0][j][i] > 0){
				count++;
			}
		}
	}
	
	int total = resize->altura*resize->largura;

	float perc = ((float)count/total)*100.0;

	destroiImagem(resize);
	destroiImagem(gabarito);

	//printf("\n %d de %d ---> %.2f%% \n", count, total, perc);

	if(perc > 90.0f){
		return 1;
	} else {
		return 0;
	}
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

int recurssChamfer(Imagem *canny, Imagem *chamfer, float value, int y, int x)
{
	chamfer->dados[0][y][x] = value;

	value = value + 0.01;

	if(x > 0 && value < THRESHOLD) {
		if(chamfer->dados[0][y][x-1] > value && canny->dados[0][y][x-1] != 1) {
			//printf("\nIF 1\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
			//if(recurssChamfer(canny, chamfer, value, y, x-1) == 1) return 1;
			recurssChamfer(canny, chamfer, value, y, x-1);
		}
	}

	if(y > 0 && value < THRESHOLD) {
		if(chamfer->dados[0][y-1][x] > value && canny->dados[0][y-1][x] != 1) {
			//printf("\nIF 2\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
			//if(recurssChamfer(canny, chamfer, value, y-1, x) == 1) return 1;
			recurssChamfer(canny, chamfer, value, y-1, x);
		}
	}

	if(x < canny->largura-1 && value < THRESHOLD) {
		if(chamfer->dados[0][y][x+1] > value && canny->dados[0][y][x+1] != 1) {
			//printf("\nIF 3\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
			//if(recurssChamfer(canny, chamfer, value, y, x+1) == 1) return 1;
			recurssChamfer(canny, chamfer, value, y, x+1);
		}
	}

	if(y < canny->altura-1 && value < THRESHOLD) {
		if(chamfer->dados[0][y+1][x] > value && canny->dados[0][y+1][x] != 1) {
			//printf("\nIF 4\nAltura: %d\nLargura: %d\nValue: %f\n", y, x, value);
			//if(recurssChamfer(canny, chamfer, value, y+1, x) == 1) return 1;
			recurssChamfer(canny, chamfer, value, y+1, x);
		}
	}


	return 1;
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

    int j, k;
		float r, g, b;
    for (j = 0; j < in->altura; j++){
        for (k = 0; k < in->largura; k++){

			r = in->dados[0][j][k];
			g = in->dados[1][j][k];
			b = in->dados[2][j][k];

      if ((r - g > 0.1 && r - b > 0.1)) {
        out->dados[0][j][k] = 1.0;
			} else {
        out->dados[0][j][k] = 0.0;
			}
		}
	}
}

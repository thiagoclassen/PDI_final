/*============================================================================*/
/* FILTRAGEM ESPACIAL                                                         */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e fun��es para filtragem espacial. */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "base.h"
#include "filtros2d.h"

/*============================================================================*/
/* FILTRAGEM LINEAR GEN�RICA                                                  */
/*============================================================================*/
/** Filtragem espacial na horizontal/vertical, usando um vetor de coeficientes.
 * Use para aplicar filtros 1D ou uma componente de um filtro separ�vel.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float* coef: vetor de coeficientes.
 *             int n: n�mero de valores no vetor de coeficientes.
 *             int vertical: se != 0, aplica o filtro na vertical.
 *
 * Valor de retorno: nenhum. */

void filtro1D (Imagem* in, Imagem* out, float* coef, int n, int vertical)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: filtro1D: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if ((!vertical && n >= in->largura*2) || (vertical && n >= in->altura*2))
    {
        printf ("ERRO: filtro1D: vetor de coeficientes grande demais!\n");
        exit (1);
    }

    int channel, row, col, i, pos;
    float soma;
    int centro = n/2;

    // Para cada centro poss�vel...
    for (channel = 0; channel < in->n_canais; channel++)
    {
        for (row = 0; row < in->altura; row++)
        {
            for (col = 0; col < in->largura; col++)
            {
                soma = 0;

                // Percorre o vetor de coeficientes.
                for (i = -centro; i <= centro; i++)
                {
                    // Tratamento de margens com imagem espelhada.
                    pos = ((vertical)? row : col) + i;

                    if (pos < 0)
                        pos = -pos;
                    else if (!vertical && pos >= in->largura)
                        pos = in->largura*2 - pos - 2;
                    else if (vertical && pos >= in->altura)
                        pos = in->altura*2 - pos - 2;

                    if (vertical)
                        soma += in->dados [channel][pos][col] * coef [centro + i];
                    else
                        soma += in->dados [channel][row][pos] * coef [centro + i];
                }

                out->dados [channel][row][col] = soma;
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
/** Filtragem espacial "for�a bruta", usando uma matriz de coeficientes. Use
 * para aplicar filtros 2D.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float** coef: matriz de coeficientes.
 *             int altura: altura da matriz.
 *             int largura: largura da matriz.
 *             int transposta: se != 0, percorre a matriz transposta.
 *
 * Valor de retorno: nenhum. */

void filtro2D (Imagem* in, Imagem* out, float** coef, int altura, int largura, int transposta)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: filtro2D: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (largura >= in->largura*2 || altura >= in->altura*2)
    {
        printf ("ERRO: filtro2D: vetor de coeficientes grande demais!\n");
        exit (1);
    }

    int channel, row, col, i, j, filter_row, filter_col;
    float soma;
    int centro_x = largura/2;
    int centro_y = altura/2;

    // Para cada centro poss�vel...
    for (channel = 0; channel < in->n_canais; channel++)
    {
        for (row = 0; row < in->altura; row++)
        {
            for (col = 0; col < in->largura; col++)
            {
                soma = 0;

                // Percorre a matriz de coeficientes.
                for (i = 0; i < altura; i++)
                    for (j = 0; j < largura; j++)
                    {
                        // Pega a linha e a coluna na imagem. Faz ratamento de margens com imagem espelhada.
                        filter_row = row - centro_y + ((transposta)? j : i);
                        filter_col = col - centro_x + ((transposta)? i : j);

                        if (filter_row < 0)
                            filter_row = -filter_row;
                        else if (filter_row >= in->altura)
                            filter_row = in->altura*2 - filter_row - 2;

                        if (filter_col < 0)
                            filter_col = -filter_col;
                        else if (filter_col >= in->largura)
                            filter_col = in->largura*2 - filter_col - 2;

                        soma += in->dados [channel][filter_row][filter_col] * coef [i][j];
                }

                out->dados [channel][row][col] = soma;
            }
        }
    }
}

/*============================================================================*/
/* FILTRO DA M�DIA                                                            */
/*============================================================================*/
/** Implementa��o de box blur usando uma imagem integral.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             int altura: altura da janela.
 *             int largura: largura da janela.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

void blur (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: blur: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (altura % 2 == 0 || largura % 2 == 0)
    {
        printf ("ERRO: blur: a janela deve ter largura e altura impares.\n");
        exit (1);
    }

    if (altura == 1 && largura == 1)
    {
        copiaConteudo (in, out);
        return;
    }

    // Primeiro calcula a imagem integral.
    Imagem* integral = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    int channel, row, col;
    for (channel = 0; channel < in->n_canais; channel++)
    {
        // Soma em linhas.
        for (row = 0; row < in->altura; row++)
        {
            integral->dados [channel][row][0] = in->dados [channel][row][0];

            for (col = 1; col < in->largura; col++)
                integral->dados [channel][row][col] = in->dados [channel][row][col] + integral->dados [channel][row][col-1];
        }

        // Agora soma na vertical.
        for (row = 1; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
                integral->dados [channel][row][col] += integral->dados [channel][row-1][col];
    }

    // Agora calcula as m�dias.
    int top, left, bottom, right;
    float soma, area;
    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
            {
                top = MAX (-1, row-altura/2-1);
                left = MAX (-1, col-largura/2-1);
                bottom = MIN (in->altura-1, row+altura/2);
                right = MIN (in->largura-1, col+largura/2);

                soma = ((top >= 0 && left >= 0)? integral->dados [channel][top][left] : 0) +
                        integral->dados [channel][bottom][right] -
                        ((left >= 0)? integral->dados [channel][bottom][left] : 0) -
                        ((top >= 0)? integral->dados [channel][top][right] : 0);

                area = (right-left)*(bottom-top);
                out->dados [channel][row][col] = soma/area;
            }

    if (!buffer)
        destroiImagem (integral);
}

/*============================================================================*/
/* FILTRO GAUSSIANO                                                           */
/*============================================================================*/
/** Implementa��o de filtro Gaussiano. Como a filtragem Gaussiana � separ�vel,
 * fazemos o processo primeiro na horizontal e depois na vertical. Os
 * coeficientes s�o calculados com base no sigma, que deve ser positivo,
 * exceto em 3 casos especiais: -3, -5 e -7. Nestes casos, o valor n�o �
 * interpretado como o sigma, e s�o usados os coeficientes cl�ssicos para a
 * aproxima��o 3x3 com sigma=0.8; 5x5 com sigma=1.1; e 7x7 com sigma=1.4,
 * respectivamente.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float sigmax: desvio padr�o na horizontal. Deve ser positivo,
 *               ou um dos 3 valores especiais.
 *             float sigmay: desvio padr�o na vertical. Deve ser positivo,
 *               ou um dos 3 valores especiais.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

// Fun��o auxiliar usada para calcular os coeficientes horizontais ou verticais seguindo uma fun��o Gaussiana.
void _filtroGaussianoCalculaCoef (int largura, float sigma, float* coef)
{
    // Valores especiais.
    if (sigma == -3)
    {
        coef [0] = coef [2] = 0.25f;
        coef [1] = 0.5f;
		return;
    }

    if (sigma == -5)
    {
        coef [0] = coef [4] = 0.0625f;
        coef [1] = coef [3] = 0.25f;
        coef [2] = 0.375f;
		return;
    }

    if (sigma == -7)
    {
        coef [0] = coef [6] = 0.03125f;
        coef [1] = coef [5] = 0.109375f;
        coef [2] = coef [4] = 0.21875f;
        coef [3] = 0.28125f;
		return;
    }

    int i;
    float x, computado, total;

    int centro = largura/2;

    // Calcula.
    coef [centro] = total = 1;
    for (i = 1; i <= centro; i++)
    {
        x = i;
        computado = exp (-(x*x)/(2*sigma*sigma));
        coef [centro-i] = computado;
        coef [centro+i] = computado;
        total += computado*2;
    }

    // Normaliza.
    total = 1.0f/total;
    for (i = 0; i < largura; i++)
        coef [i] *= total;
}

// Micro-fun��o que retorna o n�mero de coeficientes para um dado sigma.
int _filtroGaussianoNCoef (float sigma)
{
    // Primeiro, os valores especiais.
    if (sigma == -3)
        return (3);

    if (sigma == -5)
        return (5);

    if (sigma == -7)
        return (7);

    int n = (int) (sigma*4.0f + 0.5f);
    n |= 1; // Precisa ser �mpar.
    return (n);
}

void filtroGaussiano (Imagem* in, Imagem* out, float sigmax, float sigmay, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: filtroGaussiano: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    Imagem* img_aux = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    // Calcula o tamanho da matriz de coeficientes.
    int largura = _filtroGaussianoNCoef (sigmax);
    int altura = _filtroGaussianoNCoef (sigmay);

    float* coef = malloc (sizeof (float) * MAX (largura, altura));

    // Filtra na horizontal.
    _filtroGaussianoCalculaCoef (largura, sigmax, coef);
    filtro1D (in, img_aux, coef, largura, 0);

    // Agora na vertical.
    if (sigmax != sigmay)
        _filtroGaussianoCalculaCoef (altura, sigmay, coef);
    filtro1D (img_aux, out, coef, altura, 1);

    free (coef);
    if (!buffer)
        destroiImagem (img_aux);
}

/*============================================================================*/
/* UNSHARP MASKING                                                            */
/*============================================================================*/
/** Realce de bordas usando unsharp masking. Borra a imagem, verifica a
 * diferen�a da original para a borrada, e soma a diferen�a em lugares onde
 * ela for grande.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float sigma: par�metro para a suaviza��o Gaussiana.
 *             float threshold: altera apenas regi�es onde a diferen�a � grande.
 *             float mult: multiplica as diferen�as por este valor. Valores
 *               mais altos implicam em bordas mais destacadas.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum. */

void unsharpMasking (Imagem* in, Imagem* out, float sigma, float threshold, float mult, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: unsharpMasking: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    // Come�a borrando a imagem...
    filtroGaussiano (in, out, sigma, sigma, buffer);

    // Verifica a diferen�a da imagem original para a borrada.
    soma (in, out, 1, -1, out);

    // Agora percorre a imagem e procura lugares com diferen�a grande. Real�a a imagem nestes locais.
    int channel, row, col;
    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
                if (out->dados [channel][row][col] > threshold)
                    out->dados [channel][row][col] = in->dados [channel][row][col] + mult*out->dados [channel][row][col];
                else
                    out->dados [channel][row][col] = in->dados [channel][row][col];
}

/*============================================================================*/
/* FILTRO DA MEDIANA                                                          */
/*============================================================================*/
/** Implementa��o cl�ssica do filtro da mediana aproximado. Mant�m um
 * histograma que � atualizado quando a janela desliza. Usamos um histograma
 * de 256 faixas, supondo que os pixels est�o no intervalo [0,1]. Isso nos d�
 * resultados exatos para entradas e sa�das com 8bpp. Este n�o � o algoritmo
 * mais r�pido poss�vel, mas � simples de implementar.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             int altura: altura da janela.
 *             int largura: largura da janela.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

// Fun��o auxiliar, chamada pela filtroMediana8bpp para obter a mediana a partir de um histograma de 256 faixas.
int _medianaHistograma8bpp (int* hist, int total)
{
    int i, soma = 0;

    total /= 2; // Para quando encontrar a metade dos valores.

    for (i = 0; i < 256; i++)
    {
        soma += hist [i];
        if (soma >= total)
            return (i);
    }

    return (255);
}

void filtroMediana8bpp (Imagem* in, Imagem* out, int altura, int largura)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: filtroMediana8bpp: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (altura % 2 == 0 || largura % 2 == 0)
    {
        printf ("ERRO: filtroMediana8bpp: a janela deve ter largura e altura impares.\n");
        exit (1);
    }

    int channel, row, col, i, j;
    int w = largura/2;
    int h = altura/2;
    int saiu = -1, entrou = -1; // Para verificar se o histograma mudou.

    // Este � o histograma. Para trabalhar nele, teremos que reconverter a imagem para 8bpp.
    int histograma [256];
    int n_histograma = 0;

    unsigned char** in8bpp;
    in8bpp = malloc (sizeof (unsigned char*) * in->altura);
    for (i = 0; i < in->altura; i++)
        in8bpp [i] = malloc (sizeof (unsigned char) * in->largura);

    // Para cada canal...
    for (channel = 0; channel < in->n_canais; channel++)
    {
        // Converte para 8bpp.
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
                in8bpp [row][col] = float2uchar (in->dados [channel][row][col]);

        // Para cada linha...
        for (row = 0; row < in->altura; row++)
        {
            // Inicializa o histograma.
            for (i = 0; i < 256; i++)
                histograma [i] = 0;

            // Obt�m o primeiro histograma para esta linha.
            for (i = MAX (0, row-h); i <= MIN (in->altura-1, row+h); i++)
                for (j = 0; j <= w; j++)
                    histograma [in8bpp [i][j]]++;
            n_histograma = (row+h-MAX(0,row-h)+1)*(w+1); // N�mero de valores na primeira janela.

            // Obt�m a mediana para o primeiro pixel.
            out->dados [channel][row][0] = _medianaHistograma8bpp (histograma, n_histograma) / 255.0f;

            // Agora vai para as colunas seguintes.
            for (col = 1; col < in->largura; col++)
            {
                 // Remove a coluna que sai, adiciona a que entra.
                for (i = MAX (0, row-h); i <= MIN (in->altura-1, row+h); i++)
                {
                    j = col-w-1;
                    if (j >= 0)
                    {
                        saiu = in8bpp [i][j];
                        histograma [saiu]--;
                        n_histograma--;
                    }
                    else
                        saiu = -1;

                    j = col+w;
                    if (j < in->largura)
                    {
                        entrou = in8bpp [i][j];
                        histograma [entrou]++;
                        n_histograma++;
                    }
                    else
                        entrou = -1;
                }

                // Obt�m a mediana para esta posi��o.
                if (saiu == entrou)
                    out->dados [channel][row][col] = out->dados [channel][row][col-1];
                else
                    out->dados [channel][row][col] = _medianaHistograma8bpp (histograma, n_histograma) / 255.0f;
            }
        }
    }

    for (i = 0; i < in->altura; i++)
        free (in8bpp [i]);
    free (in8bpp);
}

/*----------------------------------------------------------------------------*/
/** Filtro da mediana para imagens bin�rias - implementa��o r�pida com imagem
 * integral. Basta somar os valore em cada vizinhan�a e verificar se a soma
 * � maior do que a metade da �rea da vizinhan�a. O c�digo � *muito* parecido
 * com o do box blur.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             int altura: altura da janela.
 *             int largura: largura da janela.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum */

void filtroMedianaBinario (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: filtroMedianaBinario: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (altura % 2 == 0 || largura % 2 == 0)
    {
        printf ("ERRO: filtroMedianaBinario: a janela deve ter largura e altura impares.\n");
        exit (1);
    }

    if (altura == 1 && largura == 1)
    {
        copiaConteudo (in, out);
        return;
    }

    // Primeiro calcula a imagem integral.
    Imagem* integral = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    int channel, row, col;
    for (channel = 0; channel < in->n_canais; channel++)
    {
        // Soma em linhas.
        for (row = 0; row < in->altura; row++)
        {
            integral->dados [channel][row][0] = in->dados [channel][row][0];

            for (col = 1; col < in->largura; col++)
                integral->dados [channel][row][col] = in->dados [channel][row][col] + integral->dados [channel][row][col-1];
        }

        // Agora soma na vertical.
        for (row = 1; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
                integral->dados [channel][row][col] += integral->dados [channel][row-1][col];
    }

    // Agora verifica onde a soma � maior do que a metade da �rea.
    int top, left, bottom, right;
    float soma, area = (largura*altura)/2.0f;

    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
            {
                top = MAX (-1, row-altura/2-1);
                left = MAX (-1, col-largura/2-1);
                bottom = MIN (in->altura-1, row+altura/2);
                right = MIN (in->largura-1, col+largura/2);

                soma = ((top >= 0 && left >= 0)? integral->dados [channel][top][left] : 0) +
                        integral->dados [channel][bottom][right] -
                        ((left >= 0)? integral->dados [channel][bottom][left] : 0) -
                        ((top >= 0)? integral->dados [channel][top][right] : 0);

                if (soma > area) // A maior parte dos pixels � branca.
                    out->dados [channel][row][col] = 1.0f;
                else
                    out->dados [channel][row][col] = 0;
            }

    if (!buffer)
        destroiImagem (integral);
}

/*============================================================================*/
/* M�XIMOS E M�NIMOS LOCAIS                                                   */
/*============================================================================*/
/** Localiza o m�ximo local em uma vizinhan�a da imagem. Consideramos que o
 * m�ximo local � um filtro espacial n�o-linear e separ�vel.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             int altura: altura da janela.
 *             int largura: largura da janela.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

int _maxLocalMaxLinha (Imagem* in, int channel, int row, int inicio, int fim) // Fun��o auxiliar para a maxLocal.
{
    int col;

    if (inicio < 0) inicio = 0;
    if (fim >= in->largura) fim = in->largura-1;

    int pos_max = inicio;
    for (col = inicio+1; col <= fim; col++)
        if (in->dados [channel][row][col] >= in->dados [channel][row][pos_max])
            pos_max = col;

    return (pos_max);
}

float _maxLocalMaxColuna (Imagem* in, int channel, int col, int inicio, int fim) // Fun��o auxiliar para a maxLocal.
{
    int row;

    if (inicio < 0) inicio = 0;
    if (fim >= in->altura) fim = in->altura-1;

    int pos_max = inicio;
    for (row = inicio+1; row <= fim; row++)
        if (in->dados [channel][row][col] >= in->dados [channel][pos_max][col])
            pos_max = row;

    return (pos_max);
}

void maxLocal (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: maxLocal: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (altura % 2 == 0 || largura % 2 == 0)
    {
        printf ("ERRO: maxLocal: a janela deve ter largura e altura impares.\n");
        exit (1);
    }

    Imagem* img_aux = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    int channel, row, col, pos_max;
    int w = largura/2; // largura = 2w+1
    int h = altura/2; // algtura = 2h+1
    for (channel = 0; channel < in->n_canais; channel++)
    {
        // Primeiro na horizontal.
        for (row = 0; row < in->altura; row++)
        {
            // Acha o maior valor dentro da primeira janela desta linha.
            pos_max = _maxLocalMaxLinha (in, channel, row, 0, w);
            img_aux->dados [channel][row][0] = in->dados [channel][row][pos_max];

            for (col = 1; col < in->largura; col++)
            {
                // Remove a coluna que sai, adiciona a que entra.
                if (pos_max == col-w-1)
                    pos_max = _maxLocalMaxLinha (in, channel, row, col-w, col+w);
                else if (col+w < in->largura && in->dados [channel][row][col+w] >= in->dados [channel][row][pos_max])
                    pos_max = col+w;

                img_aux->dados [channel][row][col] = in->dados [channel][row][pos_max];
            }
        }

        // Agora na vertical.
        for (col = 0; col < in->largura; col++)
        {
             // Acha o maior valor dentro da primeira janela desta coluna.
             pos_max = _maxLocalMaxColuna (img_aux, channel, col, 0, h);
             out->dados [channel][0][col] = img_aux->dados [channel][pos_max][col];

             for (row = 1; row < in->altura; row++)
             {
                if (pos_max == row-h-1)
                    pos_max = _maxLocalMaxColuna (img_aux, channel, col, row-h, row+h);
                else if (row+h < in->altura && img_aux->dados [channel][row+w][col] >= img_aux->dados [channel][pos_max][col])
                    pos_max = row+w;

                out->dados [channel][row][col] = img_aux->dados [channel][pos_max][col];
             }
        }
    }

    if (!buffer)
        destroiImagem (img_aux);
}

/*----------------------------------------------------------------------------*/
/** Localiza o m�nimo local em uma vizinhan�a da imagem. Consideramos que o
 * m�nimo local � um filtro espacial n�o-linear e separ�vel.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             int altura: altura da janela.
 *             int largura: largura da janela.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum (usa a imagem de sa�da). */

int _minLocalMinLinha (Imagem* in, int channel, int row, int inicio, int fim) // Fun��o auxiliar para a minLocal.
{
    int col;

    if (inicio < 0) inicio = 0;
    if (fim >= in->largura) fim = in->largura-1;

    int pos_min = inicio;
    for (col = inicio+1; col <= fim; col++)
        if (in->dados [channel][row][col] <= in->dados [channel][row][pos_min])
            pos_min = col;

    return (pos_min);
}

float _minLocalMinColuna (Imagem* in, int channel, int col, int inicio, int fim) // Fun��o auxiliar para a minLocal.
{
    int row;

    if (inicio < 0) inicio = 0;
    if (fim >= in->altura) fim = in->altura-1;

    int pos_min = inicio;
    for (row = inicio+1; row <= fim; row++)
        if (in->dados [channel][row][col] <= in->dados [channel][pos_min][col])
            pos_min = row;

    return (pos_min);
}

void minLocal (Imagem* in, Imagem* out, int altura, int largura, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: minLocal: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (altura % 2 == 0 || largura % 2 == 0)
    {
        printf ("ERRO: minLocal: a janela deve ter largura e altura impares.\n");
        exit (1);
    }

    Imagem* img_aux = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    int channel, row, col, pos_min;
    int w = largura/2; // largura = 2w+1
    int h = altura/2; // algtura = 2h+1
    for (channel = 0; channel < in->n_canais; channel++)
    {
        // Primeiro na horizontal.
        for (row = 0; row < in->altura; row++)
        {
            // Acha o menor valor dentro da primeira janela desta linha.
            pos_min = _minLocalMinLinha (in, channel, row, 0, w);
            img_aux->dados [channel][row][0] = in->dados [channel][row][pos_min];

            for (col = 1; col < in->largura; col++)
            {
                // Remove a coluna que sai, adiciona a que entra.
                if (pos_min == col-w-1)
                    pos_min = _minLocalMinLinha (in, channel, row, col-w, col+w);
                else if (col+w < in->largura && in->dados [channel][row][col+w] <= in->dados [channel][row][pos_min])
                    pos_min = col+w;

                img_aux->dados [channel][row][col] = in->dados [channel][row][pos_min];
            }
        }

        // Agora na vertical.
        for (col = 0; col < in->largura; col++)
        {
             // Acha o maior valor dentro da primeira janela desta coluna.
             pos_min = _minLocalMinColuna (img_aux, channel, col, 0, h);
             out->dados [channel][0][col] = img_aux->dados [channel][pos_min][col];

             for (row = 1; row < in->altura; row++)
             {
                if (pos_min == row-h-1)
                    pos_min = _minLocalMinColuna (img_aux, channel, col, row-h, row+h);
                else if (row+h < in->altura && img_aux->dados [channel][row+w][col] <= img_aux->dados [channel][pos_min][col])
                    pos_min = row+w;

                out->dados [channel][row][col] = img_aux->dados [channel][pos_min][col];
             }
        }
    }

    if (!buffer)
        destroiImagem (img_aux);
}

/*============================================================================*/
/* MORFOLOGIA MATEM�TICA                                                      */
/*============================================================================*/
/** Cria um kernel circular para operadores morfol�gicos. � somente uma imagem
 * preta com um c�rculo branco preenchido.
 *
 * Par�metros: int largura: largura do kernel (= di�metro do c�rculo).
 *
 * Valor de retorno: uma imagem contendo o kernel. Lembre-se de desaloc�-la! */

Imagem* criaKernelCircular (int largura)
{
    if (largura % 2 == 0)
    {
        printf ("ERRO: criaKernelCircular: o kernel deve ter largura impar.\n");
        exit (1);
    }

    Imagem* kernel = criaImagem (largura, largura, 1);
    int i, j, raio = largura/2, dx, dy;

    for (i = 0; i < largura; i++)
        for (j = 0; j < largura; j++)
        {
            dx = j - raio;
            dy = i - raio;
            if ((int) (sqrtf (dx*dx + dy*dy) + 0.5f) <= raio)
                kernel->dados [0][i][j] = 1.0f;
            else
                kernel->dados [0][i][j] = 0;
        }

    return (kernel);
}

/*----------------------------------------------------------------------------*/
/** Dilata��o morfol�gica para imagens bin�rias. Usei aqui uma implementa��o
 * extremeamente simples (e ineficiente).
 *
 * Par�metros: Imagem* in: imagem de entrada.
 *             Imagem* kernel: kernel para a dilata��o.
 *             Coordenada centro: centro do kernel.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *
 * Valor de retorno: nenhum. */

void dilata (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: dilata: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    // Processa cada canal independentemente.
    int channel, row, col, row2, col2, set;
    int aesq = centro.x, adir = kernel->largura-1-centro.x, acima = centro.y, abaixo = kernel->altura-1-centro.y;

    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
            {
                // Verifica se tem um pixel branco sob o kernel. Pode parar se encontrar.
                set = 0;
                for (row2 = MAX (0, row-acima); !set && row2 <= MIN (in->altura-1, row+abaixo); row2++)
                    for (col2 = MAX (0, col-aesq); !set && col2 <= MIN (in->largura-1, col+adir); col2++)
                        if (kernel->dados [0][row2-row+centro.y][col2-col+centro.x] > 0.5f && in->dados [channel][row2][col2] > 0.5f)
                            set = 1;

                if (set)
                    out->dados [channel][row][col] = 1.0f;
                else
                    out->dados [channel][row][col] = 0;
            }
}

/*----------------------------------------------------------------------------*/
/** Eros�o morfol�gica para imagens bin�rias. Usei aqui uma implementa��o
 * extremeamente simples (e ineficiente).
 *
 * Par�metros: Imagem* in: imagem de entrada.
 *             Imagem* kernel: kernel para a eros�o.
 *             Coordenada centro: centro do kernel.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *
 * Valor de retorno: nenhum. */

void erode (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: erode: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    // Processa cada canal independentemente.
    int channel, row, col, row2, col2, set;
    int aesq = centro.x, adir = kernel->largura-1-centro.x, acima = centro.y, abaixo = kernel->altura-1-centro.y;

    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
            {
                // Verifica se tem um pixel preto sob o kernel. Pode parar se encontrar.
                set = 1;
                for (row2 = MAX (0, row-acima); set && row2 <= MIN (in->altura-1, row+abaixo); row2++)
                    for (col2 = MAX (0, col-aesq); set && col2 <= MIN (in->largura-1, col+adir); col2++)
                        if (kernel->dados [0][row2-row+centro.y][col2-col+centro.x] > 0.5f && in->dados [channel][row2][col2] <= 0.5f)
                            set = 0;

                if (set)
                    out->dados [channel][row][col] = 1.0f;
                else
                    out->dados [channel][row][col] = 0;
            }
}

/*----------------------------------------------------------------------------*/
/** Abertura morfol�gica: eros�o seguida de dilata��o.
 *
 * Par�metros: Imagem* in: imagem de entrada.
 *             Imagem* kernel: kernel para a dilata��o/eros�o.
 *             Coordenada centro: centro do kernel.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum. */

void abertura (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: abertura: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    Imagem* img_aux = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    erode (in, kernel, centro, img_aux);
    dilata (img_aux, kernel, centro, out);

    if (!buffer)
        destroiImagem (img_aux);
}

/*----------------------------------------------------------------------------*/
/** Fechamento morfol�gico: dilata��o seguida de eros�o.
 *
 * Par�metros: Imagem* in: imagem de entrada.
 *             Imagem* kernel: kernel para a dilata��o/eros�o.
 *             Coordenada centro: centro do kernel.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             Imagem* buffer: uma imagem com o mesmo tamanho da imagem de
 *               entrada. Pode ser usada quando se quer evitar a aloca��o do
 *               buffer interno. Use NULL se quiser usar o buffer interno.
 *
 * Valor de retorno: nenhum. */

void fechamento (Imagem* in, Imagem* kernel, Coordenada centro, Imagem* out, Imagem* buffer)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais ||
        (buffer && (in->largura != buffer->largura || in->altura != buffer->altura || in->n_canais != buffer->n_canais)))
    {
        printf ("ERRO: fechamento: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    Imagem* img_aux = (buffer)? buffer : criaImagem (in->largura, in->altura, in->n_canais);

    dilata (in, kernel, centro, img_aux);
    erode (img_aux, kernel, centro, out);

    if (!buffer)
        destroiImagem (img_aux);
}


/*============================================================================*/
/* GRADIENTES                                                                 */
/*============================================================================*/
/** Filtro de Sobel. Cria um kernel apropriado e aplica como um filtro espacial
 * linear gen�rico.
 *
 * Par�metros: Imagem* in: iimagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             Imagem* out: imagem de sa�da. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             int tamanho: tamanho do filtro. Deve ser 1, 3, 5 ou 7. O filtro
 *               de tamanho 1 na verdade usa o kernel simples [-1, 0, 1].
 *             int vertical: 1 para o filtro vertical, 0 para o horizontal.
 *             int escalado: se != 0, os valores s�o escalados para n�o
 *               sa�rem do intervalo [-1,1] (considerando que a imagem de
 *               entrada est� normalizada).
 *
 * Valor de retorno: nenhum. */

// Sub-fun��o chamada para criar o kernel.
float** _filtroSobelCriaKernel (int tamanho, int escalado)
{
    int i, j;
    float escala = 0;

    // Cria.
    float** kernel = malloc (sizeof (float*) * tamanho);
    for (i = 0; i < tamanho; i++)
        kernel [i] = malloc (sizeof (float) * tamanho);

    // Preenche.
    if (tamanho == 3)
    {
        for (i = 0; i < 3; i++) // Bloco central de 0s.
            kernel [i][1] = 0;

        kernel [0][0] = kernel [2][0] = -1;
        kernel [1][0] = -2;

        for (i = 0; i < 3; i++)
            kernel [i][2] = -kernel [i][0]; // Parte espelhada com sinal invertido.

        escala = 1.0f/4.0f;
    }
    else if (tamanho == 5)
    {
        for (i = 0; i < 5; i++) // Bloco central de 0s.
            kernel [i][2] = 0;

        kernel [0][0] = kernel [4][0] = -1;
        kernel [1][0] = kernel [3][0] = -4;
        kernel [2][0] = -7;
        kernel [0][1] = kernel [4][1] = -2;
        kernel [1][1] = kernel [3][1] = -10;
        kernel [2][1] = -17;

        for (i = 0; i < 5; i++)
            for (j = 0; j < 2; j++) // Parte espelhada com sinal invertido.
                kernel [i][4-j] = -kernel [i][j];

         escala = 1.0f/58.0f;
    }
    else
    {
        for (i = 0; i < 7; i++) // Bloco central de 0s.
            kernel [i][3] = 0;

        kernel [0][0] = kernel [6][0] = -1;
        kernel [1][0] = kernel [5][0] = -4;
        kernel [2][0] = kernel [4][0] = -9;
        kernel [3][0] = -13;
        kernel [0][1] = kernel [6][1] = -3;
        kernel [1][1] = kernel [5][1] = -11;
        kernel [2][1] = kernel [4][1] = -26;
        kernel [3][1] = -34;
        kernel [0][2] = kernel [6][2] = -3;
        kernel [1][2] = kernel [5][2] = -13;
        kernel [2][2] = kernel [4][2] = -30;
        kernel [3][2] = -40;

         for (i = 0; i < 7; i++)
            for (j = 0; j < 3; j++) // Parte espelhada com sinal invertido.
                kernel [i][6-j] = -kernel [i][j];

        escala = 1.0f/287.0f;
    }

    if (escalado)
        for (i = 0; i < tamanho; i++)
            for (j = 0; j < tamanho; j++)
                kernel [i][j] *= escala;

    return (kernel);
}

void filtroSobel (Imagem* in, Imagem* out, int tamanho, int vertical, int escalado)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: filtroSobel: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (tamanho != 1 && tamanho != 3 && tamanho != 5 && tamanho != 7)
    {
        printf ("ERRO: filtroSobel: o tamanho do filtro deve ser 1, 3, 5 ou 7.\n");
        exit (1);
    }

    // O kernel de tamanho 1 na verdade � o gradiente simples [-1, 0, 1].
    if (tamanho == 1)
    {
        float kernel [3];
        kernel [0] = -1;
        kernel [1] = 0;
        kernel [2] = 1;
        filtro1D (in, out, kernel, 3, vertical);
        return;
    }

    // Para kernels de tamanho 3, 5 ou 7. Primeiro cria o kernel.
    float** kernel = _filtroSobelCriaKernel (tamanho, escalado);

    // Agora, chama a fun��o gen�rica.
    filtro2D (in, out, kernel, tamanho, tamanho, vertical);

    // Desaloca.
    int i;
    for (i = 0; i < tamanho; i++)
        free (kernel [i]);
    free (kernel);
}

/*----------------------------------------------------------------------------*/
/** Calcula os gradientes da imagem.
 *
 * Par�metros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               processa cada canal independentemente.
 *             int tamanho_sobel: tamanho do filtro de Sobel a se usar para
 *               obter as derivadas. Deve ser 1, 3, 5 ou 7. O filtro
 *               de tamanho 1 na verdade usa o kernel simples [-1, 0, 1].
 *             Imagem* dx: se for != NULL, usa esta imagem para as derivadas
 *               horizontais. Neste caso, deve ter o mesmo tamanho da imagem
 *               de entrada. Se for == NULL, o buffer � criado internamente.
 *             Imagem* dy: se for != NULL, usa esta imagem para as derivadas
 *               verticais. Neste caso, deve ter o mesmo tamanho da imagem
 *               de entrada. Se for == NULL, o buffer � criado internamente.
 *             Imagem* mag: imagem de sa�da para as magnitudes. Deve ter o
 *               mesmo tamanho da imagem de entrada.
 *             Imagem* ori: imagem de sa�da para as orienta��es. Os valores s�o
 *               dados em radianos, no intervalo [0,2pi). Deve ter o mesmo
 *               tamanho da imagem de entrada.
 *
 * Valor de retorno: nenhum. */

void computaGradientes (Imagem* in, int tamanho_sobel, Imagem* dx, Imagem* dy, Imagem* mag, Imagem* ori)
{
    if (in->largura != mag->largura || in->altura != mag->altura || in->n_canais != mag->n_canais ||
        in->largura != ori->largura || in->altura != ori->altura || in->n_canais != ori->n_canais ||
        (dx && (in->largura != dx->largura || in->altura != dx->altura || in->n_canais != dx->n_canais)) ||
        (dy && (in->largura != dy->largura || in->altura != dy->altura || in->n_canais != dy->n_canais)))
    {
        printf ("ERRO: computaGradientes: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    if (tamanho_sobel != 1 && tamanho_sobel != 3 && tamanho_sobel != 5 && tamanho_sobel != 7)
    {
        printf ("ERRO: computaGradientes: o tamanho do filtro deve ser 1, 3, 5 ou 7.\n");
        exit (1);
    }

    // Cria os buffers internos, se preciso.
    Imagem* _dx = (dx)? dx : criaImagem (in->largura, in->altura, in->n_canais);
    Imagem* _dy = (dy)? dy : criaImagem (in->largura, in->altura, in->n_canais);

    // Filtro de Sobel.
    if (tamanho_sobel == 1)
    {
        // O kernel de tamanho 1 na verdade � o gradiente simples [-1, 0, 1].
        float kernel [3];
        kernel [0] = -1;
        kernel [1] = 0;
        kernel [2] = 1;
        filtro1D (in, _dx, kernel, 3, 0);
        filtro1D (in, _dy, kernel, 3, 1);
    }
    else
    {
        float** kernel = _filtroSobelCriaKernel (tamanho_sobel, 1);
        filtro2D (in, _dx, kernel, tamanho_sobel, tamanho_sobel, 0);
        filtro2D (in, _dy, kernel, tamanho_sobel, tamanho_sobel, 1);
    }

    // Magnitude e orienta��o.
    int channel, row, col;
    float x, y;
    float PI_TIMES_2 = 2.0f * M_PI;
    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
            {
                x = _dx->dados [channel][row][col];
                y = _dy->dados [channel][row][col];
                mag->dados [channel][row][col] = sqrtf (x*x + y*y);
                ori->dados [channel][row][col] = atan2 (y, x);

                if (ori->dados [channel][row][col] < 0) // Coloca no intervalo [0,2PI). A sa�da da atan2 � no intervalo [-PI,PI].
                    ori->dados [channel][row][col] += PI_TIMES_2;
            }


    // Pronto.
    if (!dx)
        destroiImagem (_dx);
    if (!dy)
        destroiImagem (_dy);
}
/*============================================================================*/
/** Detector de bordas de Canny. */

// Sub-função chamada para isolar os máximos locais na imagem com as magnitudes dos gradientes.
// Usa (= destroi!) a imagem com as orientações para a saída.
void _cannyIsolaMaximosLocais (Imagem* mag, Imagem* ori)
{
    Coordenada vizinho1, vizinho2;
    int channel, row, col;

    for (channel = 0; channel < mag->n_canais; channel++)
        for (row = 0; row < mag->altura; row++)
            for (col = 0; col < mag->largura; col++)
            {
                float o = ori->dados [channel][row][col];
                if (o >= M_PI)
                    o -= M_PI; // Coloca no intervalo [0,PI).

                // Quantiza a orientação neste pixel. Seleciona 2 vizinhos para comparar.
                if (o < 0.3926990817) // Horizontal.
                {
                    vizinho1.y = vizinho2.y = row;
                    vizinho1.x = col-1;
                    vizinho2.x = col+1;
                }
                else if (o < 1.1780972451) // Diagonal.
                {
                    vizinho1.y = row-1;
                    vizinho1.x = col-1;
                    vizinho2.y = row+1;
                    vizinho2.x = col+1;
                }
                else if (o < 1.9634954085) // Vertical.
                {
                    vizinho1.x = vizinho2.x = col;
                    vizinho1.y = row-1;
                    vizinho2.y = row+1;
                }
                else if (o < 2.7488935719) // Diagonal
                {
                    vizinho1.y = row+1;
                    vizinho1.x = col-1;
                    vizinho2.y = row-1;
                    vizinho2.x = col+1;
                }
                else // Horizontal.
                {
                    vizinho1.y = vizinho2.y = row;
                    vizinho1.x = col-1;
                    vizinho2.x = col+1;
                }

                // Compara com os vizinhos.
                int eh_maximo = 1;
                if (vizinho1.x >= 0 && vizinho1.x < mag->largura && vizinho1.y >= 0 && vizinho1.y < mag->altura && mag->dados [channel][row][col] < mag->dados [channel][vizinho1.y][vizinho1.x])
                    eh_maximo = 0;
                if (vizinho2.x >= 0 && vizinho2.x < mag->largura && vizinho2.y >= 0 && vizinho2.y < mag->altura && mag->dados [channel][row][col] < mag->dados [channel][vizinho2.y][vizinho2.x])
                    eh_maximo = 0;

                if (eh_maximo)
                    ori->dados [channel][row][col] = mag->dados [channel][row][col];
                else
                    ori->dados [channel][row][col] =  0;
            }
}

// Sub-função recursiva, chamada para binarizar e inundar com histerese.
void _cannyFloodHisterese (Imagem* in, int channel, int row, int col, float threshold, Imagem* out)
{
    // Marca o pixel.
    out->dados [0][row][col] = 1;

    // Pixel já marcado!
    in->dados [channel][row][col] = 0;

    // Desce recursivamente na vizinhança-8.
    if (row > 0 && col > 0 && in->dados [channel][row-1][col-1] >= threshold) // TL
        _cannyFloodHisterese (in, channel, row-1, col-1, threshold, out);

    if (row > 0 && in->dados [channel][row-1][col] >= threshold) // T
        _cannyFloodHisterese (in, channel, row-1, col, threshold, out);

    if (row > 0 && col < in->largura-1 && in->dados [channel][row-1][col+1] >= threshold) // TR
        _cannyFloodHisterese (in, channel, row-1, col+1, threshold, out);

    if (col > 0 && in->dados [channel][row][col-1] >= threshold) // L
        _cannyFloodHisterese (in, channel, row, col-1, threshold, out);

    if (col < in->largura-1 && in->dados [channel][row][col+1] >= threshold) // R
        _cannyFloodHisterese (in, channel, row, col+1, threshold, out);

    if (row < in->altura-1 && col > 0 && in->dados [channel][row+1][col-1] >= threshold) // BL
        _cannyFloodHisterese (in, channel, row+1, col-1, threshold, out);

    if (row < in->altura-1 && in->dados [channel][row+1][col] >= threshold) // B
        _cannyFloodHisterese (in, channel, row+1, col, threshold, out);

    if (row < in->altura-1 && col < in->largura-1 && in->dados [channel][row+1][col+1] >= threshold) // BL
        _cannyFloodHisterese (in, channel, row+1, col+1, threshold, out);
}

void detectorCanny (Imagem* img, int tamanho_sobel, float t_inferior, float t_superior, int usa_proporcao, Imagem* out)
{
    if (img->largura != out->largura || img->altura != out->altura)
    {
        printf ("ERRO: detectorCanny: as imagens precisam ter o mesmo tamanho.\n");
        exit (1);
    }

    // Extrai os gradientes da imagem.
    Imagem* mag = criaImagem (img->largura, img->altura, img->n_canais);
    Imagem* ori = criaImagem (img->largura, img->altura, img->n_canais);
    computaGradientes (img, tamanho_sobel, NULL, NULL, mag, ori);

    // Isola máximos locais. A saíca fica na imagem que originalmente tinha as orientações dos gradientes.
    _cannyIsolaMaximosLocais (mag, ori);

    int channel, row, col;
    if (!usa_proporcao) // Faz do jeito tradicional, com limiar superior e inferior.
    {
        // Começa com a saída com tudo em 0.
        for (row = 0; row < out->altura; row++)
            for (col = 0; col < out->largura; col++)
                out->dados [0][row][col] = 0;

        // Limiarização com histerese. Processa todos os canais da imagem. O resultado é uma imagem de 1 canal, com as bordas localizadas em qualquer canal.
        for (channel = 0; channel < img->n_canais; channel++)
            for (row = 0; row < out->altura; row++)
                for (col = 0; col < out->largura; col++)
                    if (ori->dados [channel][row][col] >= t_superior)
                        _cannyFloodHisterese (ori, channel, row, col, t_inferior, out);

    }
    else // Considera t_inferior e t_superior como proporções de pixels que devem estar setados.
    {
        int meta_inferior = img->altura * img->largura * t_inferior + 0.5f;
        int meta_superior = img->altura * img->largura * t_superior + 0.5f;

        float thresh_inf = 0;
        float thresh_sup = sqrtf (2.0f);
        float thresh = (thresh_inf + thresh_sup)/2.0f;

        while (1)
        {
            // Copia os máximos locais para a imagem que originalmente tinha as magnitudes dos gradientes.
            copiaConteudo (ori, mag);

            // Começa com a saída com tudo em 0.
            for (row = 0; row < out->altura; row++)
                for (col = 0; col < out->largura; col++)
                    out->dados [0][row][col] = 0;

            // Limiarização com histerese. Processa todos os canais da imagem. O resultado é uma imagem de 1 canal, com as bordas localizadas em qualquer canal.
            for (channel = 0; channel < img->n_canais; channel++)
                for (row = 0; row < out->altura; row++)
                    for (col = 0; col < out->largura; col++)
                        if (mag->dados [channel][row][col] >= thresh)
                            _cannyFloodHisterese (mag, channel, row, col, thresh/2.5f, out);

            // Conta quantos pixels ficaram setados.
            int n_setados = 0;
            for (row = 0; row < img->altura; row++)
                for (col = 0; col < img->largura; col++)
                    if (out->dados [0][row][col] > 0.5f)
                        n_setados++;

            // Conseguimos?
            if (n_setados < meta_inferior)
                thresh_sup = thresh;
            else if (n_setados > meta_superior)
                thresh_inf = thresh;
            else
                break;

            if (fabs (thresh_inf - thresh_sup) < FLT_EPSILON)
                break; // Impossível chegar exatamente ao intervalo desejado.

            thresh = (thresh_inf + thresh_sup)/2.0f;
        }
    }

    // Limpa tudo.
    destroiImagem (mag);
    destroiImagem (ori);
}

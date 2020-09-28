/*
 * GeraArqDocBase.cpp

 *
 *  Created on: 30/01/2014
 *      Author: jhoyce
 */

#define MAX_SIZE_TEXT 1024 * 1024 * 100

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

using namespace std;

typedef struct
{
	int id_arq;

	long offset_text;
	int size_text;

	long offset_title;
	int size_title;

	long offset_anchor;
	int size_anchor;

	long offset_url;
	int size_url;

	long offset_outlink;
	int size_outlink;
} IDXbase;

void limparString(char *string)
{

	int tamString = strlen(string);

	for (int i = 0; i < tamString; i++)
	{
		string[i] = ' ';
	}
}

/** objetivo: Salvar a base de documentos no formato reconhecido pelo indexador ****/
/* cria um aruivo textual para cada documento da base *****/
/* cria um arquivo com structs IDXBase criados para cada dcoumento ***/
/* os structs servirao para auxiliar o indexador a recuperar o conteudo no arquivo de cada doumento **/
int main(int argc, char **argv)
{

	FILE *arquivoBase, *arqText, *arquivoIndice;
	char nomeImg[200000];
	char palavraVisual[200000];
	char nomeArqText[200000];
	char ch;
	char espaco = ' ';
	int cont = 0;
	string linha;
	palavraVisual[0] = '\0';
	IDXbase idxBase;

	//abrindo para leitura o arquivo com as palavras que descrevem cada documento da base
	/** exemplo linha do arquivo
	 *423.jpg # 	 palavra1 palavra2
	 * <nomeImg><espaco><cerquilha><tab><palavra1><espaco><palavra2>...
	 **/
	// arquivoBase = fopen(argv[1], "r"); //ex: "/home/joyce/base_infoweb/converte_Base_Formato_Reconhecido_Indexador/YC/imgs_Sift_1000_20000.db "
	//ex: "/home/joyce/base_infoweb/converte_Base_Formato_Reconhecido_Indexador/YC/imgs_Sift_1000_20000.db "
	//sr.open(argv[1]);

	//criando o arquivo onde serao salvos os structs referentes aos docs da base

	int posPalavra = 0, posNomeImg = 0, qtdeImgLida = 0;
	int qtdePalavrasLidas = 0, tamanhoTotalPalavra = 0;

	float docId = 0;
	bool lendoNomeImg = true, lendoPalavra = false;

	//------------------

	DIR *dir = nullptr;
	struct dirent *lsdir = nullptr;
	dir = opendir(argv[1]);

	/* print all the files and directories within directory */
	while ((lsdir = readdir(dir)) != nullptr)
	{
		cout << "Element name: " << lsdir->d_name << " " << int(lsdir->d_type) << " " << lsdir->d_ino << endl;
		if (lsdir->d_type == 4) // Is directory, so skip
		{
			continue;
		}

		sprintf(nomeArqText, "%s%s%s", argv[1], argv[2], lsdir->d_name);

		printf("Path: %s\n", nomeArqText);
		cout << "path: " << nomeArqText << endl;

		if ((arquivoBase = fopen((char *)nomeArqText, "r")) == nullptr)
		{
			cout << "Failure to open: " << nomeArqText << endl;
			exit(1);
		}

		sprintf(nomeArqText, "%sidx", argv[3]);

		// ex: /home/joyce/base_infoweb/converte_Base_Formato_Reconhecido_Indexador/YC/Base_YC_SIFT_1000_20000/idxe
		if ((arquivoIndice = fopen(nomeArqText, "a")) == nullptr)
		{
			cout << "Failure to open: " << nomeArqText << endl;
			exit(1);
		}

		/* lendo arquivo carcater a caracter*/
		while ((ch = fgetc(arquivoBase)) != EOF)
		{

			if (lendoNomeImg and ch == '#')
			{ //condicao qdo acaba de ler o nome da img

				docId++;

				//monta nome do arquivo de conteudo que sera gera gerado para o doc
				sprintf(nomeArqText, "%st.%0.f", argv[3], docId); //ex: /home/joyce/base_infoweb/converte_Base_Formato_Reconhecido_Indexador/YC/Base_YC_SIFT_1000_20000/text

				//cria arq de conteudo
				arqText = fopen(nomeArqText, "w");

				//salva no struct
				idxBase.id_arq = docId;
				idxBase.offset_text = 0;

				//printf("\ndocId: %f Img: %s\n", docId, nomeImg);

				posNomeImg = 0;
				lendoNomeImg = false;
				limparString(nomeImg);
				limparString(nomeArqText);
				continue;
			}
			else if (ch == '\t')
			{ // condicao que indica que o proximo caracater eh o inicio de uma palavra

				lendoPalavra = true;
				continue;
			}
			else if ((cont == 1000 || ch == ' ') and lendoPalavra == true)
			{ //parametrizando para ler proximas palavras

				if (palavraVisual[0] == ' ' or palavraVisual[0] == '\t' or palavraVisual[0] == '\0')
				{
					continue;
				}
				for (char c : palavraVisual)
				{
					if (c != ' ')
					{
						//escrevendo palavra no arquivo
						fprintf(arqText, "%c", c);

						tamanhoTotalPalavra = tamanhoTotalPalavra + sizeof(char);
					}
					else
					{
						break;
					}
				}
				fprintf(arqText, "%c", espaco);
				tamanhoTotalPalavra = tamanhoTotalPalavra + sizeof(char);

				qtdePalavrasLidas++;
				limparString(palavraVisual);
				cont = 0;
				posPalavra = 0;
				continue;
			}
			else if (ch == '\n')
			{ //condicao quando se passa para a proxima linha do arquivo

				if (palavraVisual[0] == '\t' or palavraVisual[0] == '\0')
				{
					continue;
				}

				//escrever ultima palavra no arquivo
				for (char ch : palavraVisual)
				{
					if (ch != ' ')
					{
						// escrevendo palavra no arquivo
						fprintf(arqText, "%c", ch);

						tamanhoTotalPalavra = tamanhoTotalPalavra + sizeof(char);
					}
					else
					{
						break;
					}
				}

				// salva valor no struct
				idxBase.size_text = tamanhoTotalPalavra;
				//excreve struct no arquivo
				fwrite(&idxBase, sizeof(idxBase), 1, arquivoIndice);
				// fecha arqText
				if (arqText)
				{
					fclose(arqText);
					arqText = nullptr;
				}
				qtdeImgLida++;
				lendoPalavra = false;
				lendoNomeImg = true;
				limparString(palavraVisual);
				posPalavra = 0;
				tamanhoTotalPalavra = 0;

				continue;
			}

			//lendo caracteres para formar o nome da img
			if (lendoNomeImg == true)
			{

				if (ch == ' ' or ch == '\t' or ch == '\0')
				{
					continue;
				}
				nomeImg[posNomeImg] = ch;
				posNomeImg = posNomeImg + 1;
			}

			//lendo caracteres para formar a palavra
			if (lendoPalavra == true)
			{
				if (ch == ' ' or ch == '\t' or ch == '\0')
				{
					continue;
				}
				cont++;
				//printf("%c", ch);
				palavraVisual[posPalavra] = ch;
				posPalavra = posPalavra + 1;
			}
		}

		fclose(arquivoBase);
		fclose(arquivoIndice);
	}

	closedir(dir);

	printf("Qtde Imgs Lidas: %d ", qtdeImgLida);
	printf("Qtde Palavras Lidas: %d ", qtdePalavrasLidas);

	return 0;
}

#include "IndiceInvertido.h"
#include <algorithm>
#include <iostream>
#include <list>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>

class LeitorLista
{
public:
	FILE *fp;
	_tipo_buffer *buffer;
	unsigned char btat;
	_tipo_buffer *out;
	int tam_lista;
	long offset;
	char fileatual[1000];

	LeitorLista()
	{
		fp = NULL;
		buffer = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA + 5];
		fileatual[0] = 0;
	}

	~LeitorLista()
	{
		delete buffer;
	}
	void Inicializa(long _tam_lista, char *arq, long inicio)
	{
		if (strcmp(fileatual, arq))
		{
			if (fp)
				fclose(fp);
			strcpy(fileatual, arq);
			fp = fopen(arq, "rb");
		}

		fseek(fp, inicio, SEEK_SET);

		if (_tam_lista * 2 > SIZE_BUFFER_LEITOR_LISTA + 5)
			fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA + 5, fp);
		else
			fread(buffer, sizeof(_tipo_buffer), _tam_lista * 2, fp);

		out = buffer;
		btat = 0;
		tam_lista = _tam_lista;
		offset = 0;
		//	offset = (inicio/sizeof(unsigned int));
	}

	int getDoc(unsigned int *doc, unsigned int *info, long *_offset, unsigned int *_bitoffset)
	{
		if (tam_lista == 0)
			return 0;

		tam_lista--;

		/*Final do buffer. Deve-se ler mais*/
		if (out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
		{

			memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], 5 * sizeof(int));
			fread(buffer + 5, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA, fp);
			out -= SIZE_BUFFER_LEITOR_LISTA;
			offset += SIZE_BUFFER_LEITOR_LISTA * sizeof(_tipo_buffer);
		}

		*doc = RawDeltaDecode(out, btat);
		*info = RawDeltaDecode(out, btat);
		*_offset = offset + (out - buffer);
		//	*_offset = offset + (out-buffer) * sizeof(_tipo_buffer);
		//		printf("offset %d\n",*_offset);
		*_bitoffset = btat;
		return 1;
	}
};

typedef struct post
{ //Struct da Lista Invertida ordenada por Documento
	unsigned int doc;
	unsigned int freq;
} tPost;

int main(int argc, char **argv)
{
	long offset;
	unsigned int num_docs_colecao = (unsigned int)atoi(argv[2]);

	unordered_map<float, int> normas;

	if (argc != 4)
	{
		printf("Execute\n\t%s <path_indice> <num_doc_colecao> <1->Vetorial; 2->BM25>\n", argv[0]);
		exit(1);
	}

	int tipo_norma = atoi(argv[3]);
	time_t start = time(NULL);

	FILE *fidx = NULL;
	FILE *fidf = NULL;
	FILE *fnorma = NULL;
	IDX idx;
	int term_id = 0;

	char arq[4096];
	int num_doc;
	_tipo_buffer docid = 0;

	LeitorLista leitor;
	unsigned int doc, freq = 0, bitoffset = 0;

	sprintf(arq, "%s%s", argv[1], NOME_NORMA);
	if ((fnorma = fopen(arq, "wb")) == NULL)
	{
		printf("gera_normas::Impossivel criar o arquivo de normas: (%s)\n", arq);
		exit(1);
	}

	sprintf(arq, "%s%s", argv[1], NOME_IDX);
	if ((fidx = fopen(arq, "r+")) == NULL)
	{
		printf("Nao foi possivel abrir o arquivo de idx %s\n", arq);
		exit(1);
	}

	size_t size;
	float *vetIdf = nullptr;

	FILE *fft;

	if (true || tipo_norma != 1)
	{
		sprintf(arq, "%s%s", argv[1], NOME_IDF);
		if ((fidf = fopen(arq, "r+")) == NULL)
		{
			printf("Nao foi possivel abrir o arquivo de idf %s\n", arq);
			exit(1);
		}

		sprintf(arq, "%s%s", argv[1], "FT");
		if ((fft = fopen(arq, "w+")) == NULL)
		{
			printf("Nao foi possivel criar o arquivo de norma %s\n", arq);
			exit(1);
		}

		fseek(fidf, 0, SEEK_END);
		size = ftell(fidf) / sizeof(float);
		vetIdf = new float[size];
		fseek(fidf, 0, SEEK_SET);
		fread(vetIdf, size, sizeof(float), fidf);
		fclose(fidf);
	}

	int nTerms = 0;

	do
	{

		fread(&idx, sizeof(IDX), 1, fidx);
		if (feof(fidx))
			break;

		if (idx.freq_tam_lista == 0)
		{
			term_id++;
			continue;
		}
		fwrite(&idx.freq_tam_lista, sizeof(unsigned), 1, fft);
		nTerms++;

		/*Abre o arquivo de listas de frequencia*/
		sprintf(arq, "%s%s%d", argv[1], NOME_FREQUENCIA, idx.id_arq);
		leitor.Inicializa(idx.freq_tam_lista, arq, idx.freq_inicio_termo);

		if (leitor.getDoc(&docid, &freq, &offset, &bitoffset))
		{
			num_doc++;
		}
		else
		{
			printf("erro de leitura, primeiro decodificado vazio\n");
			exit(1);
		}
		if (tipo_norma == 1)
		{
			/*UTI*/
			normas[docid] += vetIdf[term_id] * vetIdf[term_id] * freq * freq; //Vetorial
		}
		else
		{
			normas[docid] += freq;
		}

		while (leitor.getDoc(&doc, &freq, &offset, &bitoffset))
		{
			if (doc != 0)
			{
				docid += doc;
				if (tipo_norma == 1)
				{

					normas[docid] += vetIdf[term_id] * vetIdf[term_id] * freq * freq; //Vetorial
				}
				else
				{
					normas[docid] += freq;
				}
			}
		}
		term_id++;

	} while (1);

	//fwrite( normas, sizeof(float), num_docs_colecao, fnorma );

	printf("Numero de termos =  %d\n", nTerms);
	printf("Norma gerada em %.2lf \n", difftime(time(NULL), start) / 60);

	// vector<float> normas_vec(normas.begin(), normas.end());
	for (auto [key, value] : normas)
	{
		fwrite(&value, sizeof(float), 1, fnorma);
	}

	fclose(fft);
	if (tipo_norma == 1)
		delete vetIdf;
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <list>
#include "../../../libs-iw/indexer/include/IndiceInvertido.h"


class LeitorLista {
public:
	FILE *fp;
	_tipo_buffer * buffer;
	unsigned char btat;
	_tipo_buffer *out;
	int tam_lista;
	long offset;
	char fileatual[1000];

	LeitorLista() {
		fp = NULL;
		buffer = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA + 5];
		fileatual[0] = 0;
	}

	~LeitorLista() {
		delete buffer;
	}

	void Inicializa(long _tam_lista, char *arq, long inicio) {
		if (strcmp(fileatual, arq)) {
			if (fp)
				fclose(fp);
			strcpy(fileatual, arq);
			fp = fopen(arq, "rb");
		}

		fseek(fp, inicio, SEEK_SET);

		if (_tam_lista * 2 > SIZE_BUFFER_LEITOR_LISTA + 5)
			fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA + 5,
					fp);
		else
			fread(buffer, sizeof(_tipo_buffer), _tam_lista * 2, fp);

		out = buffer;
		btat = 0;
		tam_lista = _tam_lista;
		offset = 0;
		//	offset = (inicio/sizeof(unsigned int));
	}

	int getDoc(unsigned int *doc, unsigned int *info, long *_offset,
			unsigned int *_bitoffset) {
		if (tam_lista == 0)
			return 0;

		tam_lista--;

		/*Final do buffer. Deve-se ler mais*/
		if (out >= &buffer[SIZE_BUFFER_LEITOR_LISTA]) {

			memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], 5 * sizeof(int));
			fread(buffer + 5, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA,
					fp);
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

typedef struct post { //Struct da Lista Invertida ordenada por Documento
	unsigned int doc;
	unsigned int freq;
} tPost;



int main(int argc, char **argv) {
	int nl = 0, nw = 0;
	int nDocsBloco;
	tPost doc_a_doc[1024];
	FILE *ftermo_out = NULL;
	_tipo_buffer *out_freq;

	_tipo_buffer *buffer_lista_freq = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA	+ SIZE_FOLGA_LEITOR_LISTA];

	if (argc != 3) {
		printf("Execute:\n\t%s <path_indice> <1->UTI; 2->Vetorial; 3->BM25>\n",argv[0]);
		exit(1);
	}

	time_t start = time(NULL);
	FILE *ffreq_skip = NULL;
	FILE *fidx = NULL;
	FILE *fmax_score = NULL;
	FILE *fmin_score = NULL;
	FILE *fidf = NULL;
	FILE *fft;
	FILE *fnorma = NULL;
	float pre;
	float *vetIdf;
	float *vetNorm;
	float maxScore, maxScoreBlock, score, maxTodos = 0;
	float minScore;
	float *buffer_skip_freq_max_score;

	int totalNDocs;
	int term_id = 0;
	int tipo_score = atoi(argv[2]);
	int salto, num_doc;
	int id_arq_ant = -1;
	unsigned int doc, freq=0, bitoffset=0;

	size_t size;
	unsigned *vetFT;
	long offsetOut = 0;
	long Glob_offsetOut = 0;
	long *buffer_skip_freq_offset, offset=0;
	unsigned char *buffer_skip_freq_bitat;
	char arq[4096];

	
	IDX idx;

	_tipo_buffer *buffer_skip_freq_docid;
	_tipo_buffer docid=0;

	LeitorLista leitor;


	buffer_skip_freq_docid  = (_tipo_buffer*)malloc(sizeof(_tipo_buffer)*SIZE_MAX_SKIP_LIST);
	buffer_skip_freq_offset = (long*)malloc(sizeof(long)*SIZE_MAX_SKIP_LIST);
	buffer_skip_freq_bitat  = (unsigned char*)malloc(sizeof(unsigned char)*SIZE_MAX_SKIP_LIST);
	buffer_skip_freq_max_score = (float*)malloc(sizeof(float)*SIZE_MAX_SKIP_LIST);


	///Abre arquivo de IDX
	sprintf(arq, "%s%s", argv[1], NOME_IDX);
	if ((fidx = fopen(arq, "r+")) == NULL) {
		printf("Nao foi possivel abrir o arquivo de idx %s\n", arq);
		exit(1);
	}

	int termoVazio =0;

	///Se NÃo for a unfção de UTI abre os arquivos de IDF, FT e Norma
	if (tipo_score != 1) {
		sprintf(arq, "%s%s", argv[1], NOME_IDF);
		if ((fidf = fopen(arq, "r+")) == NULL) {
			printf("Nao foi possivel abrir o arquivo de idf %s\n", arq);
			exit(1);
		}
		fseek(fidf, 0, SEEK_END);
		size = ftell(fidf) / sizeof(float);
		vetIdf = new float[size];
		fseek(fidf, 0, SEEK_SET);
		fread(vetIdf, size, sizeof(float), fidf);
		fclose(fidf);

		sprintf(arq, "%sFT", argv[1]);
		if ((fft = fopen(arq, "r+")) == NULL) {
			printf("Nao foi possivel abrir o arquivo de idf %s\n", arq);
			exit(1);
		}

		fseek(fft, 0, SEEK_END);
		size = ftell(fft) / sizeof(unsigned);
		printf("tamanho do FT: %d\n", size);
		vetFT = new unsigned[size];
		fseek(fft, 0, SEEK_SET);
		fread(vetFT, sizeof(unsigned), size, fft);
		fclose(fft);

		sprintf(arq, "%s%s", argv[1], NOME_NORMA);
		if ((fnorma = fopen(arq, "r+")) == NULL) {
			printf("Nao foi possivel abrir o arquivo de norma %s\n", arq);
			exit(1);
		}
		fseek(fnorma, 0, SEEK_END);
		size = ftell(fnorma) / sizeof(float);
		vetNorm = new float[size];
		totalNDocs = size;
		fseek(fnorma, 0, SEEK_SET);
		fread(vetNorm, size, sizeof(float), fnorma);
		fclose(fnorma);
	}

	sprintf(arq, "%smax_score", argv[1]);
	fmax_score = fopen(arq, "w");

	sprintf(arq, "%smin_score", argv[1]);
	fmin_score = fopen(arq, "w");
	int maxDoc =0;

	///Para cada Termo do vocabulario [percorre o Indice Invertido]
	do {
		buffer_lista_freq[0] = 0;
		out_freq = buffer_lista_freq;
		maxScore = minScore = 0;
		nDocsBloco = 0;

		
		
		///verifica se a lista chegou ao fim ou se a lista invertida do termo esta vazia.
		fread(&idx, sizeof(IDX), 1, fidx);
		if (feof(fidx))
			break;
			
		if (idx.freq_tam_lista == 0 ) {
			term_id++;
			fwrite(&maxScore, 1, sizeof(float), fmax_score);
			fwrite(&minScore, 1, sizeof(float), fmin_score);
			termoVazio++;
			//printf("termo vazio!!!!!\n");
			continue;
		}
	
		///Abre o arquivo de listas de frequencia
		sprintf(arq, "%s%s%d", argv[1], NOME_FREQUENCIA, idx.id_arq);

		leitor.Inicializa(idx.freq_tam_lista, arq, idx.freq_inicio_termo);

		pre = (float)3 * (float)(log( 1 + (float)(totalNDocs - vetFT[term_id] + 0.5)/(float)(vetFT[term_id] + 0.5))/log(2));



		///Se o arquivo de frequencia ou skipList atingir um limite de tamnho cria novos arquivo para continuar escrevendo a skipList
		if (id_arq_ant != idx.id_arq) {
			if (ffreq_skip)
				fclose(ffreq_skip);
			/*Cria o arquivo de skiplist para o  indice posicional*/
			sprintf(arq, "%s%s%d", argv[1], NOME_FREQ_SKIP, idx.id_arq);
				
			ffreq_skip = fopen(arq, "w+");
			id_arq_ant = idx.id_arq;
			if (ftermo_out)
				fclose(ftermo_out);
			/*Cria o arquivo de skiplist para o  indice corrigido*/
			sprintf(arq, "%s%s%d.newFormat", argv[1], NOME_FREQUENCIA, idx.id_arq);
			ftermo_out = fopen(arq, "w+");
			
			Glob_offsetOut = 0;

		}

		idx.freq_inicio_skip = ftell(ffreq_skip);
		//salto = SALTO_SKIPLIST;
		salto = 128;
		num_doc = 0;
		offset = 0;
		offsetOut = 0;
		nDocsBloco = 0;


		///Carrega o primeiro documento da lista invetida do termo
		if (leitor.getDoc(&docid, &freq, &offset, &bitoffset)) {
			offsetOut += 2;
			doc_a_doc[nDocsBloco].doc = docid;
			doc_a_doc[nDocsBloco].freq = freq;
			nl++;
			nDocsBloco++;
			num_doc++;
		} else {
			printf("erro de leitura, primeiro decodificado vazio\n");
			exit(1);
		}
		if (tipo_score == 1) {
			/*UTI*/
			score = freq;
		} else if (tipo_score == 2) {
			/*Vetorial*/
			score = (freq * vetIdf[term_id]* vetIdf[term_id]) / vetNorm[docid];
		} else if (tipo_score == 3) {
			/*BM25 com k1=2; b=0.75*/
			//	score = (freq * vetIdf[term_id] * 3.0) / (freq + 2.0 * ( 0.25 + 0.75 * vetNorm[docid]) );
			score = pre * (float) (freq) / ((float) (freq + 2* (0.25+ 0.75 * (float) (vetNorm[docid])/ (float) (130))));
		}
		
		
		///Inicializa as variaveis
		maxScore = score;
		minScore = score;
		maxScoreBlock = score;


		///Percorre cada documento de uma lista para gerar os blocos
		while (leitor.getDoc(&doc, &freq, &offset, &bitoffset)) {
			if (doc != 0) {
			
				offsetOut += 2;
				nl++;
				docid += doc;
				doc_a_doc[nDocsBloco].doc = docid;
				doc_a_doc[nDocsBloco].freq = freq;
				nDocsBloco++;
				
				if (docid > maxDoc) maxDoc = docid;
				
				
				if (tipo_score == 1) {
					/*UTI*/
					score = freq;
				} else if (tipo_score == 2) {
					/*Vetorial*/
					score = (freq * vetIdf[term_id]* vetIdf[term_id]) / vetNorm[docid];
					//cout<<score<<endl;
					
				} else if (tipo_score == 3) {
					/*BM25 com k1=2; b=0.75*/
					//score = (freq * vetIdf[term_id] * 3.0) / (freq + 2.0 * ( 0.25 + 0.75 * vetNorm[docid]) );
					score = pre*(float)(freq) / ((float)(freq + 2 * ( 0.25 + 0.75 * (float)(vetNorm[docid])/(float)(130))));
				}
				
				
				if(maxScore < score)	maxScore = score;
				if(minScore > score)    minScore = score;
				
				if(score < 0)
					printf("Zica %f %d %d %f %f\n" , score, docid, freq, vetIdf[term_id], vetNorm[docid]);
				
				num_doc ++;
				
				if (maxScoreBlock < score)
					maxScoreBlock = score;

				///Se o numero de documentos computados corresponde ao tamanho de um bloco, exreve o bloco na lista invertida
				if (num_doc % salto == 0) {
					
					//if(term_id == 3) printf("\nEOB:[%d] offset:[%d] bitoffset[%d] nDocsBloco[%d]", docid, offsetOut, bitoffset, nDocsBloco);
					
					buffer_skip_freq_docid[(num_doc / salto) - 1] 	  = docid;
					buffer_skip_freq_offset[(num_doc / salto) - 1]	  = offsetOut;
					buffer_skip_freq_bitat[(num_doc / salto) - 1]	  = bitoffset;
					buffer_skip_freq_max_score[(num_doc / salto) - 1] =	maxScoreBlock;
					maxScoreBlock = 0;

					/********************
					* grava o bloco comprimido em formato onde documentos sao armazenados antes das frequencias
					* ****************************/

					//if(term_id == 3353) printf("\n ");
					/// Grava  todos os documentos primeiro
					for (int i = 0; i < nDocsBloco; i++) {
						nw++;
						//	RawDeltaEncode((doc_a_doc[i].doc), out_freq, btat_freq);
						// linha abaixo substitui o RawDeltaEncode
						*out_freq = (_tipo_buffer) doc_a_doc[i].doc;
						out_freq++;

						//		   doc_gap = doc_a_doc[i].doc;
						if (out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA]) {
							fwrite(buffer_lista_freq, sizeof(_tipo_buffer), (out_freq-buffer_lista_freq), ftermo_out);
							
							// comentado no sem compressao	buffer_lista_freq[0] = *out_freq;
							out_freq = buffer_lista_freq;
						}
					}
					
					for (int i = 0; i < nDocsBloco; i++) {
						// RawDeltaEncode(doc_a_doc[i].freq, out_freq, btat_freq);
						// linha abaixo substitui o RawDeltaEncode
						*out_freq = (_tipo_buffer) doc_a_doc[i].freq;
						out_freq++;

						if (out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA]) {
							fwrite(buffer_lista_freq, sizeof(_tipo_buffer),	(out_freq - buffer_lista_freq), ftermo_out);
							//comentado no sem compressao	buffer_lista_freq[0] = *out_freq;
							out_freq = buffer_lista_freq;
						}
					}

					/********* fim da gravacao do indice no novo formato *************/
					nDocsBloco = 0;

				}
			}
		}///Fim da percorre documentos da lista
		
		idx.freq_tam_skip = (num_doc / salto);

		///Grava ultimo bloco que possui menos documentos que o suficiente para completar um bloco
		if (num_doc % salto != 0) {

			buffer_skip_freq_docid[(num_doc / salto)] = docid;
			buffer_skip_freq_offset[(num_doc / salto)] = offsetOut;
			buffer_skip_freq_bitat[(num_doc / salto)] = bitoffset;
			buffer_skip_freq_max_score[(num_doc / salto)] = maxScoreBlock;
			idx.freq_tam_skip++;
			/********************
			 * grava o bloco comprimido em formato onde documentos sao armazenados antes das frequencias
			 *****************************/

			// inclui um fim de documento para marcar o fim da lista
			/**
			 doc_a_doc[nDocsBloco].doc = 2000000000 ; //std::numeric_limits<unsigned int>::max();
			 nDocsBloco++;
			 offsetOut+=2;
			 */
			// Grava  todos os documentos primeiro
			// printf("gravando %d elementos do termo %d\n",doc_a_doc.size(),term_id);
			for (int i = 0; i < nDocsBloco; i++) {
				nw++;
				//	RawDeltaEncode((doc_a_doc[i].doc), out_freq, btat_freq);
				// linha abaixo substitui o RawDeltaEncode
				*out_freq = (_tipo_buffer) doc_a_doc[i].doc;
				out_freq++;

				//			doc_gap = (doc_a_doc[i].doc);
				if (out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA]) {
					fwrite(buffer_lista_freq, sizeof(_tipo_buffer),	(out_freq - buffer_lista_freq), ftermo_out);
					//comentado no sem compressao	buffer_lista_freq[0] = *out_freq;
					out_freq = buffer_lista_freq;
				}
			}
			for (int i = 0; i < nDocsBloco; i++) {
				//RawDeltaEncode(doc_a_doc[i].freq, out_freq, btat_freq);
				// linha abaixo substitui o RawDeltaEncode
				*out_freq = (_tipo_buffer) doc_a_doc[i].freq;
				out_freq++;

				if (out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA]) {
					fwrite(buffer_lista_freq, sizeof(_tipo_buffer),	(out_freq - buffer_lista_freq), ftermo_out);
					// comentado no sem compressao buffer_lista_freq[0] = *out_freq;
					out_freq = buffer_lista_freq;
				}
			}

			/********* fim da gravacao do indice no novo formato *************/

		} else {
			if (nDocsBloco) {
				printf("erro na gera skip\n");
				exit(1);
			}
		}

		if (out_freq != buffer_lista_freq) {
			fwrite(buffer_lista_freq, sizeof(_tipo_buffer),	out_freq - buffer_lista_freq, ftermo_out);
		}


		fwrite(buffer_skip_freq_docid, 	   sizeof(_tipo_buffer), idx.freq_tam_skip, ffreq_skip);
		fwrite(buffer_skip_freq_offset,    sizeof(long),         idx.freq_tam_skip, ffreq_skip);
		fwrite(buffer_skip_freq_bitat, 	   sizeof(char), 		 idx.freq_tam_skip, ffreq_skip);
		fwrite(buffer_skip_freq_max_score, sizeof(float),		 idx.freq_tam_skip, ffreq_skip);

		fseek(fidx, -sizeof(IDX), SEEK_CUR);

		idx.freq_inicio_termo = Glob_offsetOut; //ftell(ftermo_out);
		
		fwrite(&idx, sizeof(IDX), 1, fidx);
		Glob_offsetOut = Glob_offsetOut + (sizeof(unsigned int) * offsetOut);

		/*Salva o score maximo da lista*/
		fwrite(&maxScore, 1, sizeof(float), fmax_score);
		fwrite(&minScore, 1, sizeof(float), fmin_score);

		if (maxScore > maxTodos)
			maxTodos = maxScore;
		term_id++;

	} while (term_id);

	fclose(fidx);
	fclose(fmax_score);
	fclose(fmin_score);

	free(buffer_skip_freq_docid);
	free(buffer_skip_freq_offset);
	free(buffer_skip_freq_bitat);
	free(buffer_skip_freq_max_score);
	
	printf("\tSkiplist gerada em %.2lf - Maior Scores %.4f\n", difftime(time(NULL), start)/60, maxTodos);
	printf("\tTermos vazios %d/%d",termoVazio, term_id);

	return 0;
}


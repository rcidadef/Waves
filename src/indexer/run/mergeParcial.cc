#include <stdio.h>
#include <iostream>
#include <time.h>
#include "HashDoc.h"
#include "IndiceInvertido.h"
#include "Heap.h"
#include "prefixinmem.h"
#include "parse_query.h"
#include "Vocabulario.h"
#include "valstringuniao_utf8.h"
#include <fstream>
#include <iostream>
#include <string>
#include "DescritorInfoweb.h"

#define MAX_TERMOS 10000000

_tipo_id doc;
unsigned int numero_termos, i, p, cont_d=0;

int main(int argc, char **argv)
{
	time_t start2 = time(NULL);

	if(argc < 3)
	{
		printf("Execute\n\t%s <path_indice> <num_temporario>\n", argv[0]);
		exit(1);
	}

	IndiceInvertido lista(argv[1], FLAG_MERGE);

	cout << "Fazendo Merge dos " << argv[2] << " Parciais" << endl;

	start2 = time(NULL);
	lista.set_cont_temporarios( atoi(argv[2]));
	lista.InicializaMerge();
	lista.Merge();

	cout << "Merge terminado em " << difftime(time(NULL), start2)/60 << " minutos" << endl;

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct lista {
	char nome[40];
} Lista;

int main() {
	Lista *list;
	int i;
	
	list = (Lista *) malloc(3 * sizeof (Lista));
	
	strcpy(list[0].nome, "coala");
	strcpy(list[1].nome, "virginia");
	strcpy(list[2].nome, "spi");
	
	FILE *pa;
	
	pa = fopen("lista.bin", "wb");
	for (i = 0; i < 3; i++) {
		fwrite(&list[i], sizeof (Lista), 1, pa);
	}
	fclose(pa);
	return 0;
}
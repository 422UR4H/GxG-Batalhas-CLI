#include "estruturas.h"


// DIRETORIO

/*
char *dir = malloc(sizeof (char) * 100);

int getDiret() {
    int i = 1;

    while (_getcwd(dir, strlen(dir)) == NULL) {
        dir = (char *) realloc(dir, sizeof (char) * ++i);
        if (i > 10)    return FALSE;
    }
    return TRUE;
}
*/

int qtdFichas;
int qtdLists;

void getDiretorio(char *dir) {
    char str[700];
    // procurando o diretorio em que o executavel se encontra
    _getcwd(str, sizeof (str));
    strcat(str, "\\bds\\");
    strcpy(dir, str);
    return;
}

int getQtdFichas() {
    return qtdFichas;
}

void pushQtdFichas(int i) {
    qtdFichas = i;
}

int getQtdLists() {
    return qtdLists;
}

void pushQtdLists(int i) {
    qtdLists = i;
}

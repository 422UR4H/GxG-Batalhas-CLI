#ifndef algoritmo_h
#define algoritmo_h
#include "estruturas.h"



//######################### Funcoes -menos- utilizadas #########################

double divideVol(double);
int inicializaMat(int, int);
void calculaGasto(int, ATAQUES, GASTO*);
void gastaRyu(double, int);
void calculaDano(int, ATAQUES*);
int calculaDef(double, int);
int aplicaDano(int, double, ATAQUES, DANO*);
int aplicaGasto(int, GASTO);

//######################### Funcoes +mais+ Utilizaveis #########################

void backSave(void);
void zeraAtaques(ATAQUES*);

//#################################### bds #####################################

int leBD(char*);
int leVol(char*);
int leHP(char*);
int leMOP(char*);
void backuphps(char*);

//---------------------- BDs (funcoes +mais+ utilizaveis) ----------------------

void pegaNomeSkill(int, int);
void limpaChar(FILE*);

//########################### FUNCOES DE COMANDOS ##############################

void operaExtAl(int, int);
void zeraExtAl(int);
void zeraExtsAl(int);

//------------------------------ shu ------------------------------

void zeraShusAl(int);
void addShuAl(int, int);
void rodaShuAl(int, int);
void turnosAl(int, int, int);

//-------------------------------- atk e damage --------------------------------

void nerfAtkAl(int, int, int, PILHA*);
void nerfAtkAl2(PILHA*);
void perfDamageAl(int, int, PILHA*);

//------------------------------------- hp -------------------------------------

void editHpAl(int, int);
void operaHpAl(int, int);
void healaHpAl(int);

//------------------------------------ mop -------------------------------------

void editMopAl(int, int);
void operaMopAl(int, int);
void healaMopAl(int);

//------------------------------------- nen ------------------------------------

void useRealRyuAl(int, int, PILHA*);
void reflexRenAl(int, int, int*);
void zeraExt(int);
void zeraShus(int);
void gastaRenAl(void);
void useAopAl(int, PILHA*, int);
void useKenAl(int);
double showKensAl(int, int);

//----------------------------------- outros -----------------------------------

void resetPersAl(int);
void reWindAl(int);
// list
void leList(char*, List**);

#endif // ALGORITMO_H_INCLUDED

#ifndef interface_h
#define interface_h
#include "estruturas.h"



//######################### Funcoes -menos- utilizadas #########################

void printDados(int);
void showSkills(int);
int leVolSkill(int);
int leCorp(void);
int opcoes(void);
int getSkill(int);

//---------------------------- Funcoes de leAtaque -----------------------------

double leRyu(int);
double leInvestida(int, int);
int leDado(int*);
void leQtd(QTDS*);
int leMat(int);
int leEms(void);
int leAcerto(int);

//---------------------------- Funcoes de leDef --------------------------------

int leDefensor(int);
double leRyuDef(int);
int leDefAdc(void);
int leSeDefendeu(void);

//------------------------------------------------------------------------------

void printMiss(int, int);
void printDano(int, int, DANO, ATAQUES);
void printGasto(int, int, int);
void leComando(char*);
void help(int);

//######################### Funcoes +mais+ Utilizaveis #########################

void corrigeNome(char*);
void printError(char*);
void linha(void);
int conta(char*);
int valorChar(char*);

//------------------------------ minhas funcoes ------------------------------

void fgetspc(int, char*);

//########################### FUNCOES DE COMANDOS ##############################

int operaExtIn(int, int);
int operaExtsIn(int);
void showExtIn(int);
void showExtsIn(int);
void showAtkVolIn(int, int);
void zeraExtIn(char*);
void zeraExtsIn(void);

//------------------------------ shu ------------------------------

void zeraShusIn(void);
void assShuPrint(int, int, char*);
int addShuIn(int);
void addShuPrint(int, int, char*);
int addShusIn(void);
void rodaShuIn(int, int, int, char*);
int turnosIn1(int, int);
void turnosIn2(int, int, int, char*);

//-------------------------------- atk e damage --------------------------------

int nerfAtkIn(int, int);
void showDamageIn(int);

//------------------------------------- hp -------------------------------------

void hpPrint(int, int);
int editHpIn(int);
int operaHpIn(int, int);
int healaHpIn(void);
void healaHpPrint(int, int);
int healaFullHpsIn(int);
void healaFullHpsPrint(int);
void saveHpsIn(void);

//------------------------------------ mop -------------------------------------

void mopPrint(int, int);
int editMopIn(int);
int operaMopIn(int, int);
int healaMopIn(void);
void healaMopPrint(int, int);
int healaFullMopsIn(int);
void healaFullMopsPrint(int);
void saveMopsIn(void);

//------------------------------------- nen ------------------------------------

int useRealRyuIn(void);
double reflexRenIn(int);
int gastaRenIn(int, int);
int useAopIn(int, int);
void usaKenIn(int);
void showKensIn(int);
int showKenIn(void);
void showKenPrint(int, int);

//----------------------------------- outros -----------------------------------

void resetPersIn(void);
int editCritIn(void);
void skipCommandIn(void);
int sairIn(int);
int reWindIn(int);


#endif // INTERFACE_H_INCLUDED

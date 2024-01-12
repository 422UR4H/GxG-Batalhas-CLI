#ifndef interface_h
#define interface_h
#define TAM 40
#define DIM 46
#define INICIAL 0
#define FINAL 1
#define FALSE 0
#define TRUE 1
#define RETURN -1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>



//struct que armazena valores de funcoes durante um turno
typedef struct _PILHA {
    char nome[TAM];
    int valInt, num; //num=numero de identificacao do personagem
    double valDouble; //guarda um valor que sera retornado, p/ o atributo do per
} PILHA;

//armazena valores todos os turnos para a funcao rewind
typedef struct _SAVE {
    int hp, mop, atacante, defensor;
} SAVE;

//armazena por 1 turno o gasto de MOP, para reduzi-lo no final
typedef struct GASTA {
    //quanto gastou, Ems, vol(?), qtd de aura bonus
    int vol, mopAnterior;
    double gastou, gastouEms;
} GASTO;

//guarda as quantidades, e é usada na struct ATAQUE, abaixo
typedef struct QUANTIDADES {
    int qtd, qtdc, qtdg; //qtdg = é a qtd de gasto
} QTDS;

typedef struct ATAQUE {
    int atk, crit, dado, mat, ems;
    double inv, ryu;
    QTDS qtds;
} ATAQUES;

typedef struct DANOS {
    int qtdDano, qtdHp, *hp, dano, danoFinal, crit, critFinal;
} DANO;

//**************************** STRUCTS PRINCIPAIS ******************************

//guarda as skills dos chars, e é usada na struct PERSONAGENS, abaixo
typedef struct SKILL {
    char nome[TAM];
    int dano, vol;
} SKILLS;
//struct que guarda as fichas com os atributos dos personagens

typedef struct PERSONAGENS {
    char nome[TAM];
    double modClas;
    int hp, hpFull, mop, mopFull, aop, pop, cdn;
    int ten, modFor, vel, qtdAtaques, shu, corpo;
    SKILLS *ataques;
} PERSONAGEM;

//******************************** FIM DESTAS **********************************




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

#endif

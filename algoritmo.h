#ifndef algoritmo_h
#define algoritmo_h
#define TAM 40
#define DIM 46
#define INICIAL 0
#define FINAL 1
#define FALSE 0
#define TRUE 1
#define RETURN -1
//sem essa biblioteca,
//a const FILE nao é
//reconhecida no codigo
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

void useRealRyuAl(int, int);
void reflexRenAl(int, int, int*);
void zeraExt(int);
void zeraShus(int);
void gastaRenAl(void);
void useAopAl(int, PILHA*);
void useKenAl(int);
double showKensAl(int, int);

//----------------------------------- outros -----------------------------------

void resetPersAl(int);
void reWindAl(int);

#endif

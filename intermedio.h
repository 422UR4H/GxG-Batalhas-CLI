#define TAM 43
#define DIM 37
#define INICIAL 0
#define FINAL 1
#define FALSE 0
#define TRUE 1
#define RETURN -1

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define TAM 40
#define DIM 37 //DIM guarda a quantidade de personagens no banco de dados
#define INICIAL 0
#define FINAL 1
#define FALSE 0
#define TRUE 1
#define RETURN -1
/*algumas funcoes sao chamadas no inicio de um turno e no final
 normalmente isso serve para trocar valores originais e destroca-los
 para que atributos da ficha de personagens não fiquem com valores errados
 INICIAL e FINAL serve para separar qual parte do codigo dessas funcoes
 sera executada corretamente, iniciando valores e retornando originais
 tudo no tempo correto de execucao. é usado um if para avaliar isso*/

/*AZZURAH: são anotações para mim, o programador do codigo,
 e nao precisam ser lidas para o entendimento do mesmo*/

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

//lista de variaveis globais
FILE *paBD, *paHP, *paBU, *paMOP;
PILHA *pilha;
SAVE *save;
PERSONAGEM per[DIM];
char comando[TAM];
int tamPilha, tamSave = 0;
int atacante = -1, defensor = -1;


//########################### PROTOTIPO DE FUNCOES #############################

void fgetspc(int, char*);
int leAtacante(char*);
int conta(char*);
int valorChar(char*);
void printError(char*);
void linha(void);
int leDef(double*, int*);
int calculaDef(double, int);
int leDefensor(void);
double leRyuDef(void);
int leDefAdc(void);
int leSeDefendeu(void);
void printDados(int);
int showKens(int);
int showKen(int);
void showFullKens(int);
void useRealRyu(int);
void showSkills(int);
int volCorpo(void);
int getSkill(int);
int pegaMult(double*, int, int*);
void leQtd(QTDS*);
double leRyu(int);
double leInvestida(int, int);
int leDado(int*);
int inicializaMat(int, int);
int leMat(int);
int leEms(void);
int leAcerto(int);
void gastaRyu(double, int);
void calculaGasto(int, ATAQUES, GASTO*);
int aplicaGasto(int, GASTO);
void printGasto(int, int);
void leComando(char*);
int testaComando(int, char*);
void help(int);
//void padContraAtk(int);
//int contraAtk(int*, int, int*, double);
void zeraExts(int);
void operaExt(int);
void operaExts(int);
void showExt(int);
void showExts(int);
int gastaRen(int);
int showAtkVol(int);
void zeraExt(int);
void zeraShus(int);
int addShu(int);
void addShus(int);
void rodaShu(int);
void turnos(int);
int rougueAtk(int);
int nerfAtk(int);
void perfDamage(int);
void showDamage(int);
void compAtk(int);
void showHps(int);
void showFullHps(int);
void showHp(int);
void showFullHp(int);
void editHp(int);
void operaHp(int);
void healaHp(int);
void healaFullHps(int);
void saveHps(int);
void showMops(int);
void showFullMops(int);
int showMop(int);
void showFullMop(int);
void showFullMop(int);
void editMop(int);
void operaMop(int);
void healaMop(int);
void healaFullMops(int);
void saveMops(int);
int reflexRen(int);
int useMat(int);
int editAop(int);
int useAop(int);
int useRen(int);
int usaKen(int);
void resetPers(int);
int editCrit(int);
int skipCommand(int);
void clean(int);
void sair(int);
void reWind(int);
void msdos(int);
void backSave(void);
void corrigeNome(char*);
int leBD(void);
int leVol(void);
int leHP(void);
int leMOP(void);
void backuphps(void);
void pegaNomeSkill(int, int);
void limpaChar(FILE*);
int aplicaDano(int, double, ATAQUES, DANO*);
void printMiss(int, int);
void printDano(int, int, DANO, ATAQUES);
void calculaDano(int, ATAQUES*);
void zeraAtaques(ATAQUES*);
int leAtaque(ATAQUES*);
//void causaDano(int);
int leCorp(void);
int opcoes(void);
int leVolSkill(int);
int suportVolCorpo(int, int);
double divideVol(double);


//############################# INICIO DO CODIGO ###############################

int main() {
    //essas variaveis do main, que foram declaradas e inicializadas,
    //indicam que tem seus valores originais retornados no final de cada turno
    int i, defesa;
    double mult, ryuDef;
    ATAQUES ataque;
    GASTO gasto;
    DANO dano;

    //carrega-se a 1 memoria para save e carregado valores de banco de dados
    save = (SAVE*) calloc(1, sizeof (SAVE));
    if (leBD() == FALSE) {
        printError("FILE paBD ou ataques");
        exit(1);
    }
    if (leVol() == FALSE) {
        printError("FILE paSkills");
        exit(1);
    }
    if (leHP() == FALSE) {
        printError("FILE paHPs");
        exit(1);
    }
    if (leMOP() == FALSE) {
        printError("FILE paMOPs");
        exit(1);
    }
    zeraShus(2);
    //passando parametro diferente de inicial e final
    //para que linha nao seja mostrada novamente
	
    system("title GxG - Batalhas 2.0");
    system("color b");
    system("cls");
    
    for (;;) { //cada reset desse loop é tratado como um turno.
        
        //atribuindo valores default para as variaveis
        atacante = -1;
        defensor = -1;
        ataque.atk = -1;
        zeraAtaques(&ataque);
        
        do { //aqui é definido o atacante e/ou um comando a ser iniciado
            linha();
            
            atacante = leAtacante("Atacante");
            
            if (atacante < -2) { //a funcao editCrit retorna valores < -2
                ataque.crit = -1 * atacante;
            }
        } while (atacante < 0);
        
        //se for o primeiro ataque do atacante:
        //*pergunta quantidade de corpo ap ser adicionado
        //*gasta o ren utilizado ao redor do corpo
        if(per[atacante].shu == 0) {
            //2 é diferente de local e inicial,
            //e a funcao precisa de um valor assim
            //para executar o correto nessa parte
            addShu(2);
            //aqui o ren é gastado
            gastaRen(2);
            
            //mostrando o numero, nome e as habilidades do atacante
            printDados(atacante);

            per[atacante].corpo += volCorpo();
        } else {
            //mostrando o numero, nome e as habilidades do atacante
            printDados(atacante);
        }
        
        //aqui recebe a habilidade a ser usada
        //getSkill garante um valor valido
        ataque.atk = getSkill(atacante);
        //caso o usuario entre -1 (ele sabe disso)
        //o turno é quebrado e reiniciado
        if (ataque.atk < 0) {
            continue;
        }
        
        //AZZURAH: nao consegui usar mult=pegaMult retornando float.
        //carregando o multiplicador de restricao da habilidade do atacante
        //funcao peculiar que age diretamente nas habilidades dos personagens
        //usa bibliotecas de interface e tambem altera a struct
        if (pegaMult(&mult, atacante, &ataque.atk) == 1) {
            continue;
        }
        
        //lendo aura bonus
        ataque.mat = inicializaMat(atacante, ataque.atk);
        
        //lendo valores do ataque
        //se for lido valor <0 para ryu, o turno é reiniciado
        if ( (leAtaque(&ataque)) == FALSE) {
            continue;
        }
        
        //calcula o gasto ataque
        calculaGasto(atacante, ataque, &gasto);
        //calcula o dano do atacante em funcao de sua habilidade utilizada
        calculaDano(atacante, &ataque);
        
        //funcao que pega o defensor, que é quem recebe o golpe do atacante (?)
        //se retornar -1 é por que o usuario resolveu retornar ao inicio
        //se retornar FALSE é pq o defensor nao se defendeu
        //se retornar TRUE é pq o defensor se defendeu
        defesa = leDef(&ryuDef, &defesa);
        if (defesa == -1) {
            zeraAtaques(&ataque);
            continue;
        } else if (defesa == TRUE) {
            defesa += per[defensor].modFor;
        }
        defesa = calculaDef(ryuDef, defesa);
	
        //i funcionando como temp
        i = aplicaDano(defesa, mult, ataque, &dano);
        if (i == FALSE) {
            printMiss(defesa, dano.dano);
        } else if (i == TRUE) {
            printDano(defesa, defensor, dano, ataque);
        } else {
            printError("damage->hp");
            exit(1);
        }
        
        //reducao do mop do atacante se utilizou atk de ems
        if (ataque.ems == TRUE) {
            //aplicaGasto retorna o valor anterior de mop do atacante
            gasto.mopAnterior = aplicaGasto(atacante, gasto);
            //impressao do gasto de mop
            printGasto(per[atacante].mop, gasto.mopAnterior);
        }
        
        //backup automatico de hps e mops
        backSave();

        //execucao da parte final do codigo de um comando
        testaComando(FINAL, comando);
        
        //é feito um backup do hp, em um banco de dados externo
        backuphps(); //backupmops();
        
        //zerando comando
        for (i = 0; i < strlen(comando); i++) {
            comando[i] = '\0';
        }
    }
    return 0;
}

//########################### FUNCOES INTERMEDIARIAS ###########################

//######################### Funcoes -menos- utilizadas #########################

int volCorpo() {
    int vol = 0, temp, op, i, corp;
    
    corp = leCorp();
    
    if (corp == FALSE) {
        return FALSE;
    } else if (corp == -1) {
        return -1;
    } else {
        
        //printa as opcoes e le a escolhida
        op = opcoes();
        //menu de opcoes
        switch (op) {
        
            case 1:
                
                //leVolSkill retorna a habilidade do volume
                temp = leVolSkill(op);
                //vol recebe os calculos do volume
                vol += suportVolCorpo(atacante, temp);
                
                break;
            
            case 2:
                
                //leVolSkill printa Quais e retorna 3
                op = leVolSkill(op);
                
                for (i = 0; i < 2; i++) {
                    //leVolSkill retorna a habilidade do volume
                    temp = leVolSkill(op);
                    //vol recebe os calculos do volume
                    vol += suportVolCorpo(atacante, temp);
                    }
                break;
                
            case 3:
                
                op = 4;
                //leVolSkill retorna qtd de habilidades
                i = leVolSkill(op);
                
                //leVolSkill printa skills e retorna 3
                op = 5;
                op = leVolSkill(op);
                
                for ( ; i > 0; i--) {
                    //leVolSkill retorna a habilidade do volume
                    temp = leVolSkill(op);
                    //vol recebe os calculos do volume
                    vol += suportVolCorpo(atacante, temp);
                    }
                break;
                
            case 4:
                
                //leVolSkill retorna o volume lido
                op = 6;
                vol = leVolSkill(op);
                break;
                
            case -1:
                
                return -1;
                
            default:
                
                //leVolSkill printa Invalido e retorna FALSE (0)
                op = 7;
                return leVolSkill(op);
        
        }
        return vol;
    }
}

int suportVolCorpo(int atacante, int temp) {
    double aux;
    
    if (temp < 0) {
                    
        if (per[atacante].ataques[temp].vol == -1) {
            aux = (double) leVolSkill(6);
            aux = divideVol(aux);
        } else {
            aux = divideVol((double) per[atacante].ataques[temp * -1].vol);
        }
        return (int) ceil(aux);

    } else {
        
        if (per[atacante].ataques[temp].vol == -1) {
            return leVolSkill(6);
        } else {
            return per[atacante].ataques[temp].vol;
        }
    }
}

int pegaMult(double *mult, int atacante, int *ataque) {
    int i, j, atk = atacante;
    char rest[3];

    if (*ataque == 0) {
        per[atk].ataques[*ataque].dano = per[atk].modFor;
        *mult = 1;
    } else if (strcmp(per[atk].ataques[*ataque].nome, "PRIMEIRO PASSO") == 0 ||
               strcmp(per[atk].ataques[*ataque].nome, "<22>SEGUNDO PASSO") == 0 ||
               strcmp(per[atk].ataques[*ataque].nome, "FECHAR PASSOS") == 0) {
        
        puts("Nota: os atks NELE tem +8 (atk) no DANO");
        //alterando atributos do personagem
        //deve haver - no lugar de +, pq o -= inverte o sinal do lado diteito
        per[atk].ataques[1].dano -= per[atk].modFor -
                per[atk].ataques[*ataque].dano;
        
        per[atk].modFor = per[atk].ataques[*ataque].dano;
        
        
        if (per[atk].ataques[*ataque].nome[0] == '<') {
            for (i = 1, j = 0; per[atk].ataques[*ataque].nome[i] != '>'; i++, j++) {
                rest[j] = per[atk].ataques[*ataque].nome[i];
            }
            i = atoi(rest); //uso i so p n ter que criar outra variavel
            per[atk].aop += i; //acho que poderia usar per[atk].aop+=atoi(rest);
            per[atk].ten += i;
        }
        return 1;
        
    } else if (strcmp(per[atacante].ataques[*ataque].nome, "BRINCALHAO") == 0) {
        printf("Multiplicador: ");
	do {
            scanf("%lf", mult);
	} while (*mult > 9);
        printf("Que ataque? ");
        *ataque = getSkill(atacante);
        //return mult;
    } else if (strcmp(per[atacante].ataques[*ataque].nome, "<33>MAN") == 0) {
        //funcao exclusiva pra habilidade de Biggle'tt
        puts("AVISO: chame ROUGUEATK para essa habilidade");
        return 1;
    } else if (per[atacante].ataques[*ataque].nome[3] == '*') {
        for (i = 0; i < 3; i++)
            rest[i] = per[atacante].ataques[*ataque].nome[i];
        *mult = atof(rest); //JUNTAR RETURN AQUI
        //return mult;
    } else {
        *mult = 1;
    }
    return 0;
}

int leAtaque(ATAQUES *ataque) {
    int atk, i;
    
    //lendo ataque
    atk = ataque->atk;
    
    //lendo ryu
    if ( (ataque->ryu = leRyu(atacante)) < 0) {
        return FALSE;
    }
    
    //lendo investida e dado
    ataque->inv = leInvestida(atacante, ataque->inv);
    ataque->dado = leDado(&ataque->crit);
    
    //se houver '*' no nome da habilide, ela pode ser usada + d 1 vez no turno
    if ((per[atacante].ataques[atk].nome[0] == '*') ||
       (per[atacante].ataques[atk].nome[1] == '*') ||
       (strcmp(per[atacante].ataques[atk].nome, "FISICO") == 0)) {

        leQtd(&ataque->qtds); //func que pega a qtd que a skill é utilizada

    }
    
    //lendo aura bonus
    if (ataque->mat > 0) {
        if ( (ataque->mat = leMat(ataque->mat)) < 0) {//OU IGUAL A -1, NAO SEI!
            leAtaque(ataque);
            return TRUE;
        }
    }
    
    //se houver <>, no nome, é pq a habilidade pode ser se emissão
    //o for abaixo vasculha o nome, para perguntar se o atk é de ems,
    //caso if retorne um valor positivo (encontre <);
    for (i = 0; i <= strlen(per[atacante].ataques[atk].nome); i++) {
        if (per[atacante].ataques[atk].nome[i] == '<') {
            //lendo valor de emissão e testando para ver se é <0
            if ( (ataque->ems = leEms()) < 0) {
                leAtaque(ataque);
                return TRUE;
            }
            break; //<-impede o for de continuar rodando atoa
	}
    }
    
    //atribuicao do gasto total para o calculo do gasto
    ataque->qtds.qtdg = ataque->qtds.qtd;
    
    //lendo qtd de acerto
    if ( (ataque->qtds.qtd = leAcerto(ataque->qtds.qtd)) < 0) {
        leAtaque(ataque);
    }
    
    return TRUE;
}

int leDef(double *ryuDef, int *defesa) {
    int valor;
    
    //Valor de retorno: se leDefensor retornar -1,
    //significa que o usuario preferiu zerar as leituras
    //para qualquer outro valor -1 entrado, a funcao
    //usa recursao apenas para reler os valores de def
    if ( (defensor = leDefensor()) < 0) {
        return -1;
    }
    if ( (*ryuDef = leRyuDef()) < 0) {
        return leDef(ryuDef, defesa);
    }
    if ( (*defesa = leDefAdc()) < 0) {
        return leDef(ryuDef, defesa);
    }
    if ( (valor = leSeDefendeu()) < 0) {
        return leDef(ryuDef, defesa);
    }
    return valor;
}


//######################### Funcoes +mais+ Utilizaveis #########################

//entra com um numero ou nome de personagem e retorna o numero desse personagem
int leAtacante(char *str) {
    char atacker[TAM]; //salva ou o nome ou o numero do personagem
    int atacante; //recebe o numero do personagem
    
    do {
        printf("%s: ", str);
        fgetspc(TAM, atacker);
        strupr(atacker);
    
        //se a entrada for um numero valido, a resposta será menor que 115
        //e esse valor é passado para atacante
        if (conta(atacker) < 115) {
        
            atacante = atoi(atacker);
            if (atacker[0] == '\0' || atacante < 0) {
                continue;
            }
        
        } else if (strcmp(atacker, "HELP") == 0) {//colocar help no command tbém
        
            help(INICIAL);
            continue;
        
        } else if (strcmp(atacker, "COMANDO") == 0 ||
                   strcmp(atacker, "COMMAND") == 0) {
            
            do {
                //por default, testaComando retorna RETURN (-1)
                leComando(comando);
                atacante = testaComando(INICIAL, comando);
                
                if (atacante == FALSE) {
                    puts("Comando Invalido!");
                } else {
                    atacante = RETURN;
                }
            } while (atacante == FALSE);
            
        } else {
            
            return valorChar(atacker);
            
        }
    //ATENCAO: a funcao testaComando pode retornar valores < -2 !!!
    } while (atacante == -1);
    
    return atacante;
}

//############################ FUNCOES DE INTERFACE ############################

//######################### Funcoes -menos- utilizadas #########################

void printDados(int atacante) {
    printf("%d ", atacante);
    puts(per[atacante].nome);
    showSkills(atacante);
    return;
}

void showSkills(int atacante) {
    int i;
    //for(i=0;i<sizeof(per[i].ataques)/sizeof(ATAQUES))
    //NA TEORIA ERA PRA ISSO DAR CERTO. E É MELHOR USAR /sizeof(ataques[0])
    for (i = 0; i < per[atacante].qtdAtaques; i++) {
        printf("[%d]", i);
        puts(per[atacante].ataques[i].nome);
    }
    return;
}

int leVolSkill(int op) {
    switch (op) {
        case 1:
            printf("Qual? ");
            scanf("%d", &op);
            return op;
            
        case 2:
            printf("Quais? ");
            return 3;
            
        case 3:
            scanf(" %d", &op);
            return op;
            
        case 4:
            printf("Quantas? ");
            scanf("%d", &op);
            return op;
            
        case 5:
            printf("Skills: ");
            return 3;
            
        case 6:
            printf("Volume: ");
            scanf(" %d", &op);
            return op;
            
        case 7:
            puts("Resposta Invalida");
            return FALSE;
    }
}

int leCorp() {
    char linha[3];
    
    printf("[0]N [1]S\nAdd Corp? ");
    fflush(stdin);
    fgets(linha, 3, stdin);
    
    if (linha[0] == '0') {
        return FALSE;
    } else if (linha[0] == '-') {
        return -1;
    } else {
        return TRUE;
    }
}

int opcoes() {
    int op;
    
    puts("[1]So uma");
    puts("[2]So duas");
    puts("[3]Varias");
    puts("[4]Volume");
    puts("[-1]Sair");
    puts("Qts Skills? ");
    scanf("%d", &op);
    puts("OBS: Volumes negativos para objetos que estavam guardados!");
    return op;
}

int getSkill(int atacante) {
    int ataque;
    
    printf("[-1]Volta\nQue ataque? ");
    scanf(" %d", &ataque);
    if (ataque >= per[atacante].qtdAtaques) {
        printf("Entrada invalida\nQue ataque? ");
        //nesse caso é chamado uma recursao retornando o valor correto
        return getSkill(atacante);
    }
    return ataque; //ataques do yasuhiko retornam -1 para usar o goto na chamada
}

//---------------------------- Funcoes de leAtaque -----------------------------

double leRyu(int atacante) {
    char linha[5];
	
    /*o if abaixo serve para caso o corpo nao seja o padrao default,
     seja mostrado ao usuario final, qual o verdadeiro valor de 50%*/
    if (per[atacante].corpo != 100) {
        printf("50%%=%.3lf\n", 50 / (1.0 * per[atacante].corpo / 100));
    }
    printf("Ryu ataque: ");
    fflush(stdin);
    fgets(linha, 5, stdin);
    if (linha[0] == '\n') {
        return 50;
    } else {
        return atof(linha);
    }
}

double leInvestida(int atacante, int inv) {
    char linha[5];
    
    //se nao foi usada uma funcao para definir um valor para inv:
    if (inv == 0) {
        printf("[%d]Invest: ", per[atacante].vel);
        fflush(stdin);
        fgets(linha, 5, stdin);
        if (linha[0] == '\n') {
            inv = 0;
        } else {
            inv = atoi(linha);
        }
    }
    return inv;
}

int leDado(int *crit) {
    char linha[5];
    int dado;
	
    //RECEBE DADO E/OU BONUS
    printf("Dado\\bonus: ");
    fflush(stdin);
    fgets(linha, 5, stdin);
    if (linha[0] == '\n') {
        dado = 0;
    } else if (linha[0] == '-') {
        *crit = 2;
        dado = 0;
    } else {
        dado = atoi(linha);
    }
    return dado;
}

void leQtd(QTDS *qtds) {
    char linha[3];

    do {
	printf("QTD de ATK: ");
	scanf("%d", &qtds->qtd);
    } while (qtds->qtd < 1);
    printf("QTD critic: ");
    fflush(stdin);
    fgets(linha, 3, stdin);
    if (linha[0] == '\n' || linha[0] == '0') {
        qtds->qtdc = 0;
    } else {
        qtds->qtdc = atoi(linha);
        if (qtds->qtdc < 0) {
            leQtd(qtds);
            return;
        }
    }
    if (qtds->qtdc > qtds->qtd) {
        puts("Ta loconaO?");
        leQtd(qtds);
    }
    return;
}

int leMat(int mat) {
    char linha[5];
    
    printf("<%03d> AOP+: ", mat);
    fflush(stdin);
    fgets(linha, 5, stdin);
    if (linha[0] != '\n' && linha[0] != '0') {
        mat = atoi(linha); //joga o novo valor de linha para mat
    } else {
        return 0;
    }
    //caso o usuario final entre -1 é chamada uma recursao em leAtacante
    if (mat < 1) {
        return -1;
    }
    return mat;
}

int leEms() {
    int ems;
	
    //testa se usa ou nao 100% do nen bonus
    //mat/man no atk (isso se for EMS)
    printf("[0]N [1]S\nAtk EMS(x2)? ");
    scanf("%d", &ems);
    return ems;
}

int leAcerto(int qtd) {
	
    if (qtd > 1) {
        printf("QTD acerto: ");
        scanf("%d", &qtd);
    }
    return qtd;
}

//---------------------------- Funcoes de leDef --------------------------------

int leDefensor() {
    char linha[TAM];
	
    //caso defensor ja tenha um valor, nao sera pedido novamente
    if (defensor == -1) {
	do {
            printf("Defensor: ");
            fgetspc(TAM, linha);
        } while (linha[0] == '\0');
		
        strupr(linha);
		
        if (conta(linha) < 115) {
            //se for numero e nao um nome:
            defensor = atoi(linha);
            if (defensor == -1) {
		//-1, volta à main, onde o turno é quebrado e reiniciado
                return -1;
            }
        } else if ((defensor = valorChar(linha)) == -1) {
            return -1;
        }
    }
    return defensor;
}

double leRyuDef() {
    double ryuDef;
	char linha[4];
	
    /*o if abaixo serve para caso o corpo nao esteja no padrao default
     seja mostrado ao usuario final o atual valor de 50%*/
    if (per[defensor].corpo != 100) {
        printf("50%%=%.3lf\n", 50 / (1.0 * per[defensor].corpo / 100));
    }
    printf("Ryu defesa: ");
    fflush(stdin);
    fgets(linha, 4, stdin);
    
    if (linha[0] == '\n') {
        return 50.0;
    } else if ( (ryuDef = atoi(linha)) < 0) {
        return -1;
    } // else desnecessario, pois ruyDef ja tem o valor de atoi(linha)
    return ryuDef;
}

int leDefAdc() {
    int defesa;
    char linha[4];
	
    printf("Defesa adc: ");
    fflush(stdin);
    fgets(linha, 4, stdin);
    if (linha[0] == '\n') {
        defesa = 0;
    } else {
        defesa = atoi(linha);
    }
    return defesa;
}

int leSeDefendeu() {
    char linha[4];
    
    printf("[0]N [1]S\nDefendeu? ");
    fflush(stdin);
    fgets(linha, 4, stdin);
    if (linha[0] == '\n' || linha[0] == '0') {
        return FALSE;
    } else {
        return TRUE;
    }
}

//------------------------------------------------------------------------------


void printMiss(int defesa, int dano) {
    printf("Defesa=%d, Dano=%d\n", defesa, dano);
    printf("MISS\n");
    return;
}

void printDano(int defesa, int defensor, DANO damage, ATAQUES ataque) {
    int i;
    
    puts(per[defensor].nome);
    printf("HP Full=%d, HP Atual=%d, Defesa=%d, Dano=%d",
            per[defensor].hpFull, damage.hp[0], defesa, damage.dano);
    
    if (damage.critFinal > 0) {
        printf(", Critico=%d", damage.crit);
    }
    printf("\n");
    
    
    if (ataque.crit > 1 && ataque.qtds.qtdc == 0) {
			
        printf("Crit Final=%d, Hp Final=%d\n",
                damage.critFinal, damage.hp[1]);
        
        return;
    
    } else {
    
        for (i = 0; i < damage.qtdDano; i++) {

            printf("Dano Final=%d, Hp Final=%d\n",
                    damage.danoFinal, damage.hp[i + 1]);

        }
	i++;
        for ( ; i < damage.qtdHp; i++) {

            printf("Crit Final=%d, Hp Final=%d\n",
                    damage.critFinal, damage.hp[i]);

        }
    }
    return;
}

void printGasto(int mopFinal, int mopAnterior) {

    puts(per[defensor].nome);
    printf("MOP Atual=%d ", mopAnterior);
    printf("MOP Final=%d\n", mopFinal);
    return;
}

//########################### Funcoes de Comandos ##############################

//------------------------------- PRINCIPAL ------------------------------------

void leComando(char *comando) {
    printf("Entre com o comando: ");
    fgetspc(TAM, comando);
    strupr(comando);
    return;
}

//------------------------------- COMANDO HELP ---------------------------------

void help(int local)//ALTERAR ORDEM DA LISTA PRECEDENDO OS MAIS UTEIS
{/*COLOCAR UM COMANDO MAIS NO HELP PARA MOSTRAR OUTROS COMANDOS COMO:
  BURST DE DANO, NERF, ALGUNS "INUTEIS" DO HP... e outros*/
    if (local == INICIAL) {
        printf( "[command]------para introduzir um comando abaixo\n"
                "[rewind]-------(ctrl+z) retorna hp e mop\n"
                "[addshu]-------aumenta ou diminui um shu de um personagem\n"
                "[addshus]------aumenta ou diminui o shu de personagens\n"
                "[rodashu]------roda um turno, calculando os gastos de mop\n"
                "[turnos]-------roda mais de um turno, gastando mop\n"
                "[gastaren]-----gasta o ren (quando é ativado)\n"
                "[operaext]-----add extensoes ao corpo do char, red. seu ren\n"
                "[showext]------mostra qtd de extensoes de corpo de um char\n"
                "[showexts]-----mostra qtd de exts de corpo de varios chars\n"
                "[showvol]------mostra o volume do ataque de um personagem\n"
                "[zeraext]------reseta a extensao de um personagem\n"
                "[zeraexts]-----reseta a extensao de todos os personagens\n"
                "[zerashus]-----zera todos os shus dos personagens\n"
                //usa a funcao calculaDano para calcular na defesa
                //"[causadano]----aplica um dano ja calculado direto na defesa\n"
                "[contratk]-----permite o contra-ataque do defensor\n"
                "[saveatk]------salva variaveis de um ataque\n"
                "[reflexren]----limita o aop de atk nesse turno\n"
                "[usemat]-------usa o bonus de mat\\man num atk que nao o tem\n"
                "[compatk]------compara 2 ataques\n"//ou comparaatk ou comparatk
                "[rougueatk]----permite um jogador usar o ataque de outro\n"
                //nao pode diminuir dano com -, pq o programa entende como crit
                "[nerfatk]------diminui o dano de um atk nesse turno\n"
                //INICIAL; FINAL pede dano full e pede novamente as funcs de def
                /**"[areadamage]---dano em area\n"*/
                "[perfdamage]---dano perfurante (ou em en)\n"
                "[showdamage]---mostra o dano de um ataque\n"
                "[editaop]------altera um aop utilizavel\n"
                "[useaop]-------usa o aop inserido\n"
                "[useRealRyu]---usa o valor de ryu inalterado por corpo\n"
                "[showFullKens]-mostra o KEN de todos os personagens\n"
                //"[cleanaop]-----limpa o aop inserido\n"//SO SERVE P MULTIFUNCS
                "[resetpers]----reseta as alteracoes feitas nos pers\n"
                //"[resetper]-----reseta a alteracao feita em um personagem\n"
                "[showhps]------mostra os hps atuais\n"
                "[showfhps]-----mostra os hps totais\n"
                "[showhp]-------mostra um hp especifico\n"
                "[showfhp]------mostra um hp total especifico\n"
                "[edithp]-------edita um hp\n"
                "[operahp]------soma ou dimiui um hp\n"
                "[healahp]------heala full um hp\n"
                "[healafullhps]-heala todos os hps\n"
                "[savehps]------salva os hps no BD\n"
                "[savehp]-------salva um hp no BD\n"
                "[showmops]-----mostra os mops atuais\n"
                "[showfmops]----mostra os mops totais\n"
                "[showmop]------mostra um mop especifico\n"
                "[showfmop]-----mostra um mop total especifico\n"
                "[editmop]------edita um mop\n"
                "[operamop]-----soma ou siminui um mop\n"
                "[healamop]-----heala full um mop\n"
                "[healafullmops]heala full mops\n"
                "[savemops]-----salva os mops no BD\n"
                /**"[savemop]------salva um mop no BD\n"*/
                "[clean]--------limpa a tela\n"
                "[msdos]--------conversa diretamente com o MS-DoS\n"
                "[skipcommand]--cancela o pedido de um comando\n"//ou skip
                "[exit]---------fecha o programa\n");
        return;
    }
    //se command for acionado, e nao for reconhecido,
    //gera um aviso e pede novamente o comando
}


//######################### Funcoes +mais+ Utilizaveis #########################

void corrigeNome(char *str) {
    int i, j;

    //le toda a string no endereco do ponteiro ate o null,
    //verificando se ha caracteres diferentes de A a Z
    //se houver, passa o valor de um endereco adiante
    //(proximo caractere) ao avaliado, e continua ate o NULL
    //continua avaliando, se houver outros,
    //o mesmo procedimento acima ocorre, se nao, encerra no NULL
    for (i = 0; *(str + i) != '\0'; i++) {
        if (*(str + i) < 65 || *(str + i) > 90) {
            for (j = i; *(str + j) != '\0'; j++) {
                *(str + j) = *(str + j + 1);
            }
        }
    }
    return;
}

void printError(char *pont) {
    printf("Nao foi alocado memoria ao ponteiro %s\n", pont);
    system("pause>>null");
    return;
}

void linha() {
    printf("------------------------------------------------------------\n");
    return;
}

int conta(char *str) {
    int i = 0, valor = 0;

    while (*(str + i) != '\0') {
        valor += *(str + i);
        i++;
    }
    return valor;
}

int valorChar(char *str) {
    int i;

    corrigeNome(str);
    //comp. o nome no endereco apontado por str, com todos os nomes de persons
    for (i = 0; strcmp(str, per[i].nome) != 0 && i < DIM; i++);
    if (i >= DIM) {
        puts("String invalida, 422UR4H, seu BURRO!!!");
        return -1;
    }
    return i;
}

//------------------------------ minhas funcoes ------------------------------

//funcao como a fgets, mas essa tira o <enter> automaticamente
void fgetspc(int tam, char *str) {
    /*criado por 422UR4*/
    fflush(stdin);
    fgets(str, tam, stdin);
    str[strlen(str) - 1] = '\0';
    return;
}


//############################ FUNCOES DE ALGORITMO ############################

//######################### Funcoes -menos- utilizadas #########################

double divideVol(double aux) {
    return aux / 2;
}

int inicializaMat(int atacante, int atk) {
    char linha[5];
    int mat, i, j;
    
    mat = 0;
    //INICIALIZA MAT
    //procura-se um '<', caso ache, o bloco de comandos é executado e breakeado,
    //para impedir o loop desnecessario
    for (i = 0; i <= strlen(per[atacante].ataques[atk].nome); i++) {
        if (per[atacante].ataques[atk].nome[i] == '<') {
            i++; //incr. i p copiar os chars numericos e converte-los para int
            //le-se todos valores entre < e > e os salva em linha[j]
            for (j = 0; per[atacante].ataques[atk].nome[i] != '>'; j++, i++) {
                linha[j] = per[atacante].ataques[atk].nome[i];
            }
            mat = atoi(linha);
            if (per[atacante].aop < mat) {
                mat = per[atacante].aop;
            }
            break; //<-ganhamos tempo de execucao impedindo o loop desnecessario
	}
    }
    return mat;
}

void calculaGasto(int atacante, ATAQUES ataque, GASTO *gasto) {
    
    if (ataque.ems == TRUE) {
    
        //gasta volume * qtd
        gasto->gastouEms = 
                per[atacante].ataques[ataque.atk].vol * ataque.qtds.qtdg;

        //soma mat em gasto
        //nao preciso multiplicar por qtd, pq mat é fixo
        gasto->gastouEms += ataque.mat;
        gasto->gastouEms *= 50;
    
    }
    
    if (ataque.ryu > 50) {
        
        //gasta ryu (a partir de 51%)
        gastaRyu(ataque.ryu, atacante);
        
        if (ataque.ems == TRUE) {
            ataque.ryu -= 50;

            //o correto seria (ataque.ryu / 50) * per[atacante].aop * 50;
            //porque a ideia é que cada ryu alem dos 50, utilizado no atk,
            //essa porcentagem é toda retirada do corpo, mas nesse caso,
            //o corpo conta com 50% da aura. logo, nao seria ryu / 100,
            //mas sim ryu / 50; logo a frente, como o aop é convertido por 50,
            //o resultado seria multiplicado por 50; logo nao precisa de 50

            //ex:
            //se for um ataque de KOU, o aop é multiplicado por 50 (100 - 50)
            //e o gasto correto do kou é exatamente 1 * aop * 50
            //1 por causa do ryu = 100; 100 / 100 --> 1
            gasto->gastouEms += ataque.ryu * per[atacante].aop;

            //se o gasto ultrapassar a qtd de aura no corpo,
            //provavelmente será por conta do volume, então,
            //o volume é ignorado e a qtd maxima é gasta...
            if (gasto->gastouEms > (per[atacante].aop + ataque.mat) * 50) {
                gasto->gastouEms = (per[atacante].aop + ataque.mat) * 50;
            }
        }
    }
    //antigo altoritmo de gasto. talvez ainda seja util:
//gasto->gastouEms = (ataque.ryu / 100) * per[atacante].aop * ataque.qtds.qtdg;
    
    return;
}

/*a partir de 51 de ryu, 1 de nen é gasto a mais por segundo,
 a partir de 61, dois a mais é gasto por segundo, etc.
 com um de mod cdn, menos um é gasto de nen por segunso,
 logo, o gasto adicional é nulo entre 50 e 61, etc.*/
/*o algoritmo da funcao abaixo, procura nos for ecadeados,
 qual foi a quantidade de ryu gasta. e em seguida,
 reduz o gasto de acordo com o mod cdn do atacante,
 e aplica o resultado no mop, reduzindo-o apenas*/
//FUNCAO QUE GASTA NEN DE ACORDO COM O RYU DO ATAQUE
void gastaRyu(double ryu, int atacante) {
    int min, max, i, gasto;
    //da pra juntar min e max, mas fica menos legivel
    for (i = 1, min = 50, max = 60; i <= 5; i++, min += 10, max += 10) {
        if (ryu > min && ryu <= max) {
            //quanto mais cdn, menor o gasto
            if ((gasto = i - per[atacante].cdn) > 0) {
                per[atacante].mop -= gasto;
                break;
            }
        }
    }
}

void calculaDano(int atacante, ATAQUES *ataque) {
    
    double ren;

    //ren é usado temporariamente, para manter o tipo da variavel mat em int
    ren = (ataque->mat*((1.0 * per[atacante].pop / 10) + 1)) / ataque->qtds.qtd;
    ataque->mat = floor(ren);
    
    //dobra o dano de nen, se for um ataque de emissão
    //LEMBRANDO que ataque->mat esta sendo usado para guardar ren
    if (ataque->ems == TRUE) {
        ataque->mat /= 2;
    }

    //INICIALIZA REN
    ren = per[atacante].aop * ((1.0 * per[atacante].pop / 10) + 1);
    ren = floor(ren);

    //CALCULA RYU reduzindo em funcao de corpo
    ataque->ryu = ataque->ryu / (1.0 * per[atacante].corpo / 100);
    //CALCULA RYU usando o multiplicador de clase e de(o) ryu
    //lembrando que mat contem o valor int de ren
    ataque->ryu = ataque->ryu / 100 * ren * per[atacante].modClas +
            per[atacante].modClas * ataque->mat;
    
    ataque->ryu = floor(ataque->ryu);
    
    //CONVERTE INVESTIDA
    ataque->inv /= 3.6;
    ataque->inv = floor(ataque->inv);
    
    //TESTA CRITICO (tambem recebe, caso positivo)
    if (ataque->dado < 0) {
        //so altera critico para x2, se nao foi usado uma funcao,
        //que aumenta o valor do dano critico
        if (ataque->crit == 1) {
            ataque->dado *= -1;
            ataque->crit = 2;
        } else {
            //se foi usada essa funcao, troca o sinal de ataque->dado
            ataque->dado *= -1;
        }
    }
    ataque->dado += per[atacante].ataques[ataque->atk].dano;
    return;
}

int calculaDef(double ryuDef, int defesa) {
    double ren;
    
    if (ryuDef > 50) {
        gastaRyu(ryuDef, defensor);
    }
	
    ryuDef = ryuDef / (1.0 * per[defensor].corpo / 100);
    
    ren = per[defensor].aop * ((1.0 * per[defensor].pop / 10) + 1);
    ren = floor(ren);
    
    ryuDef = ryuDef / 100 * ren * per[defensor].modClas;
    ryuDef = floor(ryuDef);
    
    defesa += ryuDef;
	
    return defesa;
}

int aplicaDano(int defesa, double mult, ATAQUES ataque, DANO *damage) {
    double dano, crit;
    int i, qtd;
    
    //se houver(em) dano(s) critico(s), havera um multiplicados (x2)
    //mas so se critico nao foi definido anteriormente
    if (ataque.qtds.qtdc > 0 && ataque.crit == 1) {
        ataque.crit = 2;
    }
    
    //calculo final do dano
    //se o dano for maior que a defesa
    dano = ataque.inv + mult * (ataque.ryu + ataque.dado);
    dano = floor(dano);
    if (dano > defesa) {
        
        crit = ataque.inv + mult * (ataque.ryu + ataque.crit * ataque.dado);
        crit = floor(crit);
        
    } else {
        //se o dano for <= 0, a main chama printMiss();
        return FALSE;
    }
    
    //atribuindo dano para impressao
    damage->dano = dano;
    damage->crit = crit;
    
    //reduzindo danos
    dano -= defesa;
    crit -= defesa;
    
    //se o dano critico for maior que
    //o dobro do dano padrao, o dano
    //critico recebe o dobro desse dano
    if (crit > dano * 2) {
        crit = dano * 2;
    }
    
    //se ha dano critico, mas nao ha qtdc, significa que,
    //o ataque nao é multiplo, entao imprime apenas uma vez;
    if (ataque.crit > 1 && ataque.qtds.qtdc == 0) {
        
        //atribuindo valor para qtd de impressoes de hp
        damage->qtdHp = 2;
        
        //alocando memoria paraa *hp
        if ((damage->hp = (int*)calloc(damage->qtdHp, sizeof (DANO))) == NULL) {
            return RETURN;
        }
        
        //atribuindo valores para primeira impressao
        damage->critFinal = crit; //dano tem o valor de dano critico
        damage->hp[0] = per[defensor].hp;
        
        //aplicando dano critico
        per[defensor].hp -= crit;
        
        //atribuindo valor reduzido para impressao
        damage->hp[1] = per[defensor].hp;
        
    } else {
        
        //qtd recebe a quantidade de acertos normais (sem critico)
        qtd = ataque.qtds.qtd - ataque.qtds.qtdc;
        
        //atribuindo valor para quantidade de hp para impressoes
        damage->qtdHp = ataque.qtds.qtd + 1; //qtd total +1
        damage->qtdDano = qtd;
        //o +1 serve para alocar o hp antes da reducao
        
        //alocando memoria para hp
        if ((damage->hp = (int*)calloc(damage->qtdHp, sizeof (DANO))) == NULL) {
            return RETURN;
        }
        
        //atribuicoes de valores para impressoes
        if (ataque.qtds.qtdc > 0) {
            damage->critFinal = crit;
        } else {
            damage->critFinal = 0;
        }
        damage->danoFinal = dano;
        damage->hp[0] = per[defensor].hp;
        
        for (i = 0; i < qtd; i++) {
            //reducao de hp (aplicacao de dano)
            per[defensor].hp -= dano;
            //atribuicao de hp para impressao
            damage->hp[i + 1] = per[defensor].hp;
        }
        
        ataque.qtds.qtdc += i;
        
        for ( ; i < ataque.qtds.qtdc; i++) {
            //reducao de hp (aplicacao de dano);
            per[defensor].hp -= crit;
            //atribuicao de hp para impressao
            damage->hp[i + 1] = per[defensor].hp;
        }
    }
    return TRUE;
}

int aplicaGasto(int perso, GASTO gasto) {
    
    //atribuicao do mop atual para impressao
    gasto.mopAnterior = per[perso].mop;
    
    
    //aplicacao do gasto (reducao de mop)
    per[perso].mop -= gasto.gastouEms;
    
    return gasto.mopAnterior;
}


//########################### Funcoes de Comandos ##############################

//------------------------------- PRINCIPAIS -----------------------------------


//######################### Funcoes +mais+ Utilizaveis #########################

void backSave() {
    save = (SAVE*) realloc(save, (tamSave + 1) * sizeof (SAVE));
    save[tamSave].atacante = atacante;
    save[tamSave].defensor = defensor;
    save[tamSave].mop = per[atacante].mop;
    save[tamSave].hp = per[defensor].hp;
    tamSave++;
    return;
}

void zeraAtaques(ATAQUES *ataque) {
    ataque->qtds.qtd = 1;
    ataque->qtds.qtdc = 0;
    ataque->crit = 1;
    ataque->inv = FALSE;
    ataque->ems = FALSE;
    return;
}


//#################################### bds #####################################

int leBD() {
    int i, j, qtd;
    char c;

    if ((paBD = fopen("BDfichas.txt", "r")) == NULL) {
        return FALSE;
    }
    
    for (i = 0; i < DIM; i++) {
        limpaChar(paBD);
        fgets(per[i].nome, TAM, paBD);
        per[i].nome[strlen(per[i].nome) - 1] = '\0';
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].hpFull);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].mopFull);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].aop);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].pop);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].cdn);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].ten);
        limpaChar(paBD);
        fscanf(paBD, "%lf", &per[i].modClas);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].modFor);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].vel);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].corpo);
        limpaChar(paBD);
        fscanf(paBD, "%d", &per[i].qtdAtaques);
        //soma um a qtdAtaques pq o BD nao conta o fisico que tambem e atribuido
        per[i].qtdAtaques++;
        qtd = per[i].qtdAtaques;
        //le o caractere <enter> ( \n )
        c = fgetc(paBD);
        //aloca memoria
        if ((per[i].ataques = (SKILLS*) calloc(qtd, sizeof (SKILLS))) == NULL) {
            return FALSE;
        }
        //o elemento 0 do vetor ataques é guardado para o atk fisico
        for (j = 1; j < qtd; j++) {
            pegaNomeSkill(i, j);
            c = fgetc(paBD);
            fscanf(paBD, "%d", &per[i].ataques[j].dano);
            c = fgetc(paBD);
        }
        //multiplica o MOP por 1000
        per[i].mopFull *= 1000;
    }
    fclose(paBD);
    
    return TRUE;
}

int leVol() {
    int i, j;
    char c;
    FILE *paSkills;
	
    //lendo stream FILE
    if ( (paSkills = fopen("BDskills.txt", "r")) == NULL) {
        return FALSE;
    }
	
    for (i = 0; i < DIM; i++) {
        //limpa o nome do personagem
	while (c != ':') {
            c = fgetc(paSkills);
        }
        c = fgetc(paSkills);
		
        for (j = 1; j < per[i].qtdAtaques; j++) {
            //limpa o nome das skills
            while (c != ':') {
                c = fgetc(paSkills);
            }
            //limpa o caractere <espaco> (' ')
            c = fgetc(paSkills);
            fscanf(paSkills, "%d", &per[i].ataques[j].vol);
	}
    }
    return TRUE;
}

int leHP() {
    int i;

    if ((paHP = fopen("BDHPs.txt", "r")) == NULL) {
        return FALSE;
    }
    
    for (i = 0; i < DIM; i++) {
        limpaChar(paHP);
	fscanf(paHP, "%d", &per[i].hp);
    }
    fclose(paHP);
    return TRUE;
}

int leMOP() {
    int i;
	
    if ((paMOP = fopen("BDMOPs.txt", "r")) == NULL) {
        return FALSE;
    }
    
    for (i = 0; i < DIM; i++) {
	limpaChar(paMOP);
        fscanf(paMOP, "%d", &per[i].mop);
    }
    fclose(paMOP);
    
    return TRUE;
}

//func. que salva hp e mop no final de cada turno em um banco de dados de backup
void backuphps() {
    int i;

    //alocando memoria
    paBU = fopen("BDbackup.txt", "w");
    
    for (i = 0; i < DIM; i++) {//passar hps pra int
        fputs(per[i].nome, paBU);
        fputs(": ", paBU);
        fprintf(paBU, "%d", per[i].hp);
        fputs(" : ", paBU);
        fprintf(paBU, "%d", per[i].mop);
        fputc('\n', paBU);
    }
    fclose(paBU);
    
    return;
}

//---------------------- BDs (funcoes +mais+ utilizaveis) ----------------------

//pega o nome das habilidades do banco de dados e salva-los na struct per
void pegaNomeSkill(int i, int j) {
    char c;
    int l;

    strcpy(per[i].ataques[0].nome, "FISICO");
    //le o primeiro caractere do nome
    c = fgetc(paBD);
    for (l = 0; c != ':'; l++) {
        per[i].ataques[j].nome[l] = c;
        c = fgetc(paBD);
    }
}

//funcao usada para limpar caracteres "lixo" do banco de dados
void limpaChar(FILE *pa) {
    char c;
    
    do {
        c = fgetc(pa);
    } while (c != ' ');
    
    return;
}




//########################### FUNCOES DE COMANDOS ##############################

//------------------------------- PRINCIPAIS -----------------------------------

int testaComando(int local, char *comando) {
    /*as funcoes que retornam pedindo atacante devem retornar -1*/
    if (strcmp(comando, "EXIT") == 0) {
        
        sair(local);
        
    } else if (strcmp(comando, "COMPATK") == 0 ||
               strcmp(comando, "COMPDANO") == 0) {
        
        compAtk(local);
        
    } else if (strcmp(comando, "USAKEN") == 0 ||
               strcmp(comando, "USAREN") == 0) {
        
        return usaKen(local);
        
    } else if (strcmp(comando, "REFLEXREN") == 0) {
        
        return reflexRen(local);
        
    } else if (strcmp(comando, "ROUGUEATK") == 0) {
        
        return rougueAtk(local);
        
    } else if (strcmp(comando, "REWIND") == 0) {
        
        reWind(local);
        
    } else if (strcmp(comando, "USEMAT") == 0 ||
               strcmp(comando, "USAMAT") == 0) {
        
        return useMat(local);
        
    } else if (strcmp(comando, "NERFATK") == 0 ||
               strcmp(comando, "NERFAATK") == 0) {
        
        return nerfAtk(local);
        
    } else if (strcmp(comando, "RESETPERS") == 0) {
        
        resetPers(local);
        
    } else if (strcmp(comando, "ZERASHUS") == 0) {
        
        zeraShus(local);
        
    } else if (strcmp(comando, "ADDSHU") == 0) {
        
        return addShu(local);
        
    } else if (strcmp(comando, "ADDSHUS") == 0) {
        
        addShus(local);
        
    } else if (strcmp(comando, "RODASHU") == 0 ||
               strcmp(comando, "CALCULASHU") == 0 ||
               strcmp(comando, "TURNO") == 0) {
        
        rodaShu(local);
        
    } else if (strcmp(comando, "RODASHUS") == 0 ||
               strcmp(comando, "TURNOS") == 0) {
        
        turnos(local);
        
    } else if (strcmp(comando, "GASTAREN") == 0) {
        
        gastaRen(local);
        
    } else if (strcmp(comando, "OPERAEXT") == 0 ||
               strcmp(comando, "OPERACORPO") == 0) {
        
        operaExt(local);
        
    } else if (strcmp(comando, "OPERAEXTS") == 0 ||
               strcmp(comando, "OPERACORPOS") == 0) {
        
        operaExts(local);
        
    } else if (strcmp(comando, "SHOWEXT") == 0 ||
               strcmp(comando, "SHOWCORPO") == 0) {
        
        showExt(local);
        
    } else if (strcmp(comando, "SHOWEXTS") == 0 ||
               strcmp(comando, "SHOWCORPOS") == 0) {
        
        showExts(local);
        
    } else if (strcmp(comando, "SHOWVOL") == 0) {
        
        return showAtkVol(local);
        
    } else if (strcmp(comando, "ZERAEXT") == 0 ||
               strcmp(comando, "RESETEXT") == 0 ||
               strcmp(comando, "ZERACORPO") == 0 ||
               strcmp(comando, "RESETCORPO") == 0) {
        
        zeraExt(local);
        
    } else if (strcmp(comando, "ZERAEXTS") == 0 ||
               strcmp(comando, "RESETEXTS") == 0 ||
               strcmp(comando, "ZERACORPOS") == 0 ||
               strcmp(comando, "RESETCORPOS") == 0) {
        
        zeraExts(local);
        
    /*} else if (strcmp(comando, "CAUSADANO") == 0) {
        
        causaDano(local);
    */    
    } else if (strcmp(comando, "PERFDAMAGE") == 0 ||
               strcmp(comando, "PERFATK") == 0) {
        
        perfDamage(local);
        
    } else if (strcmp(comando, "SHOWDAMAGE") == 0 ||
               strcmp(comando, "SHOWATK") == 0) {
        
        showDamage(local);
        
    } else if (strcmp(comando, "EDITAOP") == 0 ||
               strcmp(comando, "OPERAAOP") == 0 ||
               strcmp(comando, "OPERAOP") == 0) {
        
        editAop(local);
        
    } else if (strcmp(comando, "USEAOP") == 0) {
        
        useAop(local);
        
    } else if (strcmp(comando, "SHOWKENS") == 0 ||
               strcmp(comando, "SHOWRENS") == 0) {
        
        showKens(local);
        
    } else if (strcmp(comando, "SHOWKEN") == 0 ||
               strcmp(comando, "SHOWREN") == 0) {
        
        return showKen(local);
        
    } else if (strcmp(comando, "SHOWHPS") == 0) {
        
        showHps(local);
        
    } else if (strcmp(comando, "SHOWFHPS") == 0) {
        
        showFullHps(local);
        
    } else if (strcmp(comando, "SHOWHP") == 0) {
        
        showHp(local);
        
    } else if (strcmp(comando, "SHOWFHP") == 0) {
        
        showFullHp(local);
        
    } else if (strcmp(comando, "EDITHP") == 0) {
        
        editHp(local);
        
    } else if (strcmp(comando, "OPERAHP") == 0) {
        
        operaHp(local);
        
    } else if (strcmp(comando, "HEALAHP") == 0 ||
               strcmp(comando, "HEALAHP") == 0) {
        
        healaHp(local);
        
    } else if (strcmp(comando, "HEALAFULLHPS") == 0) {
        
        healaFullHps(local);
        
    } else if (strcmp(comando, "SAVEHPS") == 0) {
        
        saveHps(local);
        
    } else if (strcmp(comando, "SHOWMOPS") == 0) {
        
        showMops(local);
        
    } else if (strcmp(comando, "SHOWFMOPS") == 0) {
        
        showFullMops(local);
        
    } else if (strcmp(comando, "SHOWMOP") == 0) {
        
        return showMop(local);
        
    } else if (strcmp(comando, "SHOWFMOP") == 0) {
        
        showFullMop(local);
        
    } else if (strcmp(comando, "EDITMOP") == 0) {
        
        editMop(local);
        
    } else if (strcmp(comando, "OPERAMOP") == 0) {
        
        operaMop(local);
        
    } else if (strcmp(comando, "HEALAMOP") == 0 ||
               strcmp(comando, "HEALMOP") == 0) {
        
        healaMop(local);
        
    } else if (strcmp(comando, "HEALAFULLMOPS") == 0) {
        
        healaFullMops(local);
        
    } else if (strcmp(comando, "SAVEMOPS") == 0) {
        
        saveMops(local);
        
    } else if (strcmp(comando, "CLEAN") == 0 ||
               strcmp(comando, "CLS") == 0) {
        
        clean(local);
        
    } else if (strcmp(comando, "SKIPCOMMAND") == 0 ||
               strcmp(comando, "SKIPCOMANDO") == 0 ||
               strcmp(comando, "SKIP") == 0) {
        
        return skipCommand(local);
        
    } else if (strcmp(comando, "HELP") == 0) {
        
        help(local);
        
    } else if (strcmp(comando, "MSDOS") == 0 ||
               strcmp(comando, "CMD") == 0 ||
               strcmp(comando, "DOS") == 0) {
        
        msdos(local);
        
    } else {
        
        if (local == INICIAL) {
            return FALSE;
        }
    }
    return TRUE;
    //skipCommand so funciona eficientemente se nao houver esse return -1 acima
    //LISTA DE IFS E ELSE IFS COM OS COMANDOS E AS FUNCOES PARA OS COMANDOS
}

//------------------------- gasto e extensoes de corpo -------------------------

void operaExt(int local) {
    int opera, perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("Opere corpo=%d", per[perso].corpo);
        scanf("%d", &opera);
        per[perso].corpo += opera;
        printf("Corpo=%d\n", per[perso].corpo);
        linha();
    }
    return;
}

void operaExts(int local) {
    int qtd, i, temp;

    if (local == INICIAL) {
        temp = 0;
        do {
            printf("Qtos chars? ");
            scanf(" %d", &qtd);
            if (qtd > 9) {
                printf("[0]S [1]N\nCerteza? ");
                scanf("%d", &temp);
            }
        } while (temp != 0);

        for (i = 0; i < qtd; i++) {
            operaExt(local);
        }
        linha();
    }
    return;
}

void showExt(int local) {
    int perso;
    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d\n", per[perso].corpo);
        linha();
    }
    return;
}

void showExts(int local) {
    int i;
    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            printf("%d %s\n", per[i].corpo, per[i].nome);
        }
        linha();
    }
    return;
}

int showAtkVol(int local) {
    int atacante, i;
    
    if (local == INICIAL) {
        atacante = leAtacante("Atacante");
        
        for (i = 0; i < per[atacante].qtdAtaques; i++) {
            printf("Vol = %d", per[atacante].ataques[i].vol);
        }
    }
    return atacante;
}

void zeraExt(int local) {
    int perso;
    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        per[perso].corpo = 100;
        printf("Ext de %s zerado\n", per[perso].nome);
        linha();
    }
    return;
}

void zeraExts(int local) {
    int i;
    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            per[i].corpo = 100;
        }
        linha();
    }
    return;
}

//------------------------------ shu ------------------------------

void zeraShus(int local) {
    int i;
    if (local != FINAL) {
        for (i = 0; i < DIM; i++) {
            per[i].shu = 0;
        }
        
        if (local == INICIAL) {
            linha();
        }
    }
    return;
}

int addShu(int local) {//contar o corpo como shu tambem
    int qtd, temp;

    if (local == INICIAL) {
        
        atacante = leAtacante("Atacante");
        
        printf("[%d]%s esta usando %d SHU(s)\n",
                atacante, per[atacante].nome, per[atacante].shu);
        
    }
    //os if estao separados para que a chamada de addShu
    //direto da main, nao peça atacante novamente
    if (local != FINAL) {
        
        temp = 0;
        
        do {
            
            //leitura de qtd
            printf("[-1]Skip\nQTD de SHU: ");
            scanf("%d", &qtd);
            
            // se o valor entrado for > 9:
            if (qtd > 9) {
                printf("[0]S [1]N\nCerteza? ");
                scanf("%d", &temp);
            }
            
        } while (temp != 0);
        
        if (qtd == -1) {
            return -1;
        }
        
        //atribuindo valor lido no shu do personagem!!
        per[atacante].shu = qtd;
        
    }
    return atacante;
}

void addShus(int local) {
    int qtd, i, temp;

    if (local == INICIAL) {
        printf("Qtos chars? ");
        scanf(" %d", &qtd);
        for (i = 0; i < qtd; i++) {
            temp = addShu(local);
        }
        linha();
    }
    return;
}

void rodaShu(int local) {
    int i, temp;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            if (per[i].shu > 0) {
                if ((temp = 5 - per[i].cdn) > 0) {
                    printf("[%d]%s MOP=%d ", i, per[i].nome, per[i].mop);
                    per[i].mop -= per[i].shu * temp;
                    printf("MOP=%d\n", per[i].mop);
                }
            }
        }
        linha();
    }
    return;
}

//rodaShus
void turnos(int local) {
    int qtd, i, temp;

    if (local == INICIAL) {
        printf("Qts turnos? ");
        scanf(" %d", &qtd);
        
        if (qtd < 5) {
            for (i = 0; i < qtd; i++) {
                printf("Turno %d\n", i + 1);
                rodaShu(local);
            }
        } else {
            for (i = 0; i < DIM; i++) {
                if (per[i].shu > 0) {
                    if ((temp = 5 - per[i].cdn) > 0) {
                        printf("[%d]%s MOP=%d ", i, per[i].nome, per[i].mop);
                        per[i].mop -= per[i].shu * temp * qtd;
                        printf("MOP=%d\n", per[i].mop);
                    }
                }
            }
        }
        linha();
    }
    return;
}

//-------------------------------- atk e damage --------------------------------

int rougueAtk(int local) {
    int atk, perso, ataque, temp, qtd;
    char nome[TAM];

    if (local == INICIAL) {
        perso = leAtacante("Atacante");
        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == NULL) {
            printError("pilha");
            exit(1);
        }
        pilha[0].num = atk; //salva o atk para ser usaro no FINAL;

        perso = leAtacante("Ataque de");
        showSkills(perso);
        ataque = getSkill(perso);
        per[atk].qtdAtaques++; //aumenta em 1 a qtd de habilidades
        qtd = per[atk].qtdAtaques;
        per[atk].ataques = (SKILLS*)
                realloc(per[atk].ataques, qtd * sizeof (SKILLS));
        //passa o dano e o nome da habilidade alvo para o atk
        per[atk].ataques[qtd - 1].dano = per[perso].ataques[ataque].dano;
        //atualmente desnecessario (ou nao)
        strcpy(per[atk].ataques[qtd - 1].nome, per[perso].ataques[ataque].nome);

        printf("[0]S [1]N\nMudar dano? ");
        scanf("%d", &temp);
        if (temp != 1) {
            printf("New Damage: ");
            scanf("%d", &temp);
            per[atk].ataques[qtd - 1].dano = temp;
            printf("[0]S [1]N\nMudar bonus? ");
            scanf("%d", &temp);
            if (temp != 1) {
                puts(per[atk].ataques[qtd - 1].nome);
                printf("Novo nome: ");
                fgetspc(TAM, nome);
                strupr(nome);
                strcpy(per[atk].ataques[qtd - 1].nome, nome);
            }
        }
    } else {
        
        printf("A habilidade [0]fica ou [1]sai? ");
        scanf("%d", &temp);
        if (temp != 0) {//depois tentar declarar qtd e j aqui dentro
            atk = pilha[0].num;
            //free(pilha) ficava aqui. apagar isso
            per[atk].qtdAtaques--;
            qtd = per[atk].qtdAtaques;
            per[atk].ataques = (SKILLS*)
                    realloc(per[atk].ataques, qtd * sizeof (SKILLS));
            
            puts("Habilidade retirada");
        }
        free(pilha);
    }
    return atk;
}

int nerfAtk(int local) {
    int perso, ataque, reduz;

    if (local == INICIAL) {
        
        perso = leAtacante("Atacante");
        showSkills(perso);
        ataque = getSkill(perso);
        printf("Reduza-o: -");
        scanf("%d", &reduz);
        if (reduz > 0) {
            reduz *= -1;
        }

        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == NULL) {//era malloc
            printError("pilha");
            exit(1);
        }

        pilha[0].num = perso;
        pilha[0].valInt = ataque;
        pilha[0].valDouble = per[perso].ataques[ataque].dano;
        per[perso].ataques[ataque].dano += reduz; //reduz está negativo
        
    } else {
        
        per[pilha[0].num].ataques[pilha[0].valInt].dano = pilha[0].valDouble;
        free(pilha);
    
    }
    return perso;
}

void perfDamage(int local) {
    double temp;
    //essa função altera o aop do defensor até o reduzHP
    if (local == INICIAL) {
        defensor = leAtacante("Defensor");

        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == 0) {
            printError("pilha");
            exit(1);
        }//perso é desnec, pois eu poderia usar apenas pilha

        pilha[0].num = defensor;
        pilha[0].valInt = per[defensor].aop; //atk segura o aop de defesa
        pilha[0].valDouble = per[defensor].ten;
        temp = 1.0 * per[defensor].aop / 10;
        printf("temp = %lf", temp);
        per[defensor].aop = floor(temp);
        printf("aop = %d", per[defensor].aop);
        per[defensor].ten = 0;
        //per[defensor].aop-=(per[defensor].ten/2);
        
        linha();
        
    } else {
        
        per[pilha[0].num].aop = pilha[0].valInt;
        per[pilha[0].num].ten = (int) pilha[0].valDouble;
        free(pilha);
    
    }
    return;
}

void showDamage(int local) {
    int atacante, ataque;

    if (local == INICIAL) {
        atacante = leAtacante("Defensor");
        showSkills(atacante);
        ataque = getSkill(atacante);
        printf("Dano=%d\n", per[atacante].ataques[ataque].dano);
        linha();
    }
    return;
}

void compAtk(int local) {//NAO ESTA FUNFANDO
    int i, perso[2];
    int /*gastoAtk,*/ dano[2];
    //var neces. pq por algum motivo a func mult muda a var. ataque
    int copia1, copia2;
    int vaitomanocu;
    double mult;
    char a, *linha = "Perso ";
    ATAQUES ataque;

    if (local == INICIAL) {
        for (i = 0; i < 2; i++) {
            mult = ataque.qtds.qtd = ataque.crit = 1;
            
            itoa(++i, &a, 10);
            strcat(linha, &a);
            
            perso[i] = leAtacante(linha);
            printf("%d ", perso[i]);
            puts(per[perso[i]].nome);
            showSkills(perso[i]);
            ataque.atk = getSkill(perso[i]);
            copia1 = ataque.atk; //mult esta mudando ataque. por isso uso copia
            copia2 = perso[i];
            //segura o valor retornado, e a segunda pra passar o valor correto
            /*TETSE*/printf("%d ", perso[i]);
            vaitomanocu = pegaMult(&mult, copia2, &copia1);
            /*TETSE*/printf("%d ", perso[i]);
            
            leAtaque(&ataque);
            /*(&qtd, perso[i], ataque, &mat, &investida,
			&ryuAtk, critico, &dado, ems);
            */
            dano[i] = ataque.inv + mult * (ataque.ryu + ataque.dado);
        }
        printf("%s %d x %d %s",
        per[perso[0]].nome, dano[0], dano[1], per[perso[1]].nome);
    }
    //terminar, ou deixar assim
    return;
}

//------------------------------------- hp -------------------------------------

void showHps(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            printf("%03d ", per[i].hp);
            puts(per[i].nome);
        }
        linha();
    }
    return;
}

void showFullHps(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            printf("%03d ", per[i].hpFull);
            puts(per[i].nome);
        }
        linha();
    }
    return;
}

void showHp(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d ", per[perso].hp);
        puts(per[perso].nome);
        linha();
    }
    return;
}

void showFullHp(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d ", per[perso].hpFull);
        puts(per[perso].nome);
        linha();
    }
    return;
}

void editHp(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d <-antigo novo-> ", per[perso].hp);
        scanf("%d", &per[perso].hp);
        linha();
    }
    return;
}

void operaHp(int local) {
    int perso, opera;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("Opere HP=%d", per[perso].hp);
        scanf("%d", &opera);
        per[perso].hp += opera;
        printf("HP=%d\n", per[perso].hp);
        linha();
    }
    return;
}

void healaHp(int local) {
    int perso, teste;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("[0]S [1]N\nHealar FHP? ");
        scanf("%d", &teste);
        if (teste == 0) {
            per[perso].hp = per[perso].hpFull;
            printf("HP=%d\n", per[perso].hp);
        } else {
            puts("Operacao cancelada");
        }
        linha();
    }
    return;
}

void healaFullHps(int local) {
    int i, teste;

    if (local == INICIAL) {
        printf("[0]S [1]N\nHealar Full TODOS HPs? ");
        scanf("%d", &teste);
        if (teste == 0) {
            printf("Certeza? ");
            scanf("%d", &teste);
            if (teste == 0) {
                for (i = 0; i < DIM; i++) {
                    per[i].hp = per[i].hpFull;
                }
                puts("HPs Healados");
            } else {
                puts("Operacao cancelada");
            }
        } else {
            puts("Operacao cancelada");
        }
        linha();
    }
    return;
}

void saveHps(int local) {
    int i;
    if (local == INICIAL) {
        if ((paHP = fopen("BDHPs.txt", "w")) == NULL) {
            printError("paHPs");
            exit(1);
        }
        for (i = 0; i < DIM; i++) {
            fputs(per[i].nome, paHP);
            fputs(": ", paHP);
            //testar fprintf("paHP, "%d\n", per[i].hp);
            fprintf(paHP, "%d", per[i].hp);
            fputc('\n', paHP);
        }
        fclose(paHP);
        puts("Os HPs foram salvos");
        linha();
    }
    return;
}

//------------------------------------ mop -------------------------------------

void showMops(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            printf("%05d ", per[i].mop);
            puts(per[i].nome);
        }
        linha();
    }
    return;
}

void showFullMops(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            printf("%05d ", per[i].mopFull);
            puts(per[i].nome);
        }
        linha();
    }
    return;
}

int showMop(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d ", per[perso].mop);
        puts(per[perso].nome);
        linha();
    }
    return -1;
}

void showFullMop(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d ", per[perso].mopFull);
        puts(per[perso].nome);
        linha();
    }
    return;
}

void editMop(int local) {
    int perso, mop;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("%d <-antigo novo-> ", per[perso].mop);
        scanf("%d", &mop);
        if (mop < 50) {
            per[perso].mop = mop * 50;
        }
        else {
            per[perso].mop = mop;
        }
        linha();
    }
    return;
}

void operaMop(int local) {
    int perso, opera;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("Opere MOP=%d", per[perso].mop);
        scanf("%d", &opera);
        if (opera < 50) {
            opera *= 50;
        }
        per[perso].mop += opera;
        printf("MOP=%d\n", per[perso].mop);
        linha();
    }
    return;
}

void healaMop(int local) {
    int perso, teste;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        printf("[0]S [1]N\nHealar MOP? ");
        scanf("%d", &teste);
        if (teste == 0) {
            per[perso].mop = per[perso].mopFull;
            printf("MOP=%d", per[perso].mop);
        } else {
            puts("Operacao cancelada");
        }
        linha();
    }
    return;
}

void healaFullMops(int local) {
    int i, teste;

    if (local == INICIAL) {
        printf("[0]S [1]N\nHealar Full TODOS MOPs? ");
        scanf("%d", &teste);
        if (teste == 0) {
            printf("Certeza? ");
            scanf("%d", &teste);
            if (teste == 0) {
                for (i = 0; i < DIM; i++) {
                    per[i].mop = per[i].mopFull;
                }
                puts("MOPs Healados");
            } else {
                puts("Operacao cancelada");
            }
        } else {
            puts("Operacao cancelada");
        }
        linha();
    }
    return;
}

void saveMops(int local) {
    int i;
    if (local == INICIAL) {
        if ((paMOP = fopen("BDMOPs.txt", "w")) == NULL) {
            printError("paHPs");
            exit(1);
        }
        for (i = 0; i < DIM; i++) {
            fputs(per[i].nome, paMOP);
            fputs(": ", paMOP);
            //testar fprintf("paMOP, "%d\n", per[i].mop);
            fprintf(paMOP, "%d", per[i].mop);
            fputc('\n', paMOP);
        }
        fclose(paMOP);
        puts("Os MOPs foram salvos");
        linha();
    }
    return;
}

//------------------------------------- nen ------------------------------------

void useRealRyu(int local) {
    int personagem, qtd, i;
    PILHA *pilha;
    
    if(local == INICIAL) {
        printf("[1] [2] [0]Sai\nQuantos? ");
        scanf("%d", &qtd);
        
        if (qtd == 1 || qtd == 2) {
            for (i = 0; i < qtd; i++) {
                
                personagem = leAtacante("Atacante");
        
                //alocando memoria ao ponteiro pilha
                if ( (pilha = (PILHA*) calloc(qtd, sizeof (PILHA))) == NULL) {
                    printError("PILHA");
                    exit(1);
                }

                //passando valores (referencial e real e corpo) à struct pilha
                pilha[i].num = personagem;
                pilha[i].valInt = per[personagem].corpo;
                tamPilha = qtd;

                //alterando corpo, o falor real de ryu seja utilizado
                per[personagem].corpo = 100;
                
            }
            linha();
            
        } else {
            puts("Saindo");
            linha();
            return;
        }
    
    } else {
        
        for (i = 0; i < tamPilha; i++) {
            per[pilha[i].num].corpo = pilha[i].valInt;
        }
        free(pilha);
    
    }
    return;
}

int reflexRen(int local) {
    int i;
    char linha[4];
    double d100, temp;

    //pega a quantidade de personagens
    if (local == INICIAL) {
        
        printf("Qnts chars? ");
        fflush(stdin);
        fgets(linha, 4, stdin);
        if (linha[0] == '\n') {
            tamPilha = 1;
        }
        else if (linha[0] == '0') {
            puts("Saindo da funcao reflexRen");
            return -1;
        } else {
            tamPilha = atoi(linha);
        }

        //aloca espaco ao ponteiro de struct para armazenar os valores reais
        if ((pilha = (PILHA*) calloc(tamPilha, sizeof (PILHA))) == NULL) {
            printError("pilha");
            exit(1);
        }
        //for so é nec se +de 1 for surpr. no turno
        for (i = 0; i < tamPilha; i++) {
            
            (pilha + i)->num = leAtacante("Personagem");
            printf("[-1]z D100? ");
            scanf("%lf", &d100);
            if (d100 < 0) {
                i--; //se quero repetir o personagem, tenho que decrementar
                continue;
            }
            d100 /= 100;
            temp = d100 * per[(pilha + i)->num].aop; //temp recebe aop
            (pilha + i)->valDouble = per[(pilha + i)->num].aop;
            per[(pilha + i)->num].aop = floor(temp);
        }
        if (tamPilha == 1) {
            return (pilha + 1)->num;
        } else {
            return -1;
        }
    
    } else {
    
        for (i = 0; i < tamPilha; i++) {
            per[pilha[i].num].aop = pilha[i].valDouble;
        }
        free(pilha);
        return 0;
    
    }
}

int gastaRen(int local) {
    int temp;
    char linha[3];
    
    if (local == INICIAL) {
        
        temp = 0;
        atacante = leAtacante("Atacante");
    
    }
    //atencao:
    //os comandos abaixo sao para a chamada da main
    else if (local != INICIAL) {
    
        printf("[0]N [1]S\nUsou REN? ");
        fflush(stdin);
        fgets(linha, 3, stdin);
        
        if (linha[0] == '\n') {
            temp = 1;
        } else {
            temp = atoi(linha);
        }
        
        if (temp == 0) {
            return 0;
        }
    }
    //os ifs estao separados para que nao seja pedido
    //atacante novamente na chamada da funcao na main
    if (local != FINAL) {
        printf("MOP Atual=%d\n", per[atacante].mop);
        per[atacante].mop -= (per[atacante].aop * 50);
        printf("MOP Final=%d\n", per[atacante].mop);
    }
    return atacante;
}

int useMat(int local) {
    int i, atacante, ataque;
    char linha[5], opcl[5];

    if (local == INICIAL) {
        
        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == NULL) {
            printError("pilha");
            exit(1);
        }
        atacante = leAtacante("Atacante");
        showSkills(atacante);
        ataque = getSkill(atacante);
        printf("Qual o MAT? ");
        fflush(stdin);
        fgets(linha, 5, stdin);
        for (i = 0; i < 5; i++) {
            opcl[i] = '\0';
        }
        opcl[0] = '<';
        strcat(opcl, linha);
        opcl[strlen(opcl) - 1] = '>';

        strcpy(pilha[0].nome, per[atacante].ataques[ataque].nome);
        pilha[0].num = atacante;
        pilha[0].valInt = ataque;

        strcat(opcl, per[atacante].ataques[ataque].nome);
        strcpy(per[atacante].ataques[ataque].nome, opcl);
        //tem que salvar o nome original em uma variavel da pilha
        
    } else if (local == FINAL) {
        
        printf("Voltar nome antigo? [0]N [1]S? ");
        scanf("%d", &i);
        if (i != 0) {//uso i pra nao carregar mais variaveis desnecessariaas
            
            strcpy(per[pilha[0].num].ataques[pilha[0].valInt].nome,
                   pilha[0].nome);
            puts("Nome retornado");
        
        }
        free(pilha);
    }
    
    return atacante;
}

int editAop(int local) {
    int atacante, temp;

    if (local == INICIAL) {
        atacante = leAtacante("Atacante");

        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == 0) {
            printError("pilha");
            exit(1);
        }

        //printf("%d <-antigo novo-> ", ); TEMPORARIO
        pilha[0].num = atacante;
        pilha[0].valInt = per[atacante].aop;

        printf("Opere AOP=%d", per[atacante].aop);
        scanf("%d", &temp);
        per[atacante].aop += temp;
        
    } else if (local == FINAL) {
        
        printf("O AOP [0]fica ou [1]sai? ");
        scanf("%d", &temp);
        if (temp != 0) {//depois tentar declarar qtd e j aqui dentro
            per[pilha[0].num].aop = pilha[0].valInt;
            puts("AOP original restaurado");
        }
        free(pilha);
        
    }
    return atacante;
}

int useAop(int local) {
    int atacante, temp;

    if (local == INICIAL) {
        atacante = leAtacante("Atacante");

        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == 0) {
            printError("pilha");
            exit(1);
        }

        printf("%d <-antigo novo-> ", per[atacante].aop);
        scanf("%d", &temp);
        pilha[0].num = atacante;
        pilha[0].valInt = per[atacante].aop;
        per[atacante].aop = temp;
        
    } else if (local == FINAL) {
        
        printf("O AOP [0]fica ou [1]sai? ");
        scanf("%d", &temp);
        if (temp != 0) {//depois tentar declarar qtd e j aqui dentro
            per[pilha[0].num].aop = pilha[0].valInt;
            puts("AOP original restaurado");
        }
        free(pilha);
        
    }
    return atacante;
}

int usaKen(int local) {
    int atacante;

    if (local == INICIAL) {
        atacante = leAtacante("Atacante");
        printf("MOP=%d ", per[atacante].mop);
        per[atacante].mop -= per[atacante].aop * 50;
        printf("MOP=%d\n", per[atacante].mop);
    }
    return atacante;
}

int showKens(int local) {
    int atacante;
    float ren; //classe de aura
    int i;
    
    if (local == INICIAL) {
        atacante = leAtacante("Atacante");
        //calculo do ren 100% (KOU)
        ren = per[atacante].aop * (1 + per[atacante].pop / 10) *
                per[atacante].modClas;
        
        //imprimindo valores
        puts("Os valores de RYU sao:\n");
        for (i = 1; i < 10; i++) {
            printf("%0d = %.0lf\n", i * 5, ren * (i * 0.05));
        }
        printf("KEN = %.0lf\n", ren * 0.5);
        for (i = 11; i < 20; i++) {
            printf("%0d = %.0lf\n", i * 5, ren * (i * 0.05));
        }
        printf("KOU = %.0lf\n", ren);
    }
    return atacante;
}

int showKen(int local) {
    int atacante, porcent;
    float ren;

    if(local == INICIAL) {
        atacante = leAtacante("Atacante");
        printf("Qual a %%?");
        scanf(" %d", &porcent);
        //calculo do ren 100% (KOU)
        ren = per[atacante].aop * (1 + per[atacante].pop / 10) *
                per[atacante].modClas;
        printf("porcent%% = %.0lf\n", ren * 0.1 * porcent);
    }
    return atacante;
}

void showFullKens(int local) {
    int atacante, i;
    float ren;
    
    if (local == INICIAL) {
        atacante = leAtacante("Atacante");
        ren = per[atacante].aop * (1 + per[atacante].pop / 10) *
                per[atacante].modClas;
        
        for (i = 0; i < DIM; i++) {
            ren = per[i].aop * (1 + per[i].pop / 10) * per[i].modClas;
            printf("50%% = %.0lf - %s\n", ren * 0.05, per[i].nome);
        }
        linha();
    }
}

//----------------------------------- outros -----------------------------------

void resetPers(int local) {
    if (local == INICIAL) {
        leBD();
        zeraShus(local);
        zeraExts(local);
        puts("Personagens resetados");
        linha();
    }
    return;
}

int editCrit(int local) {
    int crit;
    if (local == INICIAL) {
        printf("Critico: x");
        scanf(" %d", &crit);
        return -1 * crit;
    }
}

int skipCommand(int local) {
    if (local == INICIAL) {
        puts("Saindo do comando");
        linha();
    }
    return RETURN;
}

void clean(int local) {
    if (local == INICIAL) {
        system("cls");
        linha();
    }
    return;
}

void sair(int local) {
    int teste;

    if (local == INICIAL) {
        printf("JA SALVOU? Fechar o programa? [0]N [1]S: ");
        scanf("%d", &teste);
        if (teste == 1) {
            puts("Fechando o programa");
            exit(0);
        }
    }
    return;
}

void reWind(int local) {
    int i, qtd;

    if (local == INICIAL) {
        qtd = tamSave + 1;
        while (qtd > tamSave) {
            printf("Qtas vezes: ");
            scanf("%d", &qtd);
        }
        if (qtd < 0) {
            return;
        }
        tamSave--;
        for (i = 0; i < qtd; i++, tamSave--) {
            per[save[tamSave].atacante].mop = save[tamSave].mop;
            per[save[tamSave].defensor].hp = save[tamSave].hp;
        }
        tamSave++;
        //save=(SAVE*)realloc(save, (tamSave+2)*sizeof(SAVE));
        printf("Os HPs e MOPs foram retornados %d turno(s) atras\n", qtd);
        linha();
    }
    return;
}

void msdos(int local) {
    char line[100];
    
    if (local == INICIAL) {
        printf("Comando: ");
        fgetspc(100, line);
        system(line);
        linha();
    }
    return;
}

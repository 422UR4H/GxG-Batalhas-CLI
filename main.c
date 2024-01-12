#include "interface.h"
#include "algoritmo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define TAM 40
#define DIM 46 //DIM guarda a quantidade de personagens no banco de dados
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

 
//lista de variaveis globais
FILE *paBD, *paHP, *paBU, *paMOP;
PILHA *pilha;
SAVE *save;
PERSONAGEM per[DIM];
char comando[TAM];
int tamPilha;
int atacante, defensor, tamSave;


//########################### PROTOTIPO DE FUNCOES #############################


int leAtacante(char*);
int leDef(int*, double*, int*);
int showKens(int);
int showKen(int);
void showFullKens(int);
void useRealRyu(int);
int volCorpo(void);
int pegaMult(double*, int, int*);
int testaComando(int, char*);
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
int leAtaque(ATAQUES*);
//void causaDano(int);
int suportVolCorpo(int, int);


//############################# INICIO DO CODIGO ###############################

int main() {
	
	int i, temp, defesa;
    double mult, ryuDef;
    ATAQUES ataque;
    GASTO gasto;
    DANO dano;
	//abaixo, coloque o diretorio onde os BDs se encontram"!!
	//mas ATENCAO! \\ no lugar de \ 
	//e tambem mude na funcao resetPers
	char dir[] = "C:\\Users\\Samsung\\Desktop\\3\\bds\\";
	
	
    system("title GxG - Batalhas 2.0");
    system("color b");
    system("cls");
	
	atacante = -1;
	defensor = -1;
	tamSave = 0;
	
    //carrega-se a 1 memoria para save e carregado valores de banco de dados
    save = (SAVE*) calloc(1, sizeof (SAVE));
    if (leBD(dir) == FALSE) {
        printError("FILE paBD ou ataques");
        exit(1);
    }
    if (leVol(dir) == FALSE) {
        printError("FILE paSkills");
        exit(1);
    }
    if (leHP(dir) == FALSE) {
        printError("FILE paHPs");
        exit(1);
    }
    if (leMOP(dir) == FALSE) {
        printError("FILE paMOPs");
        exit(1);
    }
    zeraShus(2);
    //passando parametro diferente de inicial e final
    //para que linha nao seja mostrada novamente
    
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
            //adiciona corpo, e mostra o numero, nome e skills
            temp = volCorpo();
			per[atacante].corpo += temp;
			//caso o usuario decida nao add corpo ao atacante,
			//nao seria printado os ataques para ele,
			//esse if serve para que seja mostrado mesmo assim:
			if (temp == FALSE) {
				printDados(atacante);
			}
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
        defesa = leDef(&defensor, &ryuDef, &defesa);
        if (defesa == -1) {
            zeraAtaques(&ataque);
            continue;
        } else if (defesa == TRUE) {
            defesa += per[defensor].modFor;
        }
        defesa = calculaDef(ryuDef, defesa);
		
        temp = aplicaDano(defesa, mult, ataque, &dano);
        if (temp == FALSE) {
            printMiss(defesa, dano.dano);
        } else if (temp == TRUE) {
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
            printGasto(defensor, per[atacante].mop, gasto.mopAnterior);
        }
        
        //backup automatico de hps e mops
        backSave();

        //execucao da parte final do codigo de um comando
        testaComando(FINAL, comando);
        
        //é feito um backup do hp, em um banco de dados externo
        backuphps(dir); //backupmops();
        
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
    } else if (corp == RETURN) {
        return RETURN;
    } else {
        
        //printa as opcoes e le a escolhida (qtd skills)
        op = opcoes();
		//mostrando o numero, nome e as habilidades do atacante
		printDados(atacante);
		
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
    
    //atribuindo ataque
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

int leDef(int *defensor, double *ryuDef, int *defesa) {
    int valor;
    
    //Valor de retorno: se leDefensor retornar -1,
    //significa que o usuario preferiu zerar as leituras
    //para qualquer outro valor -1 entrado, a funcao
    //usa recursao apenas para reler os valores de def
    if ( (*defensor = leDefensor(*defensor)) < 0) {
        return -1;
    }
    if ( (*ryuDef = leRyuDef(*defensor)) < 0) {
        return leDef(defensor, ryuDef, defesa);
    }
    if ( (*defesa = leDefAdc()) < 0) {
        return leDef(defensor, ryuDef, defesa);
    }
    if ( (valor = leSeDefendeu()) < 0) {
        return leDef(defensor, ryuDef, defesa);
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
                
                if (atacante == -2) {
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
		
	} else if (strcmp(comando, "SHOWFULLKENS") == 0 ||
			   strcmp(comando, "SHOWFULLRENS") == 0) {
		
		showFullKens(local);
        
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
            return -2;
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
        //le opera
        opera = operaExtIn(INICIAL, per[perso].corpo);
        //calcula opera
        operaExtAl(perso, opera);
        //printa corpo; atribuicao inutil
        opera = operaExtIn(FINAL, per[perso].corpo);
    }
    return;
}

void operaExts(int local) {
    int qtd, i, temp;

    if (local == INICIAL) {
        temp = 0;
        do {
            qtd = operaExtsIn(INICIAL);
            if (qtd > 9) {
                temp = operaExtsIn(FINAL);
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
        showExtIn(perso);
    }
    return;
}

void showExts(int local) {
    int i;
    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            showExtsIn(i);
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
            showAtkVolIn(atacante, i);
        }
    }
    return atacante;
}

void zeraExt(int local) {
    int perso;
    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        zeraExtAl(perso);
        zeraExtIn(per[perso].nome);
    }
    return;
}

void zeraExts(int local) {
    int i;
    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            zeraExtsAl(i);
        }
        zeraExtsIn();
    }
    return;
}

//------------------------------ shu ------------------------------

void zeraShus(int local) {
    int i;
    if (local != FINAL) {
        for (i = 0; i < DIM; i++) {
            zeraShusAl(i);
        }
        if (local == INICIAL) {
            zeraShusIn();
        }
    }
    return;
}

int addShu(int local) {//contar o corpo como shu tambem
    int qtd, temp;

    if (local == INICIAL) {
        
        atacante = leAtacante("Atacante");
        //printa quanto de shu o atacante esta usando
        addShuPrint(atacante, per[atacante].shu, per[atacante].nome);
        
    }
    //os if estao separados para que a chamada de addShu
    //direto da main, nao peça atacante novamente
    if (local != FINAL) {
        
        do {
			temp = 0;
            
			//leitura de qtd e teste de return
            if ( (qtd = addShuIn(INICIAL)) == RETURN) {
                return RETURN;
            }
            // se o valor entrado for > 9:
            if (qtd > 9) {
                //confirmando...
                temp = addShuIn(FINAL);
            }
        } while (temp != 0);
        
        //atribuindo valor lido no shu do personagem!!
        addShuAl(atacante, qtd);
        
    }
    return atacante;
}

void addShus(int local) {
    int qtd, i, temp;

    if (local == INICIAL) {
        qtd = addShusIn();
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
        //roda todos os personagens
        for (i = 0; i < DIM; i++) {
            //todos que tem shu...
            if (per[i].shu > 0) {
                //sao calculados aqui:
                if ((temp = 5 - per[i].cdn) > 0) {
                    rodaShuIn(INICIAL, i, per[i].mop, per[i].nome);
                    rodaShuAl(i, temp);
                    rodaShuIn(FINAL, i, per[i].mop, per[i].nome);
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
        //segundo parametro inutil
        qtd = turnosIn1(INICIAL, FALSE);
        if (qtd < 5) {
            for (i = 0; i < qtd; i++) {
                //atribuicao inutil
                temp = turnosIn1(FINAL, i);
				rodaShu(local);
            }
        } else {
            for (i = 0; i < DIM; i++) {
                if (per[i].shu > 0) {
                    if ((temp = 5 - per[i].cdn) > 0) {
                        turnosIn2(INICIAL, i, per[i].mop, per[i].nome);
                        turnosAl(temp, qtd, i);
                        turnosIn2(INICIAL, i, per[i].mop, per[i].nome);
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
            return RETURN;
        }
        pilha[0].num = atk; //salva o atk para ser usado no FINAL;
        
        //lendo personagem e ataque
        perso = leAtacante("Ataque de");
        showSkills(perso);
        ataque = getSkill(perso);
        
        per[atk].qtdAtaques++; //aumenta em 1 a qtd de habilidades
        qtd = per[atk].qtdAtaques;
        
        //alocando memoria
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
        //lendo um valor legativo para reducao
        reduz = nerfAtkIn(INICIAL, per[perso].ataques[ataque].dano);
        
        //alocacao de memoria
        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == NULL) {
            printError("pilha");
            return RETURN;
        }
        
        //salvando valores em pilha
        nerfAtkAl(perso, ataque, reduz, pilha);
        //atribuicao inutil
        reduz = nerfAtkIn(FINAL, per[perso].ataques[ataque].dano);
        
    } else {
        
        nerfAtkAl2(pilha);
        free(pilha);
    
    }
    return perso;
}

void perfDamage(int local) {
    //essa função altera o aop do defensor até o reduzHP
    if (local == INICIAL) {
        defensor = leAtacante("Defensor");
        
        //alocando memoria para pilha
        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == 0) {
            printError("pilha");
            return;
        }//perso é desnec, pois apenas pilha pode ser usado
        
        //algoritmo de dano perfurante
        perfDamageAl(INICIAL, defensor, pilha);
        
        linha();
    } else {
        perfDamageAl(FINAL, defensor, pilha);
    }
    return;
}

void showDamage(int local) {
    int atacante, ataque;
    
    if (local == INICIAL) {
        atacante = leAtacante("Defensor");
        showSkills(atacante);
        ataque = getSkill(atacante);
        showDamageIn(per[atacante].ataques[ataque].dano);
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
            hpPrint(i, per[i].hp);
        }
        linha();
    }
    return;
}

void showFullHps(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            hpPrint(i, per[i].hpFull);
        }
        linha();
    }
    return;
}

void showHp(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        hpPrint(perso, per[perso].hp);
        linha();
    }
    return;
}

void showFullHp(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        hpPrint(perso, per[perso].hpFull);
        linha();
    }
    return;
}

void editHp(int local) {
    int perso, hp;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        hp = editHpIn(perso);
        editHpAl(perso, hp);
    }
    return;
}

void operaHp(int local) {
    int perso, opera;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        //atribuicao de operacao lida
        opera = operaHpIn(INICIAL, perso);
        //algoritmo da funcao, operacao de hp
        operaHpAl(opera, perso);
        //atribuicao inutil
        opera = operaHpIn(FINAL, perso);
        linha();
    }
    return;
}

void healaHp(int local) {
    int perso, teste;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        teste = healaHpIn();
        if (teste == 0) {
            healaHpAl(perso);
            healaHpPrint(INICIAL, perso);
        } else {
            healaHpPrint(FINAL, perso);
        }
        linha();
    }
    return;
}

void healaFullHps(int local) {
    int i, teste;

    if (local == INICIAL) {
        teste = healaFullHpsIn(INICIAL);
        if (teste == 0) {
            teste = healaFullHpsIn(FINAL);
            if (teste == 0) {
                for (i = 0; i < DIM; i++) {
                    healaHpAl(i);
                }
                healaFullHpsPrint(INICIAL);
            } else {
                healaFullHpsPrint(FINAL);
            }
        } else {
            healaFullHpsPrint(FINAL);
        }
        linha();
    }
    return;
}

void saveHps(int local) {
    int i;
    if (local == INICIAL) {
        if ((paHP = fopen("C:\\Users\\Samsung\\Desktop\\3\\bds\\BDHPs.txt", "w")) == NULL) {
            printError("paHPs");
            return;
        }
        for (i = 0; i < DIM; i++) {
            fputs(per[i].nome, paHP);
            fputs(": ", paHP);
            //testar fprintf("paHP, "%d\n", per[i].hp);
            fprintf(paHP, "%d", per[i].hp);
            fputc('\n', paHP);
        }
        fclose(paHP);
    }
    return;
}

//------------------------------------ mop -------------------------------------

void showMops(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            mopPrint(i, per[i].mop);
        }
        linha();
    }
    return;
}

void showFullMops(int local) {
    int i;

    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            mopPrint(i, per[i].mopFull);
        }
        linha();
    }
    return;
}

int showMop(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        mopPrint(perso, per[perso].mop);
        linha();
    }
    return -1;
}

void showFullMop(int local) {
    int perso;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        mopPrint(perso, per[perso].mopFull);
        linha();
    }
    return;
}

void editMop(int local) {
    int perso, mop;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        mop = editMopIn(perso);
        editHpAl(perso, mop);
    }
    return;
}

void operaMop(int local) {
    int perso, opera;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        //atribuicao de operacao lida
        opera = operaMopIn(INICIAL, perso);
        //algoritmo da funcao, operacao de hp
        operaMopAl(opera, perso);
        //atribuicao inutil
        opera = operaMopIn(FINAL, perso);
        linha();
    }
    return;
}

void healaMop(int local) {
    int perso, teste;

    if (local == INICIAL) {
        perso = leAtacante("Personagem");
        teste = healaMopIn();
        if (teste == 0) {
            healaMopAl(perso);
            healaMopPrint(INICIAL, perso);
        } else {
            healaMopPrint(FINAL, perso);
        }
        linha();
    }
    return;
}

void healaFullMops(int local) {
    int i, teste;

    if (local == INICIAL) {
        teste = healaFullMopsIn(INICIAL);
        if (teste == 0) {
            teste = healaFullMopsIn(FINAL);
            if (teste == 0) {
                for (i = 0; i < DIM; i++) {
                    healaMopAl(i);
                }
                healaFullMopsPrint(INICIAL);
            } else {
                healaFullMopsPrint(FINAL);
            }
        } else {
            healaFullMopsPrint(FINAL);
        }
        linha();
    }
    return;
}

void saveMops(int local) {
    int i;
    if (local == INICIAL) {
        if ((paHP = fopen("C:\\Users\\Samsung\\Desktop\\3\\bds\\BDHPs.txt", "w")) == NULL) {
            printError("paHPs");
            return;
        }
        for (i = 0; i < DIM; i++) {
            fputs(per[i].nome, paHP);
            fputs(": ", paHP);
            //testar fprintf("paHP, "%d\n", per[i].hp);
            fprintf(paHP, "%d", per[i].hp);
            fputc('\n', paHP);
        }
        fclose(paHP);
    }
    return;
}

//------------------------------------- nen ------------------------------------

void useRealRyu(int local) {
    int perso, qtd, i;
    PILHA *pilha;
    
    if(local == INICIAL) {
        //le qtd
        qtd = useRealRyuIn();
        
        if (qtd == 1 || qtd == 2) {
            for (i = 0; i < qtd; i++) {
                
                perso = leAtacante("Atacante");
                
                //alocando memoria ao ponteiro pilha
                if ( (pilha = (PILHA*) calloc(qtd, sizeof (PILHA))) == NULL) {
                    printError("PILHA");
                    return;
                }
                //algoritmos
                tamPilha = qtd;
                useRealRyuAl(i, perso);
            }
            linha();
            
        } else {
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
    double d100;

    //pega a quantidade de personagens
    if (local == INICIAL) {
        
        //le qtd chars; e sai da funcao se o usuario quiser
        if (reflexRenIn(INICIAL) == RETURN) {
            return RETURN;
        }
        
        //aloca espaco ao ponteiro de struct para armazenar os valores reais
        if ((pilha = (PILHA*) calloc(tamPilha, sizeof (PILHA))) == NULL) {
            printError("pilha");
            return RETURN;
        }
        
        //for so é nec se +de 1 for surpr. no turno
        for (i = 0; i < tamPilha; i++) {
            
            pilha[i].num = leAtacante("Personagem");
            
            d100 = reflexRenIn(FINAL);
            
            if (d100 < 0) {
                i--; //se quero repetir o personagem, tenho que decrementar
                continue;
            }
            
            reflexRenAl(INICIAL, d100, &pilha[i].num);
            
        }
        if (tamPilha == 1) {
            return (pilha + 1)->num;
        } else {
            return RETURN;
        }
    
    } else {
    
        for (i = 0; i < tamPilha; i++) {
            reflexRenAl(FINAL, i, &pilha[i].num);
        }
        free(pilha);
        return FALSE;
    
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
    else if (local != FINAL) {
        
        //segundo parametro inutil
        temp = gastaRenIn(INICIAL, per[atacante].mop);
        
        if (temp == 0) {
            return 0;
        }
    }
    //os ifs estao separados para que nao seja pedido
    //atacante novamente na chamada da funcao na main
    if (local != FINAL) {
        
        //atribuicao inutil
        temp = gastaRenIn(FINAL, per[atacante].mop);
        //algoritmo
        gastaRenAl();
        //atribuicao inutil
        temp = gastaRenIn(FINAL, per[atacante].mop);
        
    }
    return atacante;
}

int useMat(int local) {
    int i, atacante, ataque;
    char linha[5], opcl[5];

    if (local == INICIAL) {
        
        if ((pilha = (PILHA*) calloc(1, sizeof (PILHA))) == NULL) {
            printError("pilha");
            return RETURN;
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
            return RETURN;
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
            return RETURN;
        }
        
        //le temp
        temp = useAopIn(INICIAL, per[atacante].aop);
        //algoritmo
        useAopAl(atacante, pilha);
        
    } else if (local == FINAL) {
        
        //segundo parametro inutil
        temp = useAopIn(FINAL, temp);;
        if (temp != 0) {//depois tentar declarar qtd e j aqui dentro
            per[pilha[0].num].aop = pilha[0].valInt;
            //atribuicao e segundo paremetro uniteis
            temp = useAopIn(2, temp);
        }
        free(pilha);
    }
    return atacante;
}

int usaKen(int local) {
    int atacante;

    if (local == INICIAL) {
        atacante = leAtacante("Atacante");
        usaKenIn(per[atacante].mop);
        useKenAl(per[atacante].aop);
        usaKenIn(per[atacante].mop);
    }
    return atacante;
}

int showKens(int local) {
    int atacante;
    double ren; //classe de aura
    
    if (local == INICIAL) {
        atacante = leAtacante("Atacante");
        //calculo do ren 100% (KOU)
        ren = showKensAl(atacante, 100);
        //imprimindo valores
        showKensIn(ren);
    }
    return atacante;
}

int showKen(int local) {
    int atacante, porcent;
    float ren;

    if(local == INICIAL) {
        atacante = leAtacante("Atacante");
        //pede e recebe porcentagem
        porcent = showKenIn();
        //calculo do ren 100% (KOU)
        ren = showKensAl(atacante, porcent);
        //ultimo print
        showKenPrint(porcent, ren);
    }
    return atacante;
}

void showFullKens(int local) {
    int atacante, i;
    float ren;
    
    if (local == INICIAL) {
        for (i = 0; i < DIM; i++) {
            ren = per[i].aop * (1 + 1.0 * per[i].pop / 10) * per[i].modClas;
			puts(per[i].nome);
			printf("TEN = %.0lf\n", ren * 0.1);
            printf("KEN = %.0lf\n", ren * 0.5);
			printf("KOU = %.0lf\n", ren);
			printf("\n");
        }
        linha();
    }
}

//----------------------------------- outros -----------------------------------

void resetPers(int local) {
    if (local == INICIAL) {
		char dir[] = "C:\\Users\\Samsung\\Desktop\\3\\bds\\";
		leBD(dir);
		zeraShus(local);
		zeraExts(local);
        resetPersIn();
    }
    return;
}

int editCrit(int local) {
    if (local == INICIAL) {
        return -1 * editCritIn();
    }
}

int skipCommand(int local) {
    if (local == INICIAL) {
        skipCommandIn();
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
        teste = sairIn(INICIAL);
        if (teste == 1) {
            //atribuicao inutil
            teste = sairIn(FINAL);
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
            qtd = reWindIn(INICIAL);
        }
        if (qtd < 0) {
            return;
        }
        tamSave--;
        for (i = 0; i < qtd; i++, tamSave--) {
            reWindAl(tamSave);
        }
        tamSave++;
        //save=(SAVE*)realloc(save, (tamSave+2)*sizeof(SAVE));
        qtd = reWindIn(FINAL);
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

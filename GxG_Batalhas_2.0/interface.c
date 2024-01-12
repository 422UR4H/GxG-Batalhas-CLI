#include "interface.h"


//lista de variaveis globais
FILE *paBD, *paHP, *paBU, *paMOP;
PILHA *pilha;
SAVE *save;
PERSONAGEM *per;
char comando[TAM];
int tamPilha;
int atacante, defensor, tamSave;


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
    int temp;

    switch (op) {
        case 1:
            printf("Qual? ");
            scanf("%d", &temp);
            return temp;

        case 2:
            printf("Quais? ");
            return 3;

        case 3:
            scanf(" %d", &temp);
            return temp;

        case 4:
            printf("Quantas? ");
            scanf("%d", &temp);
            return temp;

        case 5:
            printf("Skills: ");
            return 3;

        case 6:
            printf("Volume: ");
            scanf(" %d", &temp);
            return temp;

        case 7:
            puts("Resposta Invalida");
            return FALSE;
    }
    return FALSE;
}

int leCorp() {
    char linha[3];

    printf("[0]N [1]S\nAdd Corp? ");
    fflush(stdin);
    fgets(linha, 3, stdin);

    if (linha[0] == '0') {
        return FALSE;
    } else if (linha[0] == '-') {
        return RETURN;
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
    printf("Qts Skills? ");
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

int leDefensor(int defensor) {
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
        } else if ( (defensor = valorChar(linha)) == -1) {
            return leDefensor(defensor);
        }
    }
    return defensor;
}

double leRyuDef(int defensor) {
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

void printGasto(int defensor, int mopFinal, int mopAnterior) {

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
                "[skip]---------cancela o pedido de um comando\n"//ou skipcommand
                "[rewind]-------(ctrl+z) retorna hp e mop\n"
                "[msdos]--------permite um comando em DOS\n"
                "[saveall]------salva hps e mops\n"
                "[exit]---------fecha o programa\n"
                "[clean]--------limpa a tela\n"
                "=========================== SHUS ===========================\n"
                "[addshu]-------aumenta ou diminui um shu de um personagem\n"
                "[rodashu]------roda um turno, calculando os gastos de mop\n"
                "[addshus]------aumenta ou diminui o shu de personagens\n"
                "[turnos]-------roda mais de um turno, gastando mop\n"
                "[zerashus]-----zera todos os shus dos personagens\n"
                "==================== EXTENSOES DE CORPO ====================\n"
                "[operaext]-----add extensoes ao corpo do char, red. seu ren\n"
                "[showext]------mostra qtd de extensoes de corpo de um char\n"
                "[showexts]-----mostra qtd de exts de corpo de varios chars\n"
                "[showvol]------mostra o volume do ataque de um personagem\n"
                "[zeraexts]-----atribui 100 a extensao de todos os chars\n"
                "[zeraext]------atribui 100 a extensao de um personagem\n"
                "========================== ATAQUES =========================\n"
                //usa a funcao calculaDano para calcular na defesa
                //"[causadano]----aplica um dano ja calculado direto na defesa\n"
                "[rougueatk]----permite um jogador usar o ataque de outro\n"
                "[nerfatk]------diminui o dano de um atk nesse turno\n"
                //"[contratk]-----permite o contra-ataque do defensor\n"
                //"[saveatk]------salva variaveis de um ataque\n"
                //"[compatk]------compara 2 ataques\n"//ou comparaatk ou comparatk
                "=========================== DANO ===========================\n"
                "[usemat]-------usa o bonus de mat\\man num dano que nao o tem\n"
                "[useRealRyu]---usa o valor de ryu inalterado por corpo\n"
                "[showFullKens]-mostra o KEN de todos os personagens\n"
                //"[cleanaop]-----limpa o aop inserido\n"//SO SERVE P MULTIFUNCS
                "[resetpers]----reseta as alteracoes feitas nos pers\n"
                //"[resetper]-----reseta a alteracao feita em um personagem\n"
                "[perfdamage]---dano perfurante (ou em en)\n"
                "[showdamage]---mostra o dano de um ataque\n"
                "[editaop]------altera um aop utilizavel\n"
                "[useaop]-------usa o aop inserido\n"
                //"[areadamage]---dano em area\n"
                "============================ REN ===========================\n"
                "[reflexren]----limita o aop de atk nesse turno\n"
                "[gastaren]-----gasta o ren (quando é ativado)\n"
                //INICIAL; FINAL pede dano full e pede novamente as funcs de def
                "============================ HPs ===========================\n"
                "[showfhp]------mostra um hp total especifico\n"
                "[showhp]-------mostra um hp especifico\n"
                "[showfhps]-----mostra os hps totais\n"
                "[showhps]------mostra os hps atuais\n"
                "[operahp]------soma ou dimiui um hp\n"
                "[savehps]------salva os hps no BD\n"
                "[healafullhps]-heala todos os hps\n"
                "[savehp]-------salva um hp no BD\n"
                "[healahp]------heala full um hp\n"
                "[edithp]-------edita um hp\n"
                "=========================== MOPs ===========================\n"
                "[showfmop]-----mostra um mop total especifico\n"
                "[showmop]------mostra um mop especifico\n"
                "[operamop]-----soma ou siminui um mop\n"
                "[showmops]-----mostra os mops atuais\n"
                "[showfmops]----mostra os mops totais\n"
                "[savemops]-----salva os mops no BD\n"
                "[healamop]-----heala full um mop\n"
                "[healafullmops]heala full mops\n"
                "[editmop]------edita um mop\n");
                //"[savemop]------salva um mop no BD\n"
                linha();
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
    for (i = 0; strcmp(str, per[i].nome) != 0 && i < getQtdFichas(); i++);
    if (i >= getQtdFichas()) {
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

//########################### FUNCOES DE COMANDOS ##############################

int operaExtIn(int local, int corpo) {
    int opera;

    if (local == INICIAL) {
        printf("Opere corpo=%d", corpo);
        scanf("%d", &opera);
    } else {
        printf("Corpo=%d\n", corpo);
        linha();
    }
    return opera;
}

int operaExtsIn(int local) {
    int num;

    if (local == INICIAL) {
        printf("Qtos chars? ");
        scanf(" %d", &num);
    } else {
        printf("[0]S [1]N\nCerteza? ");
        scanf("%d", &num);
    }
    return num;
}

void showExtIn(int perso) {
    printf("%d\n", per[perso].corpo);
    linha();
    return;
}

void showExtsIn(int i) {
    printf("%d %s\n", per[i].corpo, per[i].nome);
    return;
}

void showAtkVolIn(int atacante, int i) {
    printf("Vol = %.2d - %s", per[atacante].ataques[i].vol,
           per[atacante].ataques[i].nome);
    return;
}

void zeraExtIn(char *nome) {
    printf("Ext de %s zerado\n", nome);
    linha();
    return;
}

void zeraExtsIn() {
    puts("Extensoes de corpo zeradas");
    linha();
    return;
}

//------------------------------ shu ------------------------------

void zeraShusIn() {
    puts("Todos os SHUs zerados!");
    linha();
    return;
}

void addShuPrint(int atacante, int shu, char *nome) {
    printf("[%d]%s esta usando %d SHU(s)\n", atacante, nome, shu);
    return;
}

int addShuIn(int local) {
    int num;
    char linha[4];

    if (local == INICIAL) {
        printf("[-1]Skip\nQTD de SHU: ");
        fgetspc(4, linha);
        if ( (num = atoi(linha) ) == 0) {
            return addShuIn(local);
        }
        return num;
    } else {
        printf("[0]S [1]N\nCerteza? ");
        fgetspc(4, linha);
        if ( (num = atoi(linha) ) == 0) {
            return addShuIn(local);
        }
        return num;
    }
}

int addShusIn() {
    int qtd;

    printf("Qtos chars? ");
    scanf(" %d", &qtd);
    return qtd;
}

void rodaShuIn(int local, int i, int mop, char *nome) {
    if (local == INICIAL) {
        printf("[%d]%s MOP=%d ", i, nome, mop);
    } else {
        printf("MOP=%d\n", mop);
    }
    return;
}

int turnosIn1(int local, int i) {
    int qtd;
    if (local == INICIAL) {
        printf("Qts turnos? ");
        scanf(" %d", &qtd);
        return qtd;
    } else {
        printf("Turno %d\n", i + 1);
        //retorno inutil
        return qtd;
    }
}

void turnosIn2(int local, int i, int mop, char *nome) {
    if (local == INICIAL) {
        printf("[%d]%s MOP=%d\t", i, per[i].nome, per[i].mop);
    } else {
        printf("MOP=%d\n", per[i].mop);
    }
    return;
}

//-------------------------------- atk e damage --------------------------------

int nerfAtkIn(int local, int dano) {
    int reduz;

    if (local == INICIAL) {
        //leitura
        printf("Reduza-o %d - ", dano);
        scanf("%d", &reduz);
        //forcando um valor negativo
        if (reduz > 0) {
            reduz *= -1;
        }
        return reduz;
    } else {
        printf("Dano=%d\n", dano);
        return TRUE; // retorno inutil
    }
}

void showDamageIn(int dano) {
    printf("Dano=%d\n", dano);
    linha();
    return;
}

//------------------------------------- hp -------------------------------------

void hpPrint(int i, int hp) {
    printf("%03d ", hp);
    puts(per[i].nome);
    return;
}


int editHpIn(int perso) {
    int hp;

    printf("%d <-antigo novo-> ", per[perso].hp);
    scanf("%d", &hp);
    linha();
    return hp;
}

int operaHpIn(int local, int perso) {
    int opera;

    if (local == INICIAL) {
        printf("Opere HP=%d", per[perso].hp);
        scanf("%d", &opera);
        return opera;
    } else {
        printf("HP=%d\n", per[perso].hp);
        return opera;
    }
}

int healaHpIn() {
    int teste;

    printf("[0]S [1]N\nHealar FHP? ");
    scanf("%d", &teste);
    return teste;
}

void healaHpPrint(int local, int perso) {
    if (local == INICIAL) {
        printf("HP=%d\n", per[perso].hp);
    } else {
        puts("Operacao cancelada");
    }
    return;
}

int healaFullHpsIn(int local) {
    int teste;

    if (local == INICIAL) {
        printf("[0]S [1]N\nHealar Full TODOS HPs? ");
        scanf("%d", &teste);
    } else {
        printf("Certeza? ");
        scanf("%d", &teste);
    }
    return teste;
}

void healaFullHpsPrint(int local) {
    if (local == INICIAL) {
        puts("HPs Healados");
    } else {
        puts("Operacao cancelada");
    }
    return;
}

void saveHpsIn() {
    puts("Os HPs foram salvos");
    linha();
    return;
}

//------------------------------------ mop -------------------------------------

void mopPrint(int i, int mop) {
    printf("%03d ", mop);
    puts(per[i].nome);
    return;
}

int editMopIn(int perso) {
    int mop;

    printf("%d <-antigo novo-> ", per[perso].mop);
    scanf("%d", &mop);
    linha();
    return mop;
}

int operaMopIn(int local, int perso) {
    int opera;

    if (local == INICIAL) {
        printf("Opere MOP=%d", per[perso].mop);
        scanf("%d", &opera);
        return opera;
    } else {
        printf("MOP=%d\n", per[perso].mop);
        return opera;
    }
}

int healaMopIn() {
    int teste;

    printf("[0]S [1]N\nHealar FMOP? ");
    scanf("%d", &teste);
    return teste;
}

void healaMopPrint(int local, int perso) {
    if (local == INICIAL) {
        printf("MOP=%d\n", per[perso].mop);
    } else {
        puts("Operacao cancelada");
    }
    return;
}

int healaFullMopsIn(int local) {
    int teste;

    if (local == INICIAL) {
        printf("[0]S [1]N\nHealar Full TODOS MOPs? ");
        scanf("%d", &teste);
    } else {
        printf("Certeza? ");
        scanf("%d", &teste);
    }
    return teste;
}

void healaFullMopsPrint(int local) {
    if (local == INICIAL) {
        puts("MOPs Healados");
    } else {
        puts("Operacao cancelada");
    }
    return;
}

void saveMopsIn() {
    puts("Os MOPs foram salvos");
    linha();
    return;
}

//------------------------------------- nen ------------------------------------

int useRealRyuIn() {
    int qtd;
    printf("[1] [2] [0]Sai\nQuantos? ");
    scanf("%d", &qtd);
    return qtd;
}

double reflexRenIn(int local) {
    double d100;

    if (local == INICIAL) {

        char linha[3];

        printf("Qnts chars? ");
        fflush(stdin);
        fgets(linha, 3, stdin);

        if (linha[0] == '\n') {
            tamPilha = 1;
        }
        else if (linha[0] == '0') {
            puts("Saindo da funcao reflexRen");
            return RETURN;
        } else {
            tamPilha = atoi(linha);
        }
        return TRUE;

    } else {

        printf("[-1]z D100? ");
        scanf("%lf", &d100);
        return d100;

    }
}

int gastaRenIn(int local, int mop) {
    char linha[3];

    if (local == INICIAL) {
        printf("[0]N [1]S\nUsou REN? ");
        fflush(stdin);
        fgets(linha, 3, stdin);

        if (linha[0] == '\n') {
            return 1;
        } else {
            return atoi(linha);
        }
    } else {
        printf("MOP Atual=%d\n", mop);
        return FALSE;
    }
}

int useAopIn(int local, int aop) {
    int temp;

    if (local == INICIAL) {
        printf("%d <-antigo novo-> ", aop);
        scanf("%d", &temp);
    } else if (local == FINAL) {
        printf("O AOP [0]fica ou [1]sai? ");
        scanf("%d", &temp);
    } else {
        puts("AOP original restaurado");
    }
    return temp;
}

void usaKenIn(int mop) {
    printf("MOP=%d ", mop);
    return;
}

void showKensIn(int ren) {
    int i;

    puts("Os valores de RYU sao:\n");
    for (i = 1; i < 10; i++) {
        printf("%03d = %.0lf\n", i * 5, ren * (i * 0.05));
    }
    printf("KEN = %.0lf\n", ren * 0.5);
    for (i = 11; i < 20; i++) {
        printf("%03d = %.0lf\n", i * 5, ren * (i * 0.05));
    }
    printf("KOU = %d\n", ren);
	linha();

    return;
}

int showKenIn() {
    int porcent;

    printf("Qual a %%?");
    scanf(" %d", &porcent);

    return porcent;
}

void showKenPrint(int porcent, int ren) {
    printf("%d%% = %d\n", porcent, ren);
	linha();
    return;
}

//----------------------------------- outros -----------------------------------

void resetPersIn() {
    puts("Personagens resetados");
    linha();
    return;
}

int editCritIn() {
    int crit;
    printf("Critico: x");
    scanf(" %d", &crit);
    return crit;
}

void skipCommandIn() {
    puts("Saindo do comando");
    linha();
    return;
}

int sairIn(int local) {
    int teste;

    if (local == INICIAL) {
        printf("JA SALVOU? Fechar o programa? [0]N [1]S: ");
        scanf("%d", &teste);
    } else {
        puts("Fechando o programa");
    }
    return teste;
}

int reWindIn(int local) {
    int qtd;

    if (local == INICIAL) {
        printf("Qtas vezes: ");
        scanf("%d", &qtd);
    } else {
        printf("Os HPs e MOPs foram retornados %d turno(s) atras\n", qtd);
        linha();
    }
    return qtd;
}

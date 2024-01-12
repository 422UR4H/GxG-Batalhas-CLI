#include "algoritmo.h"


//lista de variaveis globais
FILE *paBD, *paHP, *paBU, *paMOP, *paList;
PILHA *pilha;
SAVE *save;
PERSONAGEM *per;
char comando[TAM];
int tamPilha;
int atacante, defensor, tamSave;


//############################ FUNCOES DE ALGORITMO ############################


//######################### Funcoes -menos- utilizadas #########################

double divideVol(double aux) {
    return aux / 2;
}

int inicializaMat(int atacante, int atk) {
    //char linha[5];
    //int mat, i, j;

    /*
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
    */
    return per[atacante].ataques[atk].bonus;
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
		damage->dano = dano;
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

    printf("%lf, %d\n", gasto.gastouEms, gasto.mopAnterior);
    //atribuicao do mop atual para impressao
    gasto.mopAnterior = per[perso].mop;

    printf("%d\n", gasto.mopAnterior);
    //aplicacao do gasto (reducao de mop)
    per[perso].mop -= gasto.gastouEms;

    return gasto.mopAnterior;
}

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
/*
int leBD(char *str) {
    int i, j, qtd;
    char cTemp;
	char dir[TAM * 10];

	strcpy(dir, str);
	strcat(dir, "BDfichas.txt");
    if ((paBD = fopen(dir, "r")) == NULL) {
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
        cTemp = fgetc(paBD);
        //aloca memoria
        if ((per[i].ataques = (SKILLS*) calloc(qtd, sizeof (SKILLS))) == NULL) {
            return FALSE;
        }
        //o elemento 0 do vetor ataques é guardado para o atk fisico
        for (j = 1; j < qtd; j++) {
            pegaNomeSkill(i, j);
            cTemp = fgetc(paBD);
            fscanf(paBD, "%d", &per[i].ataques[j].dano);
            cTemp = fgetc(paBD);
        }
        //multiplica o MOP por 1000
        per[i].mopFull *= 1000;
    }
    fclose(paBD);

    return TRUE;
}
*/

int leBD(char *str) {
    FILE *pfb;
    FILE *psb;
    int i, j;
	char dir[300];

	getDiretorio(dir);
	strcat(dir, "BDfichas.bin");
    if ((pfb = fopen(dir, "rb")) == NULL) {
        return FALSE;
    }
    getDiretorio(dir);
    strcat(dir, "BDskills.bin");
    if ((psb = fopen(dir, "rb")) == NULL) {
        return FALSE;
    }

    // lendo todas as informacoes de todos as fichas
    i = 0;
    if ((per = (PERSONAGEM *) malloc(sizeof (PERSONAGEM))) == NULL) {
        puts("Erro: meoria nao alocada");
    }
    fread(&per[i++], sizeof (PERSONAGEM), 1, pfb);
    while (1) {
        if ((per = (PERSONAGEM *) realloc(per, (i + 1) * sizeof (PERSONAGEM))) == NULL) {
            printf("Erro: %d memoria nao alocada", i);
        }
        if (fread(&per[i++], sizeof (PERSONAGEM), 1, pfb) == 0) {
            if (feof(pfb)) {
                free(&per[--i]);
                break;
            }
        }
    }
    pushQtdFichas(i);

    for (i = 0; i < getQtdFichas(); i++) {
        per[i].ataques = (SKILLS *) calloc(per[i].qtdAtaques, sizeof (SKILLS));
        for (j = 0; j < per[i].qtdAtaques; j++) {
            fread(&per[i].ataques[j], sizeof (SKILLS), 1, psb);
        }
    }
    fclose(pfb);
    fclose(psb);
    return TRUE;
}

int leVol(char *str) {
    int i, j;
    char c;
    FILE *paSkills;
	char dir[TAM * 10];

	strcpy(dir, str);
	strcat(dir, "BDskills.txt");
    //lendo stream FILE
    if ( (paSkills = fopen(dir, "r")) == NULL) {
        return FALSE;
    }

    for (i = 0; i < getQtdFichas(); i++) {

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

int leHP(char *str) {
    int i;
	char dir[TAM * 10];

	strcpy(dir, str);
	strcat(dir, "BDHPs.bin");
	if ((paHP = fopen(dir, "rb")) == NULL) {
        return FALSE;
	}

	for (i = 0; i < getQtdFichas(); i++) {
        fread(&per[i].hp, sizeof (int), 1, paHP);
	}
	fclose(paHP);
	/*
	strcpy(dir, str);
	strcat(dir, "BDHPs.txt");
    if ((paHP = fopen(dir, "r")) == NULL) {
        return FALSE;
    }

    for (i = 0; i < getQtdFichas(); i++) {
        limpaChar(paHP);
		fscanf(paHP, "%d", &per[i].hp);
    }

    fclose(paHP);
    */
    return TRUE;
}

int leMOP(char *str) {
    int i;
	char dir[TAM * 10];

	strcpy(dir, str);
	strcat(dir, "BDMOPs.bin");
	if ((paMOP = fopen(dir, "rb")) == NULL) {
        return FALSE;
	}

	for (i = 0; i < getQtdFichas(); i++) {
        fread(&per[i].mop, sizeof (int), 1, paMOP);
	}
	fclose(paMOP);
	/*
	strcpy(dir, str);
	strcat(dir, "BDMOPs.txt");
    if ((paMOP = fopen(dir, "r")) == NULL) {
        return FALSE;
    }

    for (i = 0; i < getQtdFichas(); i++) {
	limpaChar(paMOP);
        fscanf(paMOP, "%d", &per[i].mop);
    }
    fclose(paMOP);
    */
    return TRUE;
}

void leList(char *str, List **list) {
    char dir[TAM * 10];
    int i;

    strcpy(dir, str);
    strcat(dir, "lista.bin");
    if ((paList = fopen(dir, "rb")) == NULL) {
        pushQtdLists(0);
        return;
    }
    i = 0;
    if ((*list = (List *) malloc(sizeof (List))) == NULL) {
        puts("Erro: meoria nao alocada");
    }
    fread(*list, sizeof (List), 1, paList);
    i++;
    if (feof(paList)) {
        i--;
        free(*list);
        *list = NULL;
        pushQtdLists(0);
        return;
    }
    while (1) {
        if ((*list = (List *) realloc(*list, (i + 1) * sizeof (List))) == NULL) {
            printf("Erro: %d memoria nao alocada", i);
        }
        if (fread((*list + i), sizeof (List), 1, paList) == 0) {
            if (feof(paList)) {
                free((*list + i));
                break;
            }
        }
        i++;
    }
    fclose(paList);
    pushQtdLists(i);
    return;
}

//func. que salva hp e mop no final de cada turno em um banco de dados de backup
void backuphps(char *str) {
    int i;
	char dir[TAM * 10];

	strcpy(dir, str);
	strcat(dir, "BDbackup.txt");
    //alocando memoria
    paBU = fopen(dir, "w");

    for (i = 0; i < getQtdFichas(); i++) {//passar hps pra int
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

void operaExtAl(int perso, int opera) {
    per[perso].corpo += opera;
    return;
}

void zeraExtAl(int perso) {
    per[perso].corpo = 100;
    return;
}

void zeraExtsAl(int i) {
    per[i].corpo = 100;
    return;
}

//------------------------------ shu ------------------------------

void zeraShusAl(int i) {
    per[i].shu = 0;
    return;
}

void addShuAl(int atacante, int qtd) {
    per[atacante].shu = qtd;
    return;
}

void rodaShuAl(int i, int temp) {
    per[i].mop -= per[i].shu * temp;
    return;
}

void turnosAl(int temp, int qtd, int i) {
    per[i].mop -= per[i].shu * temp * qtd;
}

//-------------------------------- atk e damage --------------------------------

void nerfAtkAl(int perso, int ataque, int reduz, PILHA *pilha) {
	pilha[0].num = perso;
	pilha[0].valInt = ataque;
	pilha[0].valDouble = per[perso].ataques[ataque].dano;
	per[perso].ataques[ataque].dano += reduz; //reduz está negativo
    return;
}

void nerfAtkAl2(PILHA *pilha) {
	per[pilha[0].num].ataques[pilha[0].valInt].dano = pilha[0].valDouble;
	return;
}

void perfDamageAl(int local, int defensor, PILHA *pilha) {
    double temp;

    if (local == INICIAL) {

        pilha[0].num = defensor;
        pilha[0].valInt = per[defensor].aop; //atk segura o aop de defesa
        pilha[0].valDouble = per[defensor].ten;
        temp = 1.0 * per[defensor].aop / 10;
        per[defensor].aop = floor(temp);
        per[defensor].ten = 0;
        //per[defensor].aop-=(per[defensor].ten/2);

    } else {

        per[pilha[0].num].aop = pilha[0].valInt;
        per[pilha[0].num].ten = (int) pilha[0].valDouble;
        free(pilha);

    }
    return;
}

//------------------------------------- hp -------------------------------------

void editHpAl(int perso, int hp) {
    per[perso].hp = hp;
    return;
}

void operaHpAl(int opera, int perso) {
    per[perso].hp += opera;
    return;
}

void healaHpAl(int perso) {
    per[perso].hp = per[perso].hpFull;
    return;
}

//------------------------------------ mop -------------------------------------

void editMopAl(int perso, int mop) {
    per[perso].mop = mop;
    return;
}

void operaMopAl(int opera, int perso) {
    per[perso].mop += opera;
    return;
}

void healaMopAl(int perso) {
    per[perso].mop = per[perso].mopFull;
    return;
}

//------------------------------------- nen ------------------------------------

void useRealRyuAl(int i, int perso, PILHA *pilha) {
    //passando valores (referencial e real e corpo) à struct pilha
    pilha[i].num = perso;
    pilha[i].valInt = per[perso].corpo;
    //alterando corpo, o valor real de ryu seja utilizado
    per[perso].corpo = 100;
    return;
}

void reflexRenAl(int local, int d100, int *num) {
    int temp;

    if (local == INICIAL) {

        d100 /= 100;
        temp = d100 * per[*num].aop; //temp recebe aop
        *num = per[*num].aop;
        per[*num].aop = floor(temp);

    } else {
        //d100 tem o valor de i;
        per[*num].aop = pilha[d100].valDouble;
    }
    return;
}

void gastaRenAl() {
    per[atacante].mop -= (per[atacante].aop * 50);
    return;
}

void useAopAl(int atacante, PILHA *pilha, int temp) {
    pilha[0].num = atacante;
    pilha[0].valInt = per[atacante].aop;
    per[atacante].aop = temp;
    return;
}

void useKenAl(int aop) {
    per[atacante].mop -= aop * 50;
    return;
}

double showKensAl(int atacante, int porcent) {
    double ren;

    ren = per[atacante].aop * (1 + 1.0 * per[atacante].pop / 10) *
            per[atacante].modClas;

	//mesma coisa que divisao por 100
	ren *= 0.01 * porcent;
    return floor(ren);
}

//----------------------------------- outros -----------------------------------

void reWindAl(int tamSave) {
    per[save[tamSave].atacante].mop = save[tamSave].mop;
    per[save[tamSave].defensor].hp = save[tamSave].hp;
    return;
}

#include "estruturas.h"

// lista de variaveis globais
PERSONAGEM *per;
int qtdFichas;

double leInt(char c);


int leBD() {
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
    qtdFichas = i;

    for (i = 0; i < qtdFichas; i++) {
        per[i].ataques = (SKILLS *) calloc(per[i].qtdAtaques, sizeof (SKILLS));
        for (j = 0; j < per[i].qtdAtaques; j++) {
            fread(&per[i].ataques[j], sizeof (SKILLS), 1, psb);
        }
    }
    fclose(pfb);
    fclose(psb);
    return TRUE;
}

// entra com um numero ou nome de personagem e retorna o numero desse personagem
int leNome() {
    char nome[TAM]; // salva ou o nome ou o numero do personagem
    int personagem; // recebe o numero do personagem

    do {
        printf("Nome: ");
        fgetspc(TAM, nome);
        strupr(nome);

        if (nome[0] == '-') {
            return RETURN; // -1
        }

        // se a entrada for um numero valido, a resposta será menor que 115
        // e esse valor é passado para personagem
        if (conta(nome) < 115) {
            personagem = atoi(nome);
            if (nome[0] == '\0' || personagem < 0) {
                continue;
            }
        } else {
            return valorChar(nome);
        }
    // ATENCAO: a funcao testaComando pode retornar valores < -2 !!!
    } while (personagem == -1);

    return personagem;
}

void corrigeNome(char *str) {
    int i, j;

    // le toda a string no endereco do ponteiro ate o null,
    // verificando se ha caracteres diferentes de A a Z
    // se houver, passa o valor de um endereco adiante
    // (proximo caractere) ao avaliado, e continua ate o NULL
    // continua avaliando, se houver outros,
    // o mesmo procedimento acima ocorre, se nao, encerra no NULL
    for (i = 0; *(str + i) != '\0'; i++) {
        if (*(str + i) < 65 || *(str + i) > 90) {
            for (j = i; *(str + j) != '\0'; j++) {
                *(str + j) = *(str + j + 1);
            }
        }
    }
    return;
}

int valorChar(char *str) {
    int i;

    corrigeNome(str);
    // comp. o nome no endereco apontado por str, com todos os nomes de persons
    for (i = 0; strcmp(str, per[i].nome) != 0 && i < qtdFichas; i++);
    if (i >= qtdFichas) {
        puts("Nome invalido!");
        return -1;
    }
    return i;
}

int conta(char *str) {
    int i = 0, valor = 0;

    while (*(str + i) != '\0') {
        valor += *(str + i);
        i++;
    }
    return valor;
}

//funcao como a fgets, mas essa tira o <enter> automaticamente
void fgetspc(int tam, char *str) {
    /*criado por 422UR4*/
    fflush(stdin);
    fgets(str, tam, stdin);
    str[strlen(str) - 1] = '\0';
    return;
}

// retorna RETURN se quiser voltar ao menu principal
int editPer() {
    int i, opcao;

    for (i = 0; i < qtdFichas; i++) {
        printf("[%d]%s\n", i, per[i].nome);
    }
    printf("[-1]VOLTAR\n");
    opcao = leNome();

    if (opcao >= 0 && opcao < qtdFichas) {
        printf("\n");
        return editaAtributo(opcao);
    }
    puts("Voltando...\n");
    return TRUE;
}

int leAtributo(int i) {
	int opcao;
	char linha[4];

	puts("Qual atributo deseja editar?");
	printf("[1] - %d \tHP\n" //hpFull
		   "[2] - %d \tMOP\n" //mopFull
		   "[3] - %d \tAOP\n"
		   "[4] - %d \tPOP\n"
		   "[5] - %d \tCDN\n"
		   "[6] - %d \tTEN\n"
		   "[7] - %d \tmodFor\n"
		   "[8] - %d \tvel\n"
		   "[9] - %d \tcorpo\n"
		   "[10] - %d \tataques\n"
		   "[11] - %.01lf \tmodificador de classe\n"
		   "[12] - mudar nome do personagem\n"
		   "[-1] - voltar\n",
		   per[i].hpFull,
		   per[i].mopFull,
		   per[i].aop,
		   per[i].pop,
		   per[i].cdn,
		   per[i].ten,
		   per[i].modFor,
		   per[i].vel,
		   per[i].corpo,
		   per[i].qtdAtaques,
		   per[i].modClas);
		   printf("Opcao: ");

	fgetspc(4, linha);
	printf("\n");
	opcao = atoi(linha);
	if (opcao == 0) {
		puts("Opcao invalida!\n");
		return leAtributo(i);
	}
	return opcao;
}

int leValor(int valor, char *str) {
	char linha[10];
	int novoValor;

	printf("Valor atual de %s: %d\n", str, valor);
	printf("Novo valor de %s: ", str);
	fgetspc(10, linha);
	printf("\n");
	novoValor = atoi(linha);

	//se novoValor valer 0, pode ser que o valor lido
	//seja incorreto, então há esse teste para confirmar
	if (novoValor == 0) {
		puts("[0]N [1]S");
		puts("O valor entrado foi 0?");
		fgetspc(3, linha);
		if (linha[0] != '1') {
			puts("Entre com um novo valor!");
			return leValor(valor, str);
		}
	}
	return novoValor;
}

// retorna FALSE se ocorrer um erro no save
int editaAtributo(int i) {
	int opcao, valor;

	opcao = leAtributo(i);
	switch (opcao) {
		case 1:
			valor = leValor(per[i].hpFull, "HP");
			per[i].hpFull = valor;
			break;
		case 2:
			valor = leValor(per[i].mopFull, "MOP");
			per[i].mopFull = valor;
			break;
		case 3:
			valor = leValor(per[i].aop, "AOP");
			per[i].aop = valor;
			break;
		case 4:
			valor = leValor(per[i].pop, "POP");
			per[i].pop = valor;
			break;
		case 5:
			valor = leValor(per[i].cdn, "CDN");
			per[i].cdn = valor;
			break;
		case 6:
			valor = leValor(per[i].ten, "TEN");
			per[i].ten = valor;
			break;
		case 7:
			valor = leValor(per[i].modFor, "mod. FOR");
			per[i].modFor = valor;
			break;
		case 8:
			valor = leValor(per[i].vel, "Vel");
			per[i].vel = valor;
			break;
		case 9:
			valor = leValor(per[i].corpo, "corpo");
			per[i].corpo = valor;
			break;
		case 10:
			per[i].qtdAtaques = editaAtk(per[i].qtdAtaques, &per[i].ataques);
			break;
		case 11:
		    printf("%.2lf, %d\n", per[i].modClas, (int) per[i].modClas * 10);
			valor = leValor((int) 10 * per[i].modClas, "mod. Clas * 10");
			per[i].modClas = 0.1 * valor;
			break;
        case 12:
            printf("Nome: ");
            fgetspc(TAM, per[i].nome);
            strupr(per[i].nome);
            break;
		case -1:
		    puts("Voltando...\n");
			return TRUE;
		default:
			puts("Opcao invalida!\n");
			return editaAtributo(i);
	}
	if (salvaBD() == FALSE) return FALSE;
	return editaAtributo(i);
}

int salvaBD() {
    FILE *pfb;
    FILE *psb;
    char cbin[200];
    char dir[200];
    int j, i;

    getDiretorio(dir);
    strcpy(cbin, dir);
    strcat(cbin, "BDfichas.bin");
    pfb = fopen(cbin, "wb");
    for (i = 0; i < qtdFichas; i++) {
        fwrite(&per[i], sizeof (PERSONAGEM), 1, pfb);
    }
    strcpy(cbin, dir);
    strcat(cbin, "BDskills.bin");
    psb = fopen(cbin, "wb");

    for (i = 0; i < qtdFichas; i++) {
        for (j = 0; j < per[i].qtdAtaques; j++) {
            fwrite(&per[i].ataques[j], sizeof (SKILLS), 1, psb);
        }
    }
    fclose(pfb);
    fclose(psb);
    return TRUE;
}

int salvaHP() {
    FILE *hpb;
    char dir[200];
    int i;

    getDiretorio(dir);
    strcat(dir, "BDHPs.bin");
    hpb = fopen(dir, "wb");
    for (i = 0; i < qtdFichas; i++) {
        fwrite(&per[i].hp, sizeof (int), 1, hpb);
    }
    fclose(hpb);
    return TRUE;
}

int salvaMOP() {
    FILE *mpb;
    char dir[200];
    int i;

    getDiretorio(dir);
    strcat(dir, "BDMOPs.bin");
    mpb = fopen(dir, "wb");
    for (i = 0; i < qtdFichas; i++) {
        fwrite(&per[i].mop, sizeof (int), 1, mpb);
    }
    fclose(mpb);
    return TRUE;
}

void imprimeAtk(int i, SKILLS *perAtaque) {

    printf("[%d]%s\n", i + 1, perAtaque[i].nome);
    printf("Dano: %d\n", perAtaque[i].dano);
    printf("Gasto: %d\n", perAtaque[i].vol);
    printf("Mat/Man: %d\n", perAtaque[i].bonus);
    printf("Emissao: %d\n", perAtaque[i].ems);
    printf("Restricao: %.1lf\n", perAtaque[i].restricao);
    printf("Multiplo: ");
    if (perAtaque[i].multiplo == TRUE) {
        printf("Sim\n");
    } else {
        printf("Nao\n");
    }
    return;
}

int editaAtk(int qtdAtk, SKILLS **perAtaque) {
	int opcao, temp, i;
	char linha[3];

	//imprime qtdAtk
	printf("qtd ataques: %d\n", qtdAtk);
	//imprime todos os ataques de um a um
	for (i = 0; i < qtdAtk; i++) {
        imprimeAtk(i, *perAtaque);
	}
	puts("------------------------------");
	/*printf("[1]Adc nova skill\n"
           "[2]Deletar skill\n"
           "[3]Editar skill\n"
           "[-1]Voltar\n"
           "Opcao: ");
	fgetspc(4, linha);
	opcao = atoi(linha);*/
	opcao = 3;

	switch (opcao) {
	    case -1:
            puts("Saindo da funcao...\n");
            return qtdAtk;
        case 1:
            // alocando memoria para um novo atk
            *perAtaque = (SKILLS *) realloc(*perAtaque, ++qtdAtk * sizeof (SKILLS));
            // le os dados do novo ataque
            // qtdAtk -1 para acessar o novo elemento vazio que será editado
            leDados(opcao, qtdAtk -1, qtdAtk, *perAtaque);
            break;
        case 2:
            do {
                // le o ataque a ser deletado
                printf("Qual? ");
                scanf("%d", &temp);
                // para evitar a entrada do valor 0 pelo
                // usuario, a impressao das habilidades
                // comeca do 1, logo a temp pode ir do
                // 1 até qtdAtk. logo qtdAtk será decrescido
                if (temp == 0 || temp > qtdAtk) {
                    puts("Escolha Invalida!\n");
                    continue;
                }
                temp--;
                qtdAtk--;
                printf("%d\n", qtdAtk);
            } while (FALSE); //nunca repete

            // deletando a habilidade
            while (temp < qtdAtk) {
                (*(*perAtaque + temp)) = (*(*perAtaque + temp + 1));
                temp++;
            }
            realloc(perAtaque[temp], qtdAtk * sizeof (SKILLS));
            break;
        case 3:
            do {
                // le o ataque a ser editado
                printf("Qual? ");
                fgetspc(4, linha);
                temp = atoi(linha);
                // para evitar a entrada do valor 0 pelo
                // usuario, a impressao das habilidades
                // comeca do 1, logo a temp pode ir do
                // 1 até qtdAtk. logo qtdAtk será decrescido
                if (temp > qtdAtk || temp == 0) {
                    puts("Escolha Invalida!\n");
                    continue;
                }
                if (temp < 0) {
                    puts("Voltando...\n");
                    return qtdAtk;
                }
                temp--;
            } while (FALSE); // nunca repete
            // imprime os dados do ataque escolhido
            imprimeAtk(temp, *perAtaque);
            // le os dados do ataque
            leDados(opcao, temp, qtdAtk, *perAtaque);
            break;
        default:
            puts("Opcao invalida!\n");
            return editaAtk(qtdAtk, perAtaque);
	}
    //imprime os ataques do personagem
    for (i = 0; i < qtdAtk; i++) {
        imprimeAtk(i, *perAtaque);
	}
    printf("\n");
    return qtdAtk;
}

// p é o parametro utilizado para saber qual foi a opcao
// escolhida. Se for edicao de ataque, o usuario
// escolhe os atributos a editar.
// se for criacao de ataque, todas as opcoes rodam
// e a funcao retorna TRUE automaticamente no final
void leDados(int p, int i, int qtdAtk, SKILLS *ataque) {
	int temp;
    int opcao;
    char linha[TAM];

    if (p == 3) {
        puts("Editar:");
        printf("[1]Nome \t%s\n"
               "[2]Dano \t%d\n"
               "[3]Gasto\t%d\n"
               "[4]Man/Mat\t%d\n"
               "[5]Emissao\t%d\n"
               "[6]Multiplo\t%d\n"
               "[7]Restricao\t%.1lf\n"
               "[-1]Voltar\n"
               "Opcao: ",
               ataque[i].nome,
               ataque[i].dano,
               ataque[i].vol,
               ataque[i].bonus,
               ataque[i].ems,
               ataque[i].multiplo,
               ataque[i].restricao);
        opcao = (int) leInt('i');
    } else {
        opcao = 1;
    }

    switch (opcao) {
        case -1:
            puts("Voltando...\n");
            return;
        case 1:
            printf("Nome: ");
            fgetspc(TAM, linha);
            if (linha[0] == '-') {
                break;
            }
            strupr(linha);
            strcpy(ataque[i].nome, linha);
            if (p == 3) break;
        case 2:
            printf("Dano: ");
            temp = (int) leInt('i');
            ataque[i].dano = temp;
            if (p == 3) break;
        case 3:
            printf("Gasto: ");
            scanf("%d", &temp);
            ataque[i].vol = temp;
            if (p == 3) break;
        case 4:
            printf("Man/Mat: ");
            scanf("%d", &temp);
            ataque[i].bonus = temp;
            if (p == 3) break;
        case 5:
            printf("[1]S [0]N\nEmissao? ");
            scanf("%d", &temp);
            if (temp == 1) {
                ataque[i].ems = TRUE;
            } else {
                ataque[i].ems = FALSE;
            }
            if (p == 3) break;
        case 6:
            printf("[1]S [0]N\nMultiplo? ");
            scanf("%d", &temp);
            if (temp == 1) {
                ataque[i].multiplo = TRUE;
            } else {
                ataque[i].multiplo = FALSE;
            }
            if (p == 3) break;
        case 7:
            printf("Restricao: ");
            scanf("%lf", &ataque[i].restricao);
            break;
        default:
            puts("Valor Invalido!\n");
    }
    printf("\n");
    if (p == 3) {
        // chamada recursiva
        leDados(p, i, qtdAtk, ataque);
    } else {
        return;
    }
}
/*
int leValorAtk(char *str) {
	char linha[5];
	int temp;

	printf("%s: ", str);
	fgetspc(5, linha);
	temp = atoi(linha);
	// testando o valor entrado para temp
	if (temp == 0) {
		puts("[0]N [1]S");
		puts("O valor entrado foi 0?");
		fgetspc(3, linha);
		// se for invalido, há uma chamada recursiva
		if (linha[0] != '1') {
			puts("Entre com um novo valor!");
			return leValorAtk(str);
		}
	}
	return temp;
}
*/

double leInt(char c) {
	double d = 1;
	char s[7];
	char temp[3];

	fgetspc(7, s);

	if (c == 'd') {
		d = atof(s);
	} else {
		d = atoi(s);
	}

	if (d == 0) {
		printf("[0]N [1]S\nVoce digitou 0? ");
		fgetspc(3, temp);
		if (temp[0] != '1') {
            printf("Valor Correto: ");
			return leInt(c);
		}
	}
	if (c == 'd') {
		return d;
	} else {
		return (int) d;
	}
}

void listaFicha(PERSONAGEM ficha) {
    int i;

	printf("nome: ");
	puts(ficha.nome);
	printf("hpFull: %d\n", ficha.hpFull);
	printf("mopFull: %d\n", ficha.mopFull);
	printf("aop: %d\n", ficha.aop);
	printf("pop: %d\n", ficha.pop);
	printf("cdn: %d\n", ficha.cdn);
	printf("ten: %d\n", ficha.ten);
	printf("modClas: %.1lf\n", ficha.modClas);
	printf("modFor: %d\n", ficha.modFor);
	printf("vel: %d\n", ficha.vel);
	printf("corpo: %d\n", ficha.corpo);
	//funcao que imprime os ataques
	for (i = 0; i < ficha.qtdAtaques; i++) {
        imprimeAtk(i, ficha.ataques);
	}
	printf("\n");
	return;
}

void listaPersonagens() {
    int i;

    for (i = 0; i < qtdFichas; i++) {
        listaFicha(per[i]);
    }
    printf("\n");
    return;
}

double leModClas() {
    double i = leInt('d');
    if (i > 10) {
        return i / 100;
    } else if (i > 1) {
        return i /= 10;
    } else {
        return i;
    }
}

int criaChar() {
	int i, j;
	char ficha[] = "BDfichas.bin";
	char skill[] = "BDskills.bin";
	char dir[300];
	char c;
	FILE *pf, *ps;
	PERSONAGEM person;

	// opcao para voltar operacao
	printf("[1] Para cancelar\n"
           "[0] Para prosseguir\n"
           "Opcao: ");
	fflush(stdin);
	c = getchar();
	printf("\n");
	if (c == '1') {
        puts("Voltando...\n");
        return TRUE;
	}

	// lendo valores do console
	puts("Entre com os dados do personagem:");
	printf("Nome: ");
	fgetspc(TAM, person.nome);
	strupr(person.nome);
	printf("hpFull: ");
	person.hpFull = leInt('i');
	printf("mopFull: ");
	person.mopFull = leInt('i');
	printf("aop: ");
	person.aop = leInt('i');
	printf("pop: ");
	person.pop = leInt('i');
	printf("cdn: ");
	person.cdn = leInt('i');
	printf("ten: ");
	person.ten = leInt('i');
	printf("modClas: ");
	person.modClas = leModClas();
	printf("modFor: ");
	person.modFor = leInt('i');
	printf("vel: ");
	person.vel = leInt('i');
	printf("corpo: ");
	person.corpo = leInt('i');
	printf("qtdAtk: ");
	person.qtdAtaques = leInt('i');

	// copiando valores padrao de hp e mop cheios
	person.hp = person.hpFull;
	person.mop = person.mopFull;

	qtdFichas++;
	if ((per = (PERSONAGEM *) realloc(per, qtdFichas * sizeof (PERSONAGEM))) == NULL) {
        return FALSE;
	}
	j = qtdFichas - 1;

    per[j] = person;

	// alocando memoria para os ataques
	per[j].ataques = (SKILLS *) calloc(person.qtdAtaques, sizeof (SKILLS));
	// lendo valores dos ataques
	for (i = 0; i < person.qtdAtaques; i++) {
		printf("Habilidade %d:\n", i + 1);
		printf("Nome: ");
		fgetspc(TAM, per[j].ataques[i].nome);
		strupr(per[j].ataques[i].nome);
		printf("Dano: ");
		per[j].ataques[i].dano = leInt('i');
		printf("Gasto: ");
		per[j].ataques[i].vol = leInt('i');
	}
	printf("\n");

	// salvando dados


	if (salvaBD() == 0) {
        return FALSE;
	}
	if (salvaHP() == 0) {
        return FALSE;
	}
	if (salvaMOP() == 0) {
        return FALSE;
	}
/*
	// definindo diretorio
	getDiretorio(dir);
	strcat(dir, ficha);

	// abrindo arquivo BDfichas.bin para insercao de dados
	if ( (pf = fopen(dir, "wb")) == NULL) {
		return FALSE;
	}

	// definindo proximo diretorio
	getDiretorio(dir);
	strcat(dir, skill);

	// abrindo arquivo BDskills.bin para insercao de dados
	if ( (ps = fopen(dir, "wb")) == NULL) {
        return FALSE;
	}

	// gravando dados binarios
	for (i = 0; i < qtdFichas; i++) {
        // salvando ficha
        fwrite(&per[i], sizeof (PERSONAGEM), 1, pf);
        // salvando skills
        for (j = 0; j < per[i].qtdAtaques; j++) {
            fwrite(&per[i], sizeof (SKILLS), 1, ps);
        }
	}
	// salvando nova ficha
	fwrite(&person, sizeof (PERSONAGEM), 1, pf);
	// salvando novas skills
	for (j = 0; j < per[i].qtdAtaques; j++) {
        fwrite(person.ataques, sizeof (SKILLS), 1, ps);
	}

	// aumentando quantidade (limite) de fichas (desnecessario)
	qtdFichas++;

	// fechando arquivos para liberar memoria
	fclose(pf);
	fclose(ps);
*/
    // carregando novamente os valores (desnecessario)
    if (leBD() == 0) {
        return FALSE;
    }

	return TRUE;
}

int deletaChar() {
    int i, j;
    FILE *pf, *ps;
    char dir;
    char ficha[] = "BDfichas.bin";
    char skill[] = "BDskills.bin";
    char hp[] = "BDHPs.bin";
    char mop[] = "BDMOPs.bin";


    // LENDO PERSONAGEM A SER DELETADO

    // impressao da lista de personagens
    for (i = 0; i < qtdFichas; i++) {
        printf("[%d]%s\n", i, per[i].nome);
    }
    printf("[-1]VOLTAR\n");
    // lendo personagem
    i = leNome();
    if (i < 0 && i >= qtdFichas) {
        puts("Voltando...\n");
        return TRUE;
    }


    // DELETANDO PERSONAGEM

    qtdFichas--;
    // liberando memoria dos ataques
    free(per[i].ataques);
    // fazendo a sobreposicao de estruturas
    while (i < qtdFichas) {
        per[i] = per[++i];
    }
    // liberando memoria do personagem
    free(&per[i]);

    // SALVANDO DADOS

    if (salvaBD() == 0) {
        return FALSE;
	}
	if (salvaHP() == 0) {
        return FALSE;
	}
	if (salvaMOP() == 0) {
        return FALSE;
	}
	puts("Personagem Deletado\n");
    return TRUE;
}

void fazerBackup() {
    system("start backup.bat");
    puts("Backup feito\n");
}

void ordena(int inicio, int fim) {
    int i;
    int trocou;
    PERSONAGEM temp;

    do {
        trocou = FALSE;
        for (i = inicio; i < fim - 1; i++) {
            if (strcmp(per[i].nome, per[i + 1].nome) > 0) {
                temp = per[i];
                per[i] = per[i + 1];
                per[i + 1] = temp;

                trocou = TRUE;
            }
        }
    } while (trocou);

    return;
}

int ordenaChar() {
    int qtdJogador, i, j;
    int *jogador;
    PERSONAGEM temp;


    puts("[-1]Voltar");
    printf("Quantos personagens de jogadores? ");

    // sai da funcao se o valor lido for -1
    if ((qtdJogador = leInt('i') ) == -1) {
        puts("voltando\n");
        return;
    }

    jogador = (int *) calloc(qtdJogador, sizeof (int));

    // le quais sao os jogadores
    puts("Informe quais sao seus jogadores");
    for (i = 0; i < qtdJogador; i++) {
        printf("Jogador %d:\n", i + 1);
        jogador[i] = leNome();
    }

    // passando os jogadores para os primeiros pers
    for (i = 0; i < qtdJogador; i++) {
        j = jogador[i];

        temp = per[j];
        per[j] = per[i];
        per[i] = temp;
    }

    ordena(0, qtdJogador - 1);
    ordena(qtdJogador, getQtdFichas());

    printf("\n");
    if (salvaBD() == FALSE) return FALSE;
    return TRUE;
}

int menuPrincipal() {
	int opcao;
	char linha[3];

	// CONFIGURAR COM A MAIN
	printf("[1]Editar personagem\n"
		   "[2]Deleta personagem\n"
		   "[3]Criar personagem\n"
		   "[4]Listar personagens\n"
		   "[5]Fazer auto Backup\n"
		   "[6]Ordenar personagens\n"
		   "[7]Sair do programa\n"
		   "Opcao: ");

	fgetspc(3, linha);
	opcao = atoi(linha);
	if (opcao == 0) {
		puts("Entrada invalida!\n");
		return menuPrincipal();
	}
	printf("\n");
	return opcao;
}

int main() {
	int opcao;

	system("color b");
	system("title GxG - SQL");
	system("cls");

	if (leBD() == FALSE) {
		puts("Nao foi carregado fluxo ao ponteiro FILE");
		system("pause>>null");
		return 1;
	}

	do {
		//menu principal
		opcao = menuPrincipal();

		switch(opcao) {
			case 1:
			    if (editPer() == FALSE) {
                    puts("Erro no save!\n");
			    }
				break;
            case 2:
                if (deletaChar() == FALSE) {
                    puts("Erro ao deletar!\n");
                }
                break;
			case 3:
                if (criaChar() == FALSE) {
                    puts("Erro!\n");
                }
                break;
            case 4:
                listaPersonagens();
                break;
            case 5:
                fazerBackup();
                break;
			case 6:
                if (ordenaChar() == FALSE) {
                    puts("Erro!\n");
                }
                break;
			case 7:
                puts("Saindo...");
                return 0;
			default:
				puts("Opcao invalida!\n");
				break;
		}
	} while (1);
}

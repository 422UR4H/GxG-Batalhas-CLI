#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#define FALSE 0
#define TRUE 1
#define SUCESS 0
#define FAIL 1
#define TAM 40
#define DIM 46

//######## DECLARACAO DAS STRUCTS #########

typedef struct ATK{
	char nome[TAM];
	int dano;
	int gasto;
}ATK;

typedef struct PER{
	char nome[TAM];
	int hpFull, mopFull;
	int aop, pop, ten;
	int cdn, modFor, vel, corpo;
	int qtdAtk;
	double modClas;
	ATK *ataque;
}PER;

//######## DECLARACAO DOS PROROTIPOS #########

int leNome(PER*);
int valorChar(char*, PER*);
int leOpcao(void);
void fgetspc(int, char*);
int leBD(char*, PER*);
int leVol(char*, PER*);
int leValor(int, PER, char*);
void listaFicha(PER);
void imprimeAtks(int, ATK*);
int leValorAtk(char*);
void leDados(int, ATK*);
int leAtk(int, ATK*);
int salvaAlteracoes(char*, PER*);
int conta(char*);
void limpaChar(FILE*);
void corrigeNome(char*);
void pegaNomeSkill(char *, FILE*);

//########### CODIGO DAS FUNCOES ############

//pega o nome das habilidades do banco de dados e salva-los na struct per
void pegaNomeSkill(char *nome, FILE *paBD) {
    char c;
    int i;
	
    //le o primeiro caractere do nome
    c = fgetc(paBD);
    for (i = 0; c != ':'; i++) {
        nome[i] = c;
        c = fgetc(paBD);
    }
}

//entra com um numero ou nome de personagem e retorna o numero desse personagem
int leNome(PER *per) {
    char nome[TAM]; //salva ou o nome ou o numero do personagem
    int personagem; //recebe o numero do personagem
    
    do {
        printf("Nome: ");
        fgetspc(TAM, nome);
        strupr(nome);
    
        //se a entrada for um numero valido, a resposta será menor que 115
        //e esse valor é passado para personagem
        if (conta(nome) < 115) {
        
            personagem = atoi(nome);
            if (nome[0] == '\0' || personagem < 0) {
                continue;
            }
			
        } else {
            
            return valorChar(nome, per);
            
        }
    //ATENCAO: a funcao testaComando pode retornar valores < -2 !!!
    } while (personagem == -1);
    
    return personagem;
}

int valorChar(char *str, PER *per) {
    int i;

    corrigeNome(str);
    //comp. o nome no endereco apontado por str, com todos os nomes de persons
    for (i = 0; strcmp(str, per[i].nome) != 0 && i < DIM; i++);
    if (i >= DIM) {
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


int leBD(char *str, PER *per) {
    int i, j, qtd;
    char c;
	char dir[TAM * 2];
	FILE *paBD;
	
	strcpy(dir, str);
	strcat(dir, "BDfichas.txt");
	
    if ((paBD = fopen(dir, "r")) == NULL) {
        return FAIL;
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
        fscanf(paBD, "%d", &per[i].qtdAtk);
        //le o caractere <enter> ( \n )
        c = fgetc(paBD);
        //aloca memoria
		qtd = per[i].qtdAtk;
        if ( (per[i].ataque = (ATK*) calloc(qtd, sizeof (ATK))) == NULL) {
            return FAIL;
        }
        //o elemento 0 do vetor ataques é guardado para o atk fisico
        for (j = 0; j < qtd; j++) {
            pegaNomeSkill(per[i].ataque[j].nome, paBD);
            c = fgetc(paBD);
            fscanf(paBD, "%d", &per[i].ataque[j].dano);
            c = fgetc(paBD);
        }
    }
    fclose(paBD);
    
    return SUCESS;
}

int leVol(char *str, PER *per) {
    int i, j;
    char c;
    FILE *paSkills;
	char dir[TAM * 3];
	
	strcpy(dir, str);
	strcat(dir, "BDskills.txt");
    //lendo stream FILE
    if ( (paSkills = fopen(dir, "r")) == NULL) {
        return FAIL;
    }
	
    for (i = 0; i < DIM; i++) {
		
		//limpa o nome do personagem
		while (c != ':') {
            c = fgetc(paSkills);
        }
        c = fgetc(paSkills);
		
        for (j = 0; j < per[i].qtdAtk; j++) {
            //limpa o nome das skills
            while (c != ':') {
                c = fgetc(paSkills);
            }
            //limpa o caractere <espaco> (' ')
            c = fgetc(paSkills);
            fscanf(paSkills, "%d", &per[i].ataque[j].gasto);
		}
    }
	fclose(paSkills);
    return SUCESS;
}

//funcao usada para limpar caracteres "lixo" do banco de dados
void limpaChar(FILE *pa) {
    char c;
    
    do {
        c = fgetc(pa);
    } while (c != ' ');
    
    return;
}

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

int leValor(int valor, PER ficha, char *str) {
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
			return leValor(valor, ficha, str);
		}
	}
	return novoValor;
}

void listaFicha(PER ficha) {
	int i;
	
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
	imprimeAtks(ficha.qtdAtk, ficha.ataque);
	printf("\n");
	return;
}

void imprimeAtks(int qtdAtk, ATK *perAtaque) {
	int i;
	
	//imprime qtdAtk
	printf("qtd ataques: %d\n", qtdAtk);
	//imprime todos os ataques de um a um
	for (i = 0; i < qtdAtk; i++) {
		printf("[%d]%s: %d\n", i + 1, perAtaque[i].nome, perAtaque[i].dano);
		printf("Gasto: %d\n", perAtaque[i].gasto);
	}
	return;
}

int leValorAtk(char *str) {
	char linha[5];
	int temp;
	
	//imprime valor (dano ou gasto) do ataque
	printf("%s: ", *str);
	fgetspc(5, linha);
	temp = atoi(linha);
	//testando o valor entrado para temp
	if (temp == 0) {
		puts("[0]N [1]S");
		puts("O valor entrado foi 0?");
		fgetspc(3, linha);
		//se for invalido, há uma chamada recursiva
		if (linha[0] != '1') {
			puts("Entre com um novo valor!");
			leValorAtk(str);
		}
	}
	return temp;
}

void leDados(int ataque, ATK *perAtaque) {
	int temp;
	
	//le nome
	printf("Nome: ");
	fgetspc(TAM, perAtaque[ataque].nome);
	//le dano
	temp = leValorAtk("Dano");
	perAtaque[ataque].dano = temp;
	//le gasto
	temp = leValorAtk("Gasto");
	perAtaque[ataque].gasto = temp;
	return;
}

int editaAtk(int qtdAtk, ATK *perAtaque) {
	int opcao;
	char linha[3];
	
	//mostra os ataques do personagem escolhido
	imprimeAtks(qtdAtk, perAtaque);
	printf("[-1]Voltar\n[1]Adc novo atk\n[2]Editar atk existente\nOpcao: ");
	fgetspc(3, linha);
	opcao = atoi(linha);
	
	if (opcao == 1) {
		
		//alocando memoria para um novo atk
		realloc(perAtaque, qtdAtk * sizeof (ATK));
		//le os dados do ataque
		leDados(qtdAtk, perAtaque);
		//incrementa a qtd de ataques
		qtdAtk++;
		//imprime os ataques do personagem
		imprimeAtks(qtdAtk, perAtaque);
		
		return qtdAtk;
	
	} else if (opcao == 2) {
	
		do {
			//le o ataque a ser editado
			printf("Qual? ");
			fgetspc(3, linha);
			opcao = atoi(linha);
			if (opcao == 0 || opcao > qtdAtk) {
				puts("Escolha Invalida!");
				continue;
			}
			opcao--;
		} while (FALSE); //nunca repete
		
		//le os dados do ataque
		leDados(opcao, perAtaque);
		//imprime os ataques do personagem
		imprimeAtks(qtdAtk, perAtaque);
		
		return qtdAtk;
		
	} else if (opcao == -1) {
		
		puts("Saindo da funcao");
		return qtdAtk;
		
	} else {
		
		puts("Opcao invalida");
		return editaAtk(qtdAtk, perAtaque);
		
	}
}

int salvaAlteracoes(char *dir, PER *per) {
	char move[TAM * 9];
	char fichas[] = "BDfichas.txt ";
	char skills[] = "BDskills.txt ";
	char str[] = "backup_bds";
	char linha[TAM * 3];
	int i, j;
	FILE *paBD;
	/*
	strcpy(move, "move ");
	//move 
	strcat(move, dir);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\ 
	strcat(move, fichas);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\BDfichas.txt 
	strcat(move, dir);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\BDfichas.txt
	// C:\\Users\\Samsung\\Desktop\\3\\bds\\ 
	strcat(move, str);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\BDfichas.txt
	// C:\\Users\\Samsung\\Desktop\\3\\bds\\backup_bds
	puts(move);
	
	//movendo backup
	system(move);
	*/
	strcpy(linha, dir);
	strcat(linha, fichas);
	//abrindo arquivo para edicao
	if ( (paBD = fopen(linha, "w")) == NULL) {
		return FAIL;
	}
	//escrevendo no arquivo
	for (i = 0; i < DIM; i++) {
		fprintf(paBD, "nome: %s\n", per[i].nome);
		fprintf(paBD, "hpFull: %d\n", per[i].hpFull);
		fprintf(paBD, "mopFull: %d\n", per[i].mopFull);
		fprintf(paBD, "aop: %d\n", per[i].aop);
		fprintf(paBD, "pop: %d\n", per[i].pop);
		fprintf(paBD, "cdn: %d\n", per[i].cdn);
		fprintf(paBD, "ten: %d\n", per[i].ten);
		fprintf(paBD, "modClas: %.1lf\n", per[i].modClas);
		fprintf(paBD, "modFor: %d\n", per[i]. modFor);
		fprintf(paBD, "vel: %d\n", per[i].vel);
		fprintf(paBD, "corpo: %d\n", per[i].corpo);
		fprintf(paBD, "ataques: %d\n", per[i].qtdAtk);
		for (j = 0; j < per[i].qtdAtk; j++) {
			fprintf(paBD, "%s: ", per[i].ataque[j].nome);
			fprintf(paBD, "%d\n", per[i].ataque[j].dano);
		}
		fprintf(paBD, "\n");
	}
	//liberando memoria
	fclose(paBD);
	
	//SEGUNDA PARTE
	/*
	strcpy(move, "move ");
	//move 
	strcat(move, dir);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\ 
	strcat(move, skills);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\BDskills.txt 
	strcat(move, dir);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\BDskills.txt
	// C:\\Users\\Samsung\\Desktop\\3\\bds\\ 
	strcat(move, str);
	//move C:\\Users\\Samsung\\Desktop\\3\\bds\\BDskills.txt
	// C:\\Users\\Samsung\\Desktop\\3\\bds\\backup_bds
	
	//movendo backup
	system(move);
	*/
	strcpy(linha, dir);
	strcat(linha, skills);
	//abrindo arquivo para edicao
	if ( (paBD = fopen(linha, "w")) == NULL) {
		return FAIL;
	}
	//escrevendo no arquivo
	for (i = 0; i < DIM; i++) {
		fprintf(paBD, "%s:\n", per[i].nome);
		for (j = 0; j < per[i].qtdAtk; j++) {
			fprintf(paBD, "%s: ", per[i].ataque[j].nome);
			fprintf(paBD, "%d\n", per[i].ataque[j].gasto);
		}
		fprintf(paBD, "\n");
	}
	//liberando memoria
	fclose(paBD);
	
	//FIM
	puts("Dados salvos!\n");
	
	return SUCESS;
}

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
		printf("[0]N [1]S\nO valor eh 0? ");
		fgetspc(3, temp);
		if (temp[0] != 1) {
			return leInt(c);
		}
	}
	return d;
}

void criaChar(char *str) {
	int valor, i;
	char ficha[] = "BDfichas.txt";
	char skill[] = "BDskills.txt";
	char dir[TAM * 2];
	double valorDouble;
	FILE *paBD;
	PER person;
	
	//lendo valores do colsole
	puts("Entre com os dados do personagem:");
	printf("Nome: ");
	fgetspc(5, person.nome);
	printf("hpFull: ");
	strupr(person.nome);
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
	person.cdn = leInt('i');
	printf("modClas: ");
	person.modClas = leInt('d');
	printf("modFor: ");
	person.modFor = leInt('i');
	printf("vel: ");
	person.vel = leInt('i');
	printf("corpo: ");
	person.corpo = leInt('i');
	printf("qtdAtk: ");
	person.qtdAtk = leInt('i');
	//lendo valores dos ataques
	for (i = 0; i < person.qtdAtk; i++) {
		printf("Habilidade %d:\n", i);
		printf("Nome: ");
		fgetspc(DIM, person.ataque[i].nome);
		strupr(person.ataque[i].nome);
		printf("Dano: ");
		person.ataque[i].dano = leInt('i');
		printf("Gasto: ");
		person.ataque[i].gasto = leInt('i');
	}
	
	//definindo diretorio
	strcpy(dir, str);
	strcat(dir, ficha);
	
	//abrindo arquivo para insercao de dados
	if ( (paBD = fopen(dir, "a")) == NULL) {
		puts("Nao foi carregado fluxo ao ponteiro paBD");
		return;
	}
	
	//escrevendo no arquivo ficha
	fprintf(paBD, "\nNome: %s\n", person.nome);
	fprintf(paBD, "hpFull: %d\n", person.hpFull);
	fprintf(paBD, "mopFull: %d\n", person.mopFull);
	fprintf(paBD, "aop: %d\n", person.aop);
	fprintf(paBD, "pop: %d\n", person.pop);
	fprintf(paBD, "cdn: %d\n", person.cdn);
	fprintf(paBD, "ten: %d\n", person.ten);
	fprintf(paBD, "modClas: %.1lf\n", person.modClas);
	fprintf(paBD, "modFor: %d\n", person.modFor);
	fprintf(paBD, "vel: %d\n", person.vel);
	fprintf(paBD, "corpo: %d\n", person.corpo);
	fprintf(paBD, "ataques: %d\n", person.qtdAtk);
	for (i = 0; i < person.qtdAtk; i++) {
		fprintf(paBD, "%s: %d\n", person.ataque[i].nome, person.ataque[i].dano);
	}
	
	//esvaziando ponteiro file
	fclose(paBD);
	
	//definindo novo diretorio
	strcpy(dir, str);
	strcat(dir, skill);
	
	//abrindo arquivo para insercao de dados
	if ( (paBD = fopen(dir, "a")) == NULL) {
		puts("Ficha foi salva");
		puts("Nao foi carregado fluxo ao ponteiro paBD");
		return;
	}
	//escrevendo no arquivo skills
	fprintf(paBD, "\n");
	for (i = 0; i < person.qtdAtk; i++) {
		fprintf(paBD, "%s: %d\n", person.ataque[i].nome, person.ataque[i].gasto);
	}
	//esvaziando o ponteiro file
	fclose(paBD);
	
	//sucesso. mensagens finais
	puts("Os dados foram salvos!");
	puts("Por favor, SIGA OS PASSOS:");
	puts("altere as DIM dos codigos e recompile");
	puts("coloque os valores de hp e mop nos BDs");
	puts("feche esse programa, altere a DIM e recompile");
	system("pause>>null");
	system("pause");
	
	return;
}

int menuPrincipal() {
	int opcao;
	char linha[3];
	
	printf("[1]Editar um atributo\n"
		   "[2]Editar um personagem\n"
		   "[3]Criar personagem\n"
		   "[4]Salvar dados\n"
		   "[5]Sair do programa\n"
		   "Opcao: ");
		   
	fgetspc(3, linha);
	printf("\n");
	opcao = atoi(linha);
	if (opcao == 0) {
		puts("Entrada invalida!");
		return menuPrincipal();
	}
	return opcao;
}

int leOpcao() {
	int opcao;
	char linha[4];
	
	puts("Qual atributo deseja trocar?");
	printf("[1] - HP\n" //hpFull
		   "[2] - MOP\n" //mopFull
		   "[3] - AOP\n"
		   "[4] - POP\n"
		   "[5] - CDN\n"
		   "[6] - TEN\n"
		   "[7] - modFor\n"
		   "[8] - vel\n"
		   "[9] - corpo\n"
		   "[10] - qtd ataques\n"
		   "[11] - modificador de classe\n"
		   "[12] - salvar alteracoes\n"
		   "[-1] - voltar\n"
		   "Opcao: ");
	
	fgetspc(4, linha);
	printf("\n");
	opcao = atoi(linha);;
	if (opcao == 0) {
		puts("Opcao invalida!");
		return leOpcao();
	}
	return opcao;
}

int editaAtributo(int per, char *dir, PER *ficha) {
	int opcao, valor;
	
	opcao = leOpcao();
	switch (opcao) {
		case 1:
			valor = leValor(ficha[per].hpFull, ficha[per], "HP");
			ficha[per].hpFull = valor;
			break;
		case 2:
			valor = leValor(ficha[per].mopFull, ficha[per], "MOP");
			ficha[per].mopFull = valor;
			break;
		case 3:
			valor = leValor(ficha[per].aop, ficha[per], "AOP");
			ficha[per].aop = valor;
			break;
		case 4:
			valor = leValor(ficha[per].pop, ficha[per], "POP");
			ficha[per].pop = valor;
			break;
		case 5:
			valor = leValor(ficha[per].cdn, ficha[per], "CDN");
			ficha[per].cdn = valor;
			break;
		case 6:
			valor = leValor(ficha[per].ten, ficha[per], "TEN");
			ficha[per].ten = valor;
			break;
		case 7:
			valor = leValor(ficha[per].modFor, ficha[per], "mod. FOR");
			ficha[per].modFor = valor;
			break;
		case 8:
			valor = leValor(ficha[per].vel, ficha[per], "Vel");
			ficha[per].vel = valor;
			break;
		case 9:
			valor = leValor(ficha[per].corpo, ficha[per], "corpo");
			ficha[per].corpo = valor;
			break;
		case 10:
			ficha[per].qtdAtk = editaAtk(ficha[per].qtdAtk, ficha[per].ataque);
			break;
		case 11:
			valor = leValor(10 * ficha[per].modClas, ficha[per], "mod. Clas * 10");
			ficha[per].modClas = 1.0 * valor / 10;
			break;
		case 12:
			salvaAlteracoes(dir, ficha);
			break;
		case -1:
			opcao = 0;
			break;
		default:
			puts("Opcao invalida!");
			opcao = 1;
	}
	return opcao;
}
//########### INICIO DO CODIGO ############

int main() {
	int i, per, opcao, valor;
	char dir[] = "C:\\Users\\Samsung\\Desktop\\3\\bds\\";
	char linha[3];
	PER ficha[DIM];
	
	
	system("color b");
	system("title GxG SQL");
	system("cls");
	
	if (leBD(dir, ficha) == FAIL) {
		puts("Nao foi carregado fluxo ao ponteiro FILE");
		system("pause>>null");
		return 1;
	}
	if (leVol(dir, ficha) == FAIL) {
		puts("Nao foi carregado fluxo ao ponteiro FILE2");
		system("pause>>null");
		return 1;
	}	
	//imprime os dados de todos os personagens
	for (i = 0; i < DIM; i++) {
		listaFicha(ficha[i]);
	}
	
	do {
		//menu principal
		opcao = menuPrincipal();
		
		switch(opcao) {
			case 1:
				//le o nome do personagem a ser editado
				per = leNome(ficha);
				//funcao com o menu editaAtributo
				listaFicha(ficha[per]);
				opcao = editaAtributo(per, dir, ficha);
				listaFicha(ficha[per]);
				break;
			case 2:
				//le o nome do personagem a ser editado
				per = leNome(ficha);
				listaFicha(ficha[per]);
				do {
					opcao = editaAtributo(per, dir, ficha);
				} while (opcao);
				listaFicha(ficha[per]);
				break;
			case 3:
				criaChar(dir);
				break;
			case 4:
				salvaAlteracoes(dir, ficha);
				break;
			case 5:
				printf("[1]Se ja tiver SALVADO! ");
				fgetspc(3, linha);
				opcao = atoi(linha);
				if (opcao == 1) {
					puts("Saindo...");
					//time
					return 0;
				}
				//else
				break;
			default:
				puts("Opcao invalida!");
				break;
		}
	} while (1);
}

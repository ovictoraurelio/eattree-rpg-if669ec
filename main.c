/***
	*
	*	--- RPG IP 2016 ---
	* @descricao main.c é o arquivo principal do jogo, onde toda a mágica acontece!
	* @desde 06/06/2016
	* @autores
	*   [
	*	 	  Bruno Melo
	*		  Filipe Cumaru
	*		  Rafael Santana
	*		  Victor Aurélio
	* 	]
	*
	*
***/
#include <stdio.h>
#include <stdlib.h> // malloc, calloc, free
#include <string.h> // strcmp, stract, strcpy
#include <time.h>
#include <ctype.h> // toupper
#define KEY_H 72
#if defined( __unix__ ) || defined( __APPLE__ )
	#include <unistd.h>
	#include "getch.h"
	#define CLS "clear"
	#define KEY_ENTER 10
	#define CTRL_C 86 // representa a tecla V porque o ctrl+c no linux, teriamos que usar signal event
	void delay( unsigned long ms ){
	    usleep( ms * 1000 );//micro segundos
	}
#elif defined(__WIN32__) || defined(_WIN32) || defined(WIN32) || defined(__WINDOWS__) || defined(__TOS_WIN__)
	#define CLS "cls"
	#include <conio.h>
	#include <windows.h>
	#define KEY_ENTER 13
	#define CTRL_C 3
  	void delay( unsigned long ms ){
    	Sleep( ms );
	}
#endif



typedef struct{
	char representacao;// A letra que vai representar o personagem no mapa
	char nome[20]; //O Nome do personagem
	int ataque, defesa, vida;// Variáveis que representam quanto de ataque, defesa ou vida o personagem possui
	int x, y;// Coordenadas X e Y do personagem no mapa
}Personagem;

typedef struct{
	int continuarExecutandoJogo;
	int pontuacao;
	int tamanho; // Tamanho de  Linhas x Colunas (Nossa matriz será sempre quadrada)
	char *nomePacotePersonagens;
	char *nomeMapa;
	char **mapa; // A matriz que representa o mapa
	Personagem *heroi;
	Personagem *monstros;
}Jogo;

/***
	*
	*	Declarando cabeçalhos das funções
	*
***/
Personagem* buscarMonstroEmbate(Jogo *jogo);
void carregarMapaParaOJogo(Jogo *jogo);
void carregarPersonagensParaOJogo(Jogo *jogo);
void desenharMapa(Jogo *jogo, int *rodada);
void definirNovaPosicaoNoMapa(int teclaPressionada, int *x, int *y, int *tamanhoMapa, int vidaPersonagem);
void exibirMenu(char **conteudo, int numeroDeLinhas, char *ultimo, char *opcao);
void exibirTelaEmbate(Jogo *jogo, Personagem *monstroEmbate);
void iniciarJogo(Jogo *atual);
int iniciarEmbate(Jogo *atual);
int move(int w, int x, int y, int z, int e);
void movimentarPersonagem(int x, int y, Personagem* personagem, Jogo* jogo, int tamanhoMapa, int monstro, int *HP);
char getchPersonalizado();
char pegarOpcaoMenu(char *nomeArquivoDeMenu);
void posicionarPersonagem(Personagem *personagem, int x, int y);
void telaCriarMapa();
void telaCriarPacoteDePersonagens(Jogo *jogo);
void telaSelecionarMapa(Jogo *jogo);
void telaSelecionarPacoteDePersonagens(Jogo *jogo);
void telaPreJogo(Jogo *jogo);
int validaMovimento (int x, int y, int tamanho, int monstro, Jogo* jogo);
int validarVidaPersonagens(Jogo *jogo);
void sairDoJogo(Jogo *jogo);
// PROVAVEL BIBLIOTECA - FUNÇÕES DE ARQUIVO:
void alterarCaracterPrePalavra(char *texto, char *busca, char c);
int buscarNomeEmArquivo(char *busca, char *nomeArquivo);
void criarPacotePersonagens(char *nomePacotePersonagens, Personagem *heroi, Personagem *monstros);
void criarMapa (char* name);
void exibirArquivo(char *nomeDoArquivo);
char** lerArquivoTexto(char *nomeDoArquivo, char **conteudo, int *numeroDeLinhas);
void listarPersonagens();
void listarMapas();
void salvarMapaNoIndex(char *nomeDoMapa);
void salvarPersonagemNoIndex(char *nomeDoPersonagem);

int main(int argc, char const *argv[]){
	Jogo *jogoAtual = (Jogo*) calloc(1,sizeof(Jogo));
	jogoAtual->heroi = NULL;
	jogoAtual->monstros = NULL;
	jogoAtual->continuarExecutandoJogo=1;
	jogoAtual->nomePacotePersonagens = (char*) malloc(sizeof(char) * 77);
	jogoAtual->nomeMapa = (char*) malloc(sizeof(char) * 61);
	while(jogoAtual->continuarExecutandoJogo > 0){
		fflush(stdin);
		system(CLS);
		if(jogoAtual->continuarExecutandoJogo == 1){
			jogoAtual->continuarExecutandoJogo++;
			exibirArquivo("templates/bem_vindo");
			getchPersonalizado();
		}
		switch(pegarOpcaoMenu("templates/menu_tela_inicial")){// De acordo com a opção que o menu retornar
			case 'W': //Caso seja selecionada a primeira opção do menu
				telaPreJogo(jogoAtual);
			break;
			case 'S': //Caso seja selecionada a opção de baixo do menu (GAME OPTIONS)
				switch(pegarOpcaoMenu("templates/menu_opcoes_jogo")){
					case 'W': // CONFIGURAÇÕES DOS MAPAS
						switch(pegarOpcaoMenu("templates/menu_mapa")){
							case 'W': //SELECIONAR UM MAPA
								telaSelecionarMapa(jogoAtual);
							break;
							case 'S': //CRIAR UM MAPA
								telaCriarMapa(jogoAtual);
							break;
							default: break;
						};
					break;
					case 'S': // CONFIGURAÇÕES DE PERSONAGENS
						switch(pegarOpcaoMenu("templates/menu_personagens")){
							case 'W': //SELECIONAR UM PERSONAGEM
								telaSelecionarPacoteDePersonagens(jogoAtual);
							break;
							case 'S': //CRIAR UM PERSONAGEM
								telaCriarPacoteDePersonagens(jogoAtual);
							break;
							default: break;
						};
					break;
					default: break;
				};
			break;
			case CTRL_C:
				jogoAtual->continuarExecutandoJogo = 0;
			break;
			default: break;
		};
	}
	sairDoJogo(jogoAtual);
	return 0;
}


/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 -MENUS-
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
/***--------------------------------------------------------------***/
/***
	*
	*	1 - pegarOpcaoMenu	#Esta retorna qual a opção escolhida pelo usuário no menu.
	*	2 - exibirMenu		#Esta função exibe o menu e de acordo com a atual posição do cursor
	*   	 				 ela coloca '>' em cima ou em baixo do texto.
	*
***/
char pegarOpcaoMenu(char *nomeArquivoDeMenu){
	char ultimo='S', opcao='W', **conteudo;
	int numeroDeLinhas;
	conteudo = lerArquivoTexto(nomeArquivoDeMenu, conteudo, &numeroDeLinhas);
	do{
		if(opcao == KEY_H){ // garantir que após a tela de ajuda, ele redesenha o menu.
			opcao = ultimo == 'W' ? 'S' : 'W';
		}
		if(ultimo != opcao && (opcao == 'W' || opcao == 'S')){// Só vai redesenhar se ele realmente alternar entre as posições...
			system(CLS);
			exibirMenu(conteudo, numeroDeLinhas, &ultimo, &opcao);
		}
	}while((opcao = getchPersonalizado()) && opcao != KEY_ENTER && opcao != CTRL_C);
	if(opcao == CTRL_C){
		return CTRL_C;//vai retornar um case inválido (diferente de w, e de s).. fará com que retorne para o menu principal.
	}
	return ultimo;
}
void exibirMenu(char **conteudo, int numeroDeLinhas, char *ultimo, char *opcao){
	int i;
	for(i=2; i<numeroDeLinhas; i++){
				//Temos as duas primeiras palavras escaneadas,
				//sabemos diretamente do menu quem deve ser a opção de cima e a opção de baixo.
				//conteudo[0] - primeira palavra. conteudo[1] - segunda palavra.
				if(*opcao == 'W'){
					*ultimo = 'W';
					alterarCaracterPrePalavra(conteudo[i], conteudo[0], '>');
					alterarCaracterPrePalavra(conteudo[i], conteudo[1], ' ');
				}else if(*opcao == 'S'){
					*ultimo = 'S';
					alterarCaracterPrePalavra(conteudo[i], conteudo[0], ' ');
					alterarCaracterPrePalavra(conteudo[i], conteudo[1], '>');
				}
				printf("%s", conteudo[i]);
	}
}

/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 -JOGO-
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
/***--------------------------------------------------------------***/
/***
	*
	*	buscarMonstroEmbate
	*	definirNovaPosicaoNoMapa
	*	desenharMapa				#Função que modifica o mapa a cada rodada. A cada movimento do heroi, ela reseta o mapa e produz um novo com as novas
									 informações sobre a sua posição, posição dos monstros e das árvores.
	*	move 						#Função que determina o movimento pseudo-aleatório dos monstros
	*	movimentarPersonagem		#Função que valida a nova posição do monstro dada pela função move
	* 	range
	*	posicionarPersonagem
	*	telaPreJogo
	*	exibirTelaEmbate
	*	iniciarJogo
	*	iniciarEmbate
	*
	*
***/
Personagem* buscarMonstroEmbate(Jogo *jogo){
	int i;
	for(i=0; i<4; i++){
		if(jogo->monstros[i].x == jogo->heroi->x && jogo->monstros[i].y == jogo->heroi->y){
			return &jogo->monstros[i];
		}
	}
	return NULL;
}
void definirNovaPosicaoNoMapa(int teclaPressionada, int *x, int *y, int *tamanhoMapa, int vidaPersonagem){
	if(vidaPersonagem > 0){// O PERSONAGEM SÓ ANDA SE A VIDA FOR MAIOR QUE ZERO. Isso possibilita que quando eu alocar x = -1 e y = -1 ele fico no "limbo" para sempre
		switch(teclaPressionada){
			case 65: /** getch 'A' == 65 **/
			case 2:
					if(*x > 1)
						(*x)--;
			break;
			case  87: /** getch 'W' == 87 **/
			case 3:
					if(*y > 1)
						(*y)--;
			break;
			case 83: /** getch 'S' == 83 **/
			case 1:
					if(*y < *tamanhoMapa - 2)
						(*y)++;
			break;
			case 68: /** getch 'D' == 68 **/
			case 0:
					if(*x < *tamanhoMapa - 1)
						(*x)++;
			break;
			default:break;
		};
	}
}
void desenharMapa(Jogo *jogo, int *rodada){
	int i,j,k,imprimirConteudoDoMapa;
	for(i=0; i<jogo->tamanho; i++){
		for(j=0; j<=jogo->tamanho; j++){
				imprimirConteudoDoMapa=1;
				if(jogo->heroi->x == j && jogo->heroi->y == i){
						printf("H");
						imprimirConteudoDoMapa=0;
				}
				for(k=0; k<4; k++){
						if(jogo->monstros[k].x == j && jogo->monstros[k].y == i){
							printf("M");//representacao alerta!!!
							imprimirConteudoDoMapa=0;
						}
				}
				if(imprimirConteudoDoMapa){
						printf("%c",jogo->mapa[i][j]);
				}
		}
		printf("\n");
	}
	printf("\nRodada: %d\nPontuacao: %d\nVida: %d\n", *rodada,jogo->pontuacao, jogo->heroi->vida);
}
int move(int w, int x, int y, int z, int e){
	if (w == 0 || x == 0 || y == 0 || z == 0 || e == 0)
		return 0;
	else
		return ((((w+x+e)*y)/z) + move(w-1,x-1,y-1,z-1,e))%5;
}
void movimentarPersonagem(int x, int y, Personagem* personagem, Jogo* jogo, int tamanhoMapa, int monstro, int *HP){
	int podeMovimentar=0;
	if(monstro == 0)// Monstro 0 é o heroi!
		podeMovimentar = 1;
	else if(monstro == 1 && jogo->mapa[y][x] != '*' && jogo->heroi->x != x && jogo->heroi->y != y && (0 < x && x < tamanhoMapa/2 && 0 < y && y < tamanhoMapa/2))
		podeMovimentar = 1;
	else if(monstro == 2 && jogo->mapa[y][x] != '*' && jogo->heroi->x != x && jogo->heroi->y != y && (tamanhoMapa/2 < x && x < tamanhoMapa-1 && 0 < y && y < tamanhoMapa/2))
		podeMovimentar = 1;
	else if(monstro == 3 && jogo->mapa[y][x] != '*' && jogo->heroi->x != x && jogo->heroi->y != y && (0 < x && x < tamanhoMapa/2 && tamanhoMapa/2 < y && y < tamanhoMapa-1))
		podeMovimentar = 1;
	else if(monstro == 4 && jogo->mapa[y][x] != '*' && jogo->heroi->x != x && jogo->heroi->y != y && (tamanhoMapa/2 < x && x < tamanhoMapa-1 && tamanhoMapa/2 < y && y < tamanhoMapa-1))
		podeMovimentar = 1;
	if(podeMovimentar){
		personagem->x = x;
		personagem->y = y;
		if (monstro == 0) {
			personagem->vida = personagem->vida + ((*HP)*0.1);
			if (personagem->vida > *HP)
				personagem->vida = *HP;
		}
	}
}

void posicionarPersonagem(Personagem *personagem, int x, int y){
	personagem->x = x;
	personagem->y = y;
}

void telaPreJogo(Jogo *jogo){
		system(CLS);

		if(jogo->heroi == NULL){ // Heroi sempre vai estar alocado.. Porém se tivermos carregado um pacote de personagens teremos um nome para o heroi
				jogo->nomePacotePersonagens = "default";
		}
		if(jogo->mapa == NULL){
				jogo->nomeMapa = "default";
		}
		jogo->pontuacao=0;
		carregarPersonagensParaOJogo(jogo);
		carregarMapaParaOJogo(jogo);
		if(jogo->heroi != NULL && jogo->mapa != NULL && jogo->monstros != NULL){
				exibirArquivo("templates/jogo_carregado");
				printf("\nMapa selecionado: %s\n",jogo->nomeMapa);
				printf("\nPacote de personagens selecionado: %s\n",jogo->nomePacotePersonagens);
				delay(2000);// Espera 2 segundos
				iniciarJogo(jogo);
		}else{
				printf("\nProblemas ao carregar o jogo!\n");
				exit(1);
		}
}
void exibirTelaEmbate(Jogo *jogo, Personagem *monstroEmbate){
	system(CLS);
	exibirArquivo("templates/tela_batalha");
	printf("Heroi: %s\nAtaque: %d\nDefesa: %d\nVida: %d\n\n\nMonstro: %s\nAtaque: %d\nDefesa: %d\nVida: %d \n",jogo->heroi->nome, jogo->heroi->ataque, jogo->heroi->defesa, jogo->heroi->vida, monstroEmbate->nome,monstroEmbate->ataque,monstroEmbate->defesa, monstroEmbate->vida);
}
int validarVidaPersonagens(Jogo *jogo){
	return jogo->heroi->vida > 0 && (jogo->monstros[0].vida > 0 || jogo->monstros[1].vida > 0  || jogo->monstros[2].vida > 0 || jogo->monstros[3].vida > 0);
}
void iniciarJogo(Jogo *jogo){
	 	char teclaPressionada;
		int rodada=0, metadeTamanho = jogo->tamanho/2, i = 0, novoX, novoY, movimento, continuarAposEmbate = 1, HP_max = jogo->heroi->vida;
		posicionarPersonagem(jogo->heroi, metadeTamanho, metadeTamanho);
		posicionarPersonagem(&jogo->monstros[0], metadeTamanho/2, metadeTamanho/2);
		posicionarPersonagem(&jogo->monstros[1], metadeTamanho/2 + metadeTamanho, metadeTamanho/2);
		posicionarPersonagem(&jogo->monstros[2], metadeTamanho/2, metadeTamanho/2 + metadeTamanho);
		posicionarPersonagem(&jogo->monstros[3], metadeTamanho/2 + metadeTamanho, metadeTamanho/2 + metadeTamanho);

		system(CLS);
		desenharMapa(jogo, &rodada);

		while(jogo->continuarExecutandoJogo > 0 && continuarAposEmbate && validarVidaPersonagens(jogo) && teclaPressionada != CTRL_C){
			while(jogo->continuarExecutandoJogo > 0 && continuarAposEmbate && validarVidaPersonagens(jogo) && ( teclaPressionada = getchPersonalizado() ) && (teclaPressionada=='A' || teclaPressionada=='S' || teclaPressionada=='W' || teclaPressionada=='D') && teclaPressionada != CTRL_C){
				if(jogo->pontuacao > 0){
					jogo->pontuacao--;
				}
				novoX = jogo->heroi->x;
				novoY = jogo->heroi->y;
				definirNovaPosicaoNoMapa(teclaPressionada, &novoX, &novoY, &jogo->tamanho, jogo->heroi->vida);
				movimentarPersonagem(novoX, novoY, jogo->heroi, jogo, jogo->tamanho, 0, &HP_max);
				if(jogo->mapa[novoY][novoX] == '*'){
					jogo->pontuacao+=20;
					jogo->mapa[novoY][novoX] = ' ';
				}else if(buscarMonstroEmbate(jogo) != NULL){
					continuarAposEmbate = iniciarEmbate(jogo);
				}
				for (i = 0; i < 4; i++){
					if(jogo->monstros[i].vida > 0){
						movimento = move(jogo->heroi->x,jogo->heroi->y,jogo->monstros[i].x,jogo->monstros[i].y,rodada);
						novoX = jogo->monstros[i].x;
						novoY = jogo->monstros[i].y;
						definirNovaPosicaoNoMapa(movimento, &novoX, &novoY, &jogo->tamanho, jogo->monstros[i].vida);
						movimentarPersonagem(novoX, novoY, &jogo->monstros[i], jogo, jogo->tamanho, i+1, 0);
					}
				}
				if(jogo->monstros[0].vida == 0 && jogo->monstros[1].vida == 0  && jogo->monstros[2].vida == 0 && jogo->monstros[3].vida == 0){
						switch(pegarOpcaoMenu("templates/menu_voce_venceu")){
							case 'W':
								continuarAposEmbate = 0;//para encerrar os loops.
							break;
							case 'S':
								jogo->continuarExecutandoJogo = 0; // para encerrar os loops.
							break;
							default: break;
						};
				}else {
					if(continuarAposEmbate){ // para não desenhar após reiniciar o jogo..
						rodada++;
						system(CLS);
						desenharMapa(jogo, &rodada);
					}
				}
			}
		}
}

int range(int w, int x, int y, int z){
	if (w <= 0 || x <= 0|| y <= 0|| z <= 0)
		return 0;
	else
		return (((w + x)*y/(z + 1)) + range(z-10,w-10,x-10,y-10))%30;
}

int iniciarEmbate(Jogo *jogo){
	int i, jogada, rangeAtaqueHeroi;
	Personagem *monstroEmbate;
	monstroEmbate = buscarMonstroEmbate(jogo);
	exibirTelaEmbate(jogo, monstroEmbate);
	printf("\n\n\n\t\t- OS CAMPEOES JA ESTAO NA ARENA DE BATALHA -\n\n%s voce eh o primeiro a atacar!\nPressione a tecla T para atacar: ", jogo->heroi->nome);
	while(jogo->heroi->vida > 0 && monstroEmbate->vida > 0 && ((jogada = toupper(getch())) && jogada != 'R')){
		if(jogada == 'T' && jogo->heroi->vida > 0 && monstroEmbate->vida > 0){
				rangeAtaqueHeroi = range(jogo->heroi->vida, jogo->heroi->ataque, monstroEmbate->vida, monstroEmbate->ataque);

				monstroEmbate->vida = monstroEmbate->vida - (jogo->heroi->ataque + rangeAtaqueHeroi - monstroEmbate->defesa);
				jogo->heroi->vida = jogo->heroi->vida - (monstroEmbate->ataque + range(monstroEmbate->vida, monstroEmbate->ataque, jogo->heroi->vida, jogo->heroi->ataque) - jogo->heroi->defesa );
				//jogo->heroi->vida = jogo->heroi->vida - (monstroEmbate->ataque + rangeAtaqueHeroi - jogo->heroi->defesa );

				jogo->pontuacao += jogo->heroi->ataque + rangeAtaqueHeroi;
				jogo->heroi->vida = jogo->heroi->vida > 0 ? jogo->heroi->vida : 0;
				monstroEmbate->vida = monstroEmbate->vida > 0 ? monstroEmbate->vida : 0;
				exibirTelaEmbate(jogo, monstroEmbate);
				printf("\nAgora eh sua vez %s de atacar:\n", jogo->heroi->nome);
		}
	}
	system(CLS);
	if(monstroEmbate->vida == 0){
		posicionarPersonagem(monstroEmbate, -1, -1);
		exibirArquivo("templates/derrotou_monstro");
		printf("********************************************************************************\n");
		printf("\t\t\t\t %s\n",monstroEmbate->nome);
		printf("********************************************************************************\n");
		printf("********************************************************************************\n");
		delay(1500);
		return 1;
	}
	if(jogo->heroi->vida <= 0){
		switch(pegarOpcaoMenu("templates/menu_perdeu_jogo")){
			case 'W':
				return 0;
			break;
			case 'S':
				jogo->continuarExecutandoJogo = 0;
			break;
			default: break;
		};
	}
	if(jogada == 'R'){
		jogo->heroi->vida = jogo->heroi->vida/2;
		posicionarPersonagem(jogo->heroi, jogo->tamanho/2, jogo->tamanho/2);
		return 1;
	}
	return 0;//só vem pra cá se o heroi tiver morrido!
}
/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 -MAPAS-
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
/***--------------------------------------------------------------***/
/***
	*
	*	1 - telaSelecionarMapa
	*	2 - telaCriarMapa
	*	3 - criarMapa 						#Esta função cria um novo mapa segundo o jogador
	*	4 - salvarMapaNoIndex 				#Essa função salva o nome do mapa no arquivo  mapas.txt nosso indice de mapas
	*	5 - listarMapas  					#Essa função carrega os mapas salvos. Apartir do arquivo mapas.txt
	*	6 - carregarMapaParaOJogo			#Essa função carrega o mapa do arquivo de texto.
	*
***/
void telaSelecionarMapa(Jogo *jogo){
	system(CLS);
	listarMapas();
	fflush(stdin);//BUG MEMORY!
	printf("\nDigite o nome do mapa que deseja selecionar:\n");
	scanf(" %50s", jogo->nomeMapa);
	while(buscarNomeEmArquivo(jogo->nomeMapa, "mapas.txt") == 0){
		printf("\nMapa informado nao esta na lista! Digite o nome do mapa que deseja selecionar:\n");
		scanf(" %50s", jogo->nomeMapa);
	}
	carregarMapaParaOJogo(jogo);
}
void telaCriarMapa(){
	system(CLS);
	char *nomeDoMapa = (char*) malloc(sizeof(char) * 51);
	fflush(stdin);//BUG MEMORY!
	printf("\nDigite o nome do mapa(ate 50 caracteres e sem espacos): ");
	scanf(" %50s", nomeDoMapa);
	while(buscarNomeEmArquivo(nomeDoMapa, "mapas.txt") > 0){
		printf("\nJa existe um mapa com este nome, por favor, digite outro: \n");
		scanf(" %50s", nomeDoMapa);
	}
	criarMapa(nomeDoMapa);
	salvarMapaNoIndex(nomeDoMapa);
	free(nomeDoMapa);
}
void criarMapa(char* nomeDoMapa){
	char **mapa;
	int dim = 0, i = 0, j = 0,num;
	char *aux = (char*) malloc(sizeof(char) * 61);
	FILE* arq;
	strcpy(aux,"mapas/");
	strcat(aux,nomeDoMapa);
	strcat(aux,".txt");
	printf("Digite n na forma que (((n*4)+5) vai ser o tamanho real do mapa):");// pedindo o n
	scanf("%d", &num);
	dim = (4*num)+5;
	printf("Digite o mapa  %d x %d:\n",dim,dim);// pedindo o mapa
	mapa = (char**) malloc(dim*sizeof(char*));
	if(mapa == NULL){
		printf("Erro mapa muito grande\n");
		exit (1);
	}
	for (i = 0; i <= dim; i++) {
		mapa[i] = (char*) malloc((dim+1)*sizeof(char));
		if (mapa[i] == NULL) {
			printf("Erro mapa muito grande\n");// conferindo se o mapa n foi muito grande e a memória n pode ser alocada
			exit (1);
		}
	}
	for (i = 0; i < dim; i++)// pedindo o mapa
		for (j = 0; j < dim+1; j++)// pedindo o mapa
			scanf("%c", &mapa[i][j]);// pedindo o mapa
	arq = fopen(aux,"w");
	for (i = 0; i < dim; ++i)// transformando o limite do mapa em uma cerca
	{
		for (j = 0; j <= dim; ++j)// transformando o limite do mapa em uma cerca
		{
			if(i!=0 && i!= dim-1 && (j==0 || j==dim))mapa[i][j]='|';
			if(i==0 && j==0)mapa[i][j]='/';// transformando o limite do mapa em uma cerca
			else if(i==0 && j==dim)mapa[i][j]='\\';
			else if(i==0)mapa[i][j]='-';// transformando o limite do mapa em uma cerca
			if(i==dim-1 && j==0)mapa[i][j]='\\';
			else if(i==dim-1 && j==dim)mapa[i][j]='/';
			else if (i==dim-1)mapa[i][j]='-';// transformando o limite do mapa em uma cerca
		}
	}
	fprintf(arq, "%d\n", dim);// passando a dimensão do mapa para o arquivo
	for (i = 0; i < dim; i++) {
		for (j = 0; j <= dim; j++){
			fprintf(arq, "%c", mapa[i][j]);// passando mapa para o arquivo
		}
		fprintf(arq,"\n");
	}
	free(mapa);
	free(aux);
	fclose(arq);
}
void salvarMapaNoIndex(char *nomeDoMapa){
	FILE *file = fopen("mapas.txt", "a");
	if(file == NULL){
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		fprintf((FILE*) file,"\n");
		fputs(nomeDoMapa, (FILE*) file);
	}
	fclose(file);
}
void listarMapas(){
	int i,mapaSelecionado;
	char *texto = (char*) malloc(sizeof(char) * 51);
	FILE *file = fopen("mapas.txt", "r");
	if(file == NULL){
		printf("\n\tArquivo de mapas nao encontrado!\n");
		exit(1);
	}else{
		printf("\n\nMapas encontrados na base de dados:\n\n");
		while(!feof(file)){
			fgets(texto, 50, (FILE*) file);
			printf("%s", texto);
		}
	}
	free(texto);
	fclose(file);
}
void carregarMapaParaOJogo(Jogo *jogo){
	int i;
	char *texto = (char*) malloc(sizeof(char) * 251), *aux = (char*) malloc(sizeof(char) * 61);
	strcpy(aux, "mapas/");
	strcat(aux, jogo->nomeMapa);
	strcat(aux, ".txt");
	FILE *file = fopen(aux, "r");
	if(file == NULL){
		printf("\n\tArquivo de mapa %s nao encontrado!\n", aux);
		exit(1);
	}else{
		fscanf(file, "%d", &jogo->tamanho);
		if(jogo->mapa != NULL){
			free(jogo->mapa);
		}
		jogo->mapa = (char**) malloc(sizeof(char*) * jogo->tamanho);
		jogo->mapa[0] = (char*) malloc(sizeof(char) * 300);//quebra de linha após o inteiro
		fgets(jogo->mapa[0], 300, (FILE*) file);
		for(i=0; i<jogo->tamanho; i++){
			jogo->mapa[i] = (char*) malloc(sizeof(char) * 300);
			fgets(jogo->mapa[i], 300, (FILE*) file);
		}
	}
	free(texto);
	free(aux);
	fclose(file);
}

/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 -PERSONAGENS-
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------------------------------------------------***/
/***
	*
	*	1 - telaSelecionarPacoteDePersonagens
	*	2 - telaCriarPacoteDePersonagens
	*	3 - salvarPersonagemNoIndex
	*	4 - criarPacotePersonagens
	*	5 - listarPersonagens
	*	6 - carregarPersonagensParaOJogo
	*
***/
void telaSelecionarPacoteDePersonagens(Jogo *jogo){
		system(CLS);
		listarPersonagens();
		fflush(stdin);//BUG MEMORY!
		printf("\n\nDigite o nome do pacote personagem que deseja selecionar:\n");
		scanf(" %50s", jogo->nomePacotePersonagens);
		while(buscarNomeEmArquivo(jogo->nomePacotePersonagens, "personagens.txt") == 0){
			printf("\nNome informado nao esta na lista!\nDigite o nome do pacote personagem que deseja selecionar:\n");
			scanf(" %50s", jogo->nomePacotePersonagens);
		}
		carregarPersonagensParaOJogo(jogo);
}
void telaCriarPacoteDePersonagens(Jogo *jogo){
		int i;
		system(CLS);
		printf("Bem vindo a criacao de um novo pacote de personagens!!\n");
		fflush(stdin);//BUG MEMORY!
		printf("Digite o nome do pacote de personagens(ate 50 caracteres, sem espacos): \n");
		scanf(" %50s", jogo->nomePacotePersonagens);
		while(buscarNomeEmArquivo(jogo->nomePacotePersonagens, "personagens.txt") > 0){
			printf("Ja existe um pacote de personagens com este nome, por favor, digite outro: \n");
			scanf(" %50s",  jogo->nomePacotePersonagens);
		}
		if(jogo->heroi != NULL){
				free(jogo->heroi);
				free(jogo->monstros);
		}
		jogo->heroi = (Personagem*) malloc(sizeof(Personagem));
		jogo->monstros = (Personagem*) malloc(4*sizeof(Personagem));
		printf("Digite o nome do heroi\n");
		scanf(" %[^\n]",jogo->heroi->nome);
		printf("Digite a quantidade de ataque que o heroi possui: \n");
		scanf("%d", &jogo->heroi->ataque);
		printf("Digite a quantidade de defesa que o heroi possui: \n");
		scanf("%d", &jogo->heroi->defesa);
		printf("Digite a quantidade de vida que o heroi possui: \n");
		scanf("%d", &jogo->heroi->vida);
		for(i=0; i<4; i++){
			printf("Digite o nome do monstro %d\n", i+1);
			scanf(" %[^\n]",jogo->monstros[i].nome);
			printf("Digite a quantidade de ataque que o monstro possui: \n");
			scanf("%d", &jogo->monstros[i].ataque);
			printf("Digite a quantidade de defesa que o monstro possui: \n");
			scanf("%d", &jogo->monstros[i].defesa);
			printf("Digite a quantidade de vida que o monstro possui: \n");
			scanf("%d", &jogo->monstros[i].vida);
		}
		salvarPersonagemNoIndex(jogo->nomePacotePersonagens);
		criarPacotePersonagens(jogo->nomePacotePersonagens, jogo->heroi, jogo->monstros);
}
void salvarPersonagemNoIndex(char *nomeDoPersonagem){
	FILE *file = fopen("personagens.txt", "a");
	if(file == NULL){
		printf("\n\tArquivo de personagens nao encontrado!\n");
		exit(1);
	}else{
		fprintf((FILE*) file, "\n");
		fputs(nomeDoPersonagem, (FILE*) file);
	}
	fclose(file);
}
void criarPacotePersonagens(char *nomePacotePersonagens, Personagem *heroi, Personagem *monstros){
		char *aux = (char*) malloc(sizeof(char) * 77);
		int i;
		strcpy(aux, "personagens/");
		strcat(aux, nomePacotePersonagens);
		strcat(aux, ".bin");
		FILE *file = fopen(aux, "wb");
		if(file == NULL){
				printf("\n\tNão foi possível criar um pacote de personagens!\n");
				exit(1);
		}else{
				fwrite(heroi, sizeof(Personagem), 1, file);//"Cria"/"Limpa"
				fclose(file);
				file = fopen(aux, "ab");//Abre em modo append
				for(i=0; i<4; i++)
						fwrite(&monstros[i], sizeof(Personagem), 1, file);
		}
		free(aux);
		fclose(file);
}
void listarPersonagens(){
	int i,personagemSelecionado;
	char *texto = (char*) malloc(sizeof(char) * 50);
	FILE *file = fopen("personagens.txt", "r");
	if(file == NULL){
		printf("\n\tArquivo de personagens nao encontrado!\n");
		exit(1);
	}else{
		printf("\n\nPacotes de personagens encontrados na base de dados:\n\n");
		while(!feof(file)){
			fgets(texto, 50, (FILE*) file);
			printf("%s", texto);
		}
	}
	free(texto);
	fclose(file);
}
void carregarPersonagensParaOJogo(Jogo *jogo){
	char *aux = (char*) malloc(sizeof(char) * 68);
	int i;
	strcpy(aux,"personagens/");
	strcat(aux,jogo->nomePacotePersonagens);
	strcat(aux,".bin");
	FILE *file = fopen(aux, "rb");
	if(file == NULL){
		printf("\n\tArquivo %s nao encontrado!\n", aux);
		exit(1);
	}else{
			if(jogo->heroi != NULL){
					free(jogo->heroi);
					free(jogo->monstros);
			}
			jogo->heroi = (Personagem*) malloc(sizeof(Personagem));
			jogo->monstros = (Personagem*) malloc(4*sizeof(Personagem));
			fread(jogo->heroi, sizeof(Personagem), 1, file);
			for(i=0; i<4; i++){
				fread(&jogo->monstros[i], sizeof(Personagem), 1, file);
			}
	}
	free(aux);
	fclose(file);
}

/***-----------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------
-----------------------
-----------------------                 -UTEIS-
-----------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
---------------------------------------------------------------------
-----------------------------------------------------------------***/
/***
	*
	*	Esta função recebe um texto como parametro,
	* 	busca uma determinada palavra e coloca um caracter duas posições antes da palavra.
	*
***/
void alterarCaracterPrePalavra(char *texto, char *busca, char c){
	char *ptr, *aux;
	aux = strstr(texto,busca); // Procura por uma palavra no texto e retorna o endereço da posição do caracter 0 desta palavra.
	if(aux != NULL){
		ptr = aux-2; // Volta 2 caracteres antes do caracter de inicio da palavra.
		*ptr = c;// Substitui o conteúdo pelo caracter C que foi passado como parametro
	}
}
/***
	*
	* 	Essa função pesquisa por um nome de até 50 caracteres em algum arquivo que armazene strings de até 50 caracteres, linha por linha..
	*
***/
int buscarNomeEmArquivo(char *busca, char *nomeArquivo){
	FILE *file = fopen(nomeArquivo, "r");
	char *texto = (char*) malloc(sizeof(char) * 51);
	if(file == NULL){
			printf("\n\nTentando buscar a palavra %s no arquivo %s mas o arquivo não foi encontrado!\n", busca, nomeArquivo);
			exit(1);
	}else{
			while(!feof(file)){
					fscanf(file, " %[^\n]", texto);
					if(strcmp(texto, busca) == 0){
							free(texto);
							fclose(file);
							return 1;
					}
			}
	}
	free(texto);
	fclose(file);
	return 0;
}
/***
	*
	* 	Essa função carrega um arquivo de texto e exibe-o na tela.
	*
***/
void exibirArquivo(char *nomeDoArquivo){// função que mostra um arquivo na tela
	FILE *file;
	file = fopen(nomeDoArquivo, "r");
	char *texto = malloc(sizeof(char) * 251);
	if(file == NULL){
		printf("\n\tArquivo %s nao encontrado!\n", nomeDoArquivo);
		exit(1);
	}else{
			while(!feof(file)){
				fgets(texto, 250, (FILE*) file);
				printf("%s", texto);
			}
	}
	fclose(file);
	free(texto);
}
char** lerArquivoTexto(char *nomeDoArquivo, char **conteudo, int *numeroDeLinhas){
	char ch;
	conteudo = (char**) malloc(sizeof(char*));
	conteudo[0] = (char*) malloc(sizeof(char) * 251);
	FILE *file;
	file = fopen(nomeDoArquivo, "r");
	if(file == NULL){
		printf("\n\tArquivo %s nao encontrado!\n", nomeDoArquivo);
		exit(1);
	}else{
			for(*numeroDeLinhas=0; (!feof(file)); (*numeroDeLinhas)++){
				conteudo = (char**) realloc(conteudo, (*numeroDeLinhas + 1) * sizeof(char*));
				conteudo[*numeroDeLinhas] = (char*) malloc(sizeof(char) * 251);//assumindo que nenhuma linha terá mais de 250 carcteres
				fgets(conteudo[*numeroDeLinhas], 250, (FILE*) file);
				if(*numeroDeLinhas < 2)// para não pegar o '\n'
					conteudo[*numeroDeLinhas][strlen(conteudo[*numeroDeLinhas]) - 1] = '\0';
			}
	}
	fclose(file);
	return conteudo;
}
void sairDoJogo(Jogo *jogo){
		system(CLS);
		exibirArquivo("templates/tela_final");
		printf("\n");
		free(jogo);
		delay(1500);
		exit(0);
}
char getchPersonalizado(){
	char c = toupper(getch());
	if(c == KEY_H){
		system(CLS);
		exibirArquivo("templates/ajuda");
		printf("\n");
		getch();
	}
	return c;
}

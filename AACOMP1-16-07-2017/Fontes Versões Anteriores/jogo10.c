#include <SDL2/SDL.h>
#include <SDL2/SDL2_rotozoom.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "score.h"

#define MAX 5
#define TAM_TILE 40
#define NUM_LINHAS 15
#define NUM_COLUNAS 19
#define NUM_ZUMBIS 15
#define NUM_TORRES 15
#define VIVO 1
#define MORTO 0
#define ATIVADA 1
#define DESATIVADA 0

// Para compilar: gcc jogo10.c -o jogo10 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm
// Para executar: ./jogo10

// Structs

typedef struct _TIRO
{
	SDL_Rect rect;
	int tiroAtivo;
	double angulo;
	int xVel;
	int yVel;

}TIRO;


typedef struct _TORRE
{
	int estado; //ativada ou desativada
	SDL_Surface *surface;
	SDL_Rect rect;
	TIRO tiros[MAX];
	double angulo;

}TORRE;

typedef struct _ZUMBI
{
	int estado; //morto ou vivo
	SDL_Surface *surface;
	SDL_Rect rect;
	SDL_Rect sprites[2];
	int xVel;
	int yVel;
	int frame;
	int resistencia;

}ZUMBI;

// Var.  globais
SDL_Window  *window;  // --- SDL_Surface* window; mudanças sdl2, agora existe uma SDL_Window dedicada p/ fazer janelas
SDL_Surface *tela;		// de qq forma precisaremos de uma surface p/ tela, mais abaixo usaremos ela
SDL_Surface *fundo;
SDL_Surface *fundoMenu;
SDL_Surface *fundoColocarNome;
SDL_Event event;

// surfaces menu score
SDL_Surface *fundoScore;
SDL_Surface *texto1; // texto nome
SDL_Surface *texto2; // texto pontuação
SDL_Rect rectTexto1; 
SDL_Rect rectTexto2;
TTF_Font *fonte;
SDL_Color corTexto;

SDL_Surface *transformedSurface;

SDL_Surface *selecao;
SDL_Surface *mouse;
SDL_Surface *vidas;
// surface tiles cenario
SDL_Surface *zero;
SDL_Surface *um;
SDL_Surface *dois;
SDL_Surface *tres;
SDL_Surface *quatro;
SDL_Surface *cinco;
SDL_Surface *seis;
SDL_Surface *sete;
SDL_Surface *oito;
SDL_Surface *nove;
SDL_Surface *dez;
SDL_Surface *onze;
SDL_Surface *doze;
SDL_Surface *treze;
SDL_Surface *quatorze;
SDL_Surface *garrinha;
SDL_Surface *desenvolvedores;


SDL_Rect rectSelecao;
SDL_Rect rectMouse;
SDL_Rect rectGarrinha;
SDL_Rect campo[NUM_LINHAS][NUM_COLUNAS];
SDL_Rect rectVidas;

// Var
int mapa[NUM_LINHAS][NUM_COLUNAS];
char *nomeFase;
char nj[30];
int pontuacao = 0;
//int i = 0;
//int loop = 1;
int colocarZumbi = 1;
int tamOrda = 0;
int qtdTorres = 0;

char pb[16]; // buffer para pontuacao
Lista *lista; // lista de scores

ZUMBI z1[NUM_ZUMBIS];
TORRE t1[NUM_TORRES];

// Funções


// inicia sistema de video e etc
void inicializarComponentes()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG);

    //checando inicializacao
    if(TTF_Init() < 0)
    {
        printf("Erro Inicialização sistema fonte TTF_Init %s\n", TTF_GetError());
    }

}

// carrega fonte de texto para menu score
TTF_Font *carregarFonte(const char* arquivo, int tamanhoFonte)
{
    TTF_Font *fonte = TTF_OpenFont(arquivo, tamanhoFonte);
    if(!fonte)
        printf("Problema no carregamento da fonte!! %s\n", TTF_GetError());
    return fonte;
}

void inicializaFonte()
{
    atexit(TTF_Quit);
    //setando fonte e tamanho fonte
    fonte = carregarFonte("Fontes/fonte1.ttf",30);
    // setando cor
    corTexto.r = 139;
    corTexto.g = 0;
    corTexto.b = 0;
}
// carrega window e todas as surfaces utilizadas no jogo e checa se foram encontradas
void carregarSurfaces()
{
	// 880 largura e 640 altura
    window = SDL_CreateWindow("Tower Defense",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,880,640,SDL_WINDOW_OPENGL);
    tela = SDL_GetWindowSurface(window); 
	fundo = IMG_Load("Imagens/fundo2.png");
	fundoMenu = IMG_Load("Imagens/fundoMenu2.png");
	selecao = IMG_Load("Imagens/selecao.png");
	mouse = IMG_Load("Imagens/selecao.png");
	desenvolvedores = IMG_Load("Imagens/desenvolvedores3.png");
	fundoScore = IMG_Load("Imagens/menuRanking3.png");
	fundoColocarNome = IMG_Load("Imagens/fundoColocarNome.png"); 

	for(int i=0;i<NUM_ZUMBIS;i++)
	{
		z1[i].surface = IMG_Load("Imagens/z1.png");
	}

	for(int i=0;i<NUM_TORRES;i++)
	{
		t1[i].surface = IMG_Load("Imagens/canhao_simples_1.png"); 
	}

	// tiles
	zero = IMG_Load("Imagens/0.png");
	um = IMG_Load("Imagens/1.png");
	dois = IMG_Load("Imagens/2.png");
	tres = IMG_Load("Imagens/3.png");
	quatro = IMG_Load("Imagens/4.png");
	cinco = IMG_Load("Imagens/5.png");
	seis = IMG_Load("Imagens/6.png");
	sete = IMG_Load("Imagens/7.png");
	oito = IMG_Load("Imagens/8.png");
	nove = IMG_Load("Imagens/9.png");
	dez = IMG_Load("Imagens/10.png");
	onze = IMG_Load("Imagens/11.png");
	doze = IMG_Load("Imagens/12.png");
	treze = IMG_Load("Imagens/13.png");
	quatorze = IMG_Load("Imagens/14.png");
	garrinha = IMG_Load("Imagens/mao_com_sangue.png");
	vidas = IMG_Load("Imagens/3vidas.png");
	// checo se imagens foram achadas na pasta imagens
	// importante colocar no print qual imagem não foi achada
	// fica mais facil pra achar caso ocorra erro no carregamento

	if(!window)
	{
		printf("Nao foi possivel carregar window\n");
	}

	if(!tela)
	{
		printf("Nao foi possivel carregar surface tela\n");
	}

	if(!fundo)
	{
		printf("Nao foi possível carrega surface fundo\n");
	}

	if(!selecao || !mouse)
	{
		printf("Nao foi possivel carregar imagem selecao\n");
	}
	if (!zero ||!um || !dois || !tres || !quatro||!cinco ||!seis || !sete || !oito|| !nove|| !dez
		|| !onze|| !doze|| !treze || !quatorze)
	{
		printf("Nao foi possivel carregar imagem tiles\n");
	}

	if(!fundoMenu)
	{
		printf("Nao foi possível carrega surface fundo Menu\n");
	}
	if(!garrinha)
	{
        printf("Nao foi possivel carregar imagem garrinha\n");
	}
	if(!desenvolvedores)
	{
        printf("Nao foi possivel carregar imagem desenvolvedores\n");
	}
	if(!fundoScore)
	{
		printf("Nao foi possivel carregar imagem fundoScore\n");
	}
	if(!vidas)
	{
		printf("Nao foi possivel carregar imagem vidas\n");
	}
	if(!fundoColocarNome)
	{
		printf("Nao foi possivel carregar imagem fundoColocarNome\n");
	}

}

void inicializarTiros(TORRE torre)
{
	// inicia o vetor de tiros com 0's em todas posiçoes (tiros nao ativos)
	for(int j = 0;j < MAX;j++)
	{
		torre.tiros[j].tiroAtivo = 0;
		//printf("valor tiro %d\n",torre.tiros[j].tiroAtivo);
	}

}

// funcao que verifica se os tiros estao dentro dos limites da tela
int controlarBlitTiro(SDL_Rect rect)
{
	if(rect.x < 0 || rect.y < 0 || rect.x > 760 || rect.y > 600)
	{
		return 1; // esta fora dos limites

	}else
	{
		return 0; // esta nos limites
	}

}

// funcao que verifica se a torre esta dentro dos limites da tela
void controlarPosicaoTorre(SDL_Rect rect)
{
	if(rect.x < 0)
	{
		printf("Entrei x<0\n");
		rect.x = 0;

	}else if(rect.x > 760)
	{
		rect.x = 760;

	}else if(rect.y < 0)
	{
		rect.y = 0;

	}else if(rect.y > 600)
	{
		rect.y = 600;
	}
}

void carregarFase(char *nomeArquivo)
{

	FILE *arquivo = fopen(nomeArquivo,"r");

    if(arquivo == NULL)
    {
        printf("Erro no carregamento do txt \n.");
        exit(1);
    }

    // mapa[15][19] // 15 altura e 19 largura
    // para saber o numero de linhas da matriz(tilemap)
    // divido a altura da tela(600) pelo tam da tile que eu desejo ter no jogo
    // no caso quero(40), entao 600/40 = 19
    // numero de colunas (760/40) = 15

    //lendo arquivo txt e jogando os valores na matriz mapa

    for(int i=0; i<NUM_LINHAS; i++)
    {
        for(int j=0; j<NUM_COLUNAS; j++)
        {

            fscanf(arquivo,"%d",&mapa[i][j]);
            //printf("%d\n",mapa[i][j]);

        }
    }

    fclose(arquivo);

}

void blitarMapa()
{
	 // blitando o mapa de azulejos
    for(int i=0; i<NUM_LINHAS; i++)
    {
        for(int j=0; j<NUM_COLUNAS; j++)
        {

            campo[i][j].x = j*TAM_TILE;
            campo[i][j].y = i*TAM_TILE;
            campo[i][j].w = TAM_TILE;
            campo[i][j].h = TAM_TILE;

            if(mapa[i][j] == 0)
            {
                SDL_BlitSurface(zero,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 1)
            {
                SDL_BlitSurface(um,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 2)
            {
                SDL_BlitSurface(dois,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 3)
            {
                SDL_BlitSurface(tres,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 4)
            {
                SDL_BlitSurface(quatro,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 5)
            {
                SDL_BlitSurface(cinco,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 6)
            {
                SDL_BlitSurface(seis,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 7)
            {
                SDL_BlitSurface(sete,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 8)
            {
                SDL_BlitSurface(oito,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 9)
            {
                SDL_BlitSurface(nove,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 10)
            {
                SDL_BlitSurface(dez,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 11)
            {
                SDL_BlitSurface(onze,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 12)
            {
                SDL_BlitSurface(doze,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 13)
            {
                SDL_BlitSurface(treze,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == 14)
            {
                SDL_BlitSurface(quatorze,NULL,tela,&campo[i][j]);
            }

        }
    }
}

// quando usuario escolher a fase, o nome de um txt será
// associado ao nome do arquivo txt que será lido para carregar a fase na funcao carregarFase()
void escolherFase(int opcao)
{

	switch(opcao)
	{
		case 1:
			nomeFase = "Fases/matriz.txt";
		break;

		default:
			break;
	}
}

void inicializarEstadoZumbis()
{
	for (int i = 0; i < NUM_ZUMBIS; i++)
	{
		z1[i].estado = MORTO;
		
	}

}

// prepara qtd de zumbis p/ uma orda especifica
void inicializarZumbis(int i)
{

	z1[i].rect.x = 0; 
	z1[i].rect.y = 80;
	z1[i].estado = VIVO;

}

void inicializarEstadoTorre()
{
	for (int i = 0; i < NUM_TORRES; i++)
	{
		t1[i].estado = DESATIVADA;	
	}

}

void inicializarTorres(int i, int x,int y)
{
	t1[i].rect.x = x;
	t1[i].rect.y = y;
	t1[i].estado = ATIVADA;
}
// descobrir pq só funciona com ponteiro pq??????
// essa funcao anda com apenas 1 zumbi
void andarZumbi1(SDL_Rect *rectZumbi)
{
	if(rectZumbi->x < 160)
	{
		rectZumbi->x++;
	}
	else if(rectZumbi->x == 160 && rectZumbi->y != 360)
	{

		rectZumbi->y++;

	}else if(rectZumbi->y == 360 && rectZumbi->x != 400 && rectZumbi->x != 600)
	{
		rectZumbi->x++;

	}else if(rectZumbi->x == 400 && rectZumbi->y != 120)
	{
		rectZumbi->y--;

	}else if(rectZumbi->y == 120 && rectZumbi->x != 600)
	{
		rectZumbi->x++;

	}
	else if(rectZumbi->x == 600 && rectZumbi->y != 480)
	{
		rectZumbi->y++;
	}
	else if(rectZumbi->y == 480 && rectZumbi->x != 720)
	{
		rectZumbi->x++;

	}else if(rectZumbi->x == 720)
	{
		rectZumbi->x = 0;
		rectZumbi->y = 80;
	}
}

void andarZumbi2()
{
	for (int i = 0; i < tamOrda; i++)
	{

		if(z1[i].estado == VIVO)
		{
			if(z1[i].rect.x < 165)
			{
				z1[i].surface = IMG_Load("Imagens/z1lado.png");

				z1[i].rect.x++;
			}
			else if(z1[i].rect.x == 165 && z1[i].rect.y != 360)
			{
				z1[i].surface = IMG_Load("Imagens/z1descendo.png");
				z1[i].rect.y++;

			}else if(z1[i].rect.y == 360 && z1[i].rect.x != 405 && z1[i].rect.x != 605)
			{
				z1[i].surface = IMG_Load("Imagens/z1lado.png");
				z1[i].rect.x++;

			}else if(z1[i].rect.x == 405 && z1[i].rect.y != 118)
			{
				z1[i].surface = IMG_Load("Imagens/z1subindo.png");
				z1[i].rect.y--;

			}else if(z1[i].rect.y == 118 && z1[i].rect.x != 605)
			{
				z1[i].surface = IMG_Load("Imagens/z1lado.png");
				z1[i].rect.x++;

			}
			else if(z1[i].rect.x == 605 && z1[i].rect.y != 480)
			{
				z1[i].surface = IMG_Load("Imagens/z1descendo.png");
				z1[i].rect.y++;
			}
			else if(z1[i].rect.y == 480 && z1[i].rect.x != 720)
			{
				z1[i].surface = IMG_Load("Imagens/z1lado.png");
				z1[i].rect.x++;

			}else if(z1[i].rect.x == 720)
			{
				z1[i].rect.x = 0;
				z1[i].rect.y = 80;
			}
		}
	}

}


void controlarFrame()
{
	for (int i = 0; i < tamOrda; i++)
	{
		z1[i].frame++;

    	if(z1[i].frame == 2)
   		{
        	z1[i].frame = 0;
    	}
	}

}

void blitarZumbis()
{
	for (int i = 0; i < tamOrda; i++)
	{
		if(z1[i].estado == VIVO)
			SDL_BlitSurface(z1[i].surface,&z1[i].sprites[z1[i].frame], tela, &z1[i].rect);
	}
}

void blitarTorres()
{
	for (int i = 0; i < qtdTorres; i++)
	{
		if(t1[i].estado == ATIVADA)
			SDL_BlitSurface(t1[i].surface,NULL, tela, &t1[i].rect);
	}
}

void carregarSprite()
{

   for (int i = 0; i < NUM_ZUMBIS; i++)
	{
		z1[i].sprites[0].x = 7;
    	z1[i].sprites[0].y = 0;
    	z1[i].sprites[0].w = 40;
    	z1[i].sprites[0].h = 80;

    	z1[i].sprites[1].x = 47;
    	z1[i].sprites[1].y = 0;
    	z1[i].sprites[1].w = 40;
    	z1[i].sprites[1].h = 80;

    	z1[i].frame = 0;
    }
}
// criar vetor de torres para colocar mais torres no jogo
// funcao exibe recordes do jogo na tela
int recorde()
{
    rectTexto1.x = 240;
    rectTexto1.y = 210;
    rectTexto1.w = 0;
    rectTexto1.h = 0;

    rectTexto2.x = 550;
    rectTexto2.y = 210;
    rectTexto2.w = 0;
    rectTexto2.h = 0;

    SDL_BlitSurface(fundoScore,NULL,tela,NULL);

    No *paux = lista->inicio;
    

    while(paux != NULL)
    {

        sprintf(pb, "%d", paux->jogador->pontuacao);
        //printf("%s\n",paux->jogador->nome);
        //printf("%d\n",paux->jogador->pontuacao);
        texto1 = TTF_RenderText_Solid(fonte,paux->jogador->nome,corTexto);
        texto2 = TTF_RenderText_Solid(fonte,pb,corTexto);

        SDL_BlitSurface(texto1, NULL, tela, &rectTexto1);
        SDL_BlitSurface(texto2, NULL, tela, &rectTexto2);
        SDL_UpdateWindowSurface(window);

        rectTexto1.y = rectTexto1.y + 40;
        rectTexto2.y = rectTexto2.y + 40;

        paux = paux->prox;  
    }


    int loop = 0; // Variável de controle do loop
    while (loop == 0) // Loop principal
    {
        // Lê a fila de eventos e põe o evento mais antigo em "event"
        while (SDL_PollEvent(&event)) // Loop de eventos
        {

            // Verifica se o evento mais antigo é do tipo SDL_QUIT
            if (event.type == SDL_QUIT) // Se o usuário clicou para fechar a janela
            {
            	 return 0;

            }else if(event.type == SDL_MOUSEBUTTONDOWN)
            {
                if((event.button.x >= 100 && event.button.x <= 175) && (event.button.y >= 100 && event.button.y <= 140))
                {
                    //printf("Voltar para menu!\n");
                    return 1;
                }
                
            } 
        }
    	
    	//SDL_BlitSurface(fundoScore,NULL,tela,0);
        SDL_UpdateWindowSurface(window);
    }
}

int inserirNome()
{
	int indice = 0;
	char mensagem[] = "Digite seu nome:";

	SDL_Surface *ttfMensagem;
	SDL_Surface *ttfNome;
	SDL_Surface *ttfTraco;
	SDL_Rect posicaoMsg;
	SDL_Rect posicao;
	char nomeJogador[30];

	posicaoMsg.x = 300;
	posicaoMsg.y = 200;
	posicao.x = 290;
	posicao.y = 300;

	for (int x = 0; x < 30; ++x)
	{
		nomeJogador[x] = ' '; 
	}

	int loop = 0;

    while(loop == 0)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                return 0;
            }
            else if( event.type == SDL_KEYDOWN )
			{
				switch( event.key.keysym.sym )
				{
					case SDLK_ESCAPE:
						return 0;
					break;
					
					case SDLK_a:
						if(indice<30)
						{
							nomeJogador[indice] = 'a';
							indice++;
						}
						break;
					case SDLK_b:
						if(indice<30)
						{
							nomeJogador[indice] = 'b';
							indice++;
						}
						break;
					case SDLK_c:
						if(indice<30)
						{
							nomeJogador[indice] = 'c';
							indice++;
						}
						break;
					case SDLK_d:
						if(indice<30)
						{
							nomeJogador[indice] = 'd';
							indice++;
						}
						break;
					case SDLK_e:
						if(indice<30)
						{
							nomeJogador[indice] = 'e';
							indice++;
						}
						break;
					case SDLK_f:
						if(indice<30)
						{
							nomeJogador[indice] = 'f';
							indice++;
						}
						break;
					case SDLK_g:
						if(indice<30)
						{
							nomeJogador[indice] = 'g';
							indice++;
						}
						break;
					case SDLK_h:
						if(indice<30)
						{
							nomeJogador[indice] = 'h';
							indice++;
						}
						break;
					case SDLK_i:
						if(indice<30)
						{
							nomeJogador[indice] = 'i';
							indice++;
						}
						break;
					case SDLK_j:
						if(indice<30)
						{
							nomeJogador[indice] = 'j';
							indice++;
						}
						break;
					case SDLK_k:
						if(indice<30)
						{
							nomeJogador[indice] = 'k';
							indice++;
						}
						break;
					case SDLK_l:
						if(indice<30)
						{
							nomeJogador[indice] = 'l';
							indice++;
						}
						break;
					case SDLK_m:
						if(indice<30)
						{
							nomeJogador[indice] = 'm';
							indice++;
						}
						break;
					case SDLK_n:
						if(indice<30)
						{
							nomeJogador[indice] = 'n';
							indice++;
						}
						break;
					case SDLK_o:
						if(indice<30)
						{
							nomeJogador[indice] = 'o';
							indice++;
						}
						break;
					case SDLK_p:
						if(indice<30)
						{
							nomeJogador[indice] = 'p';
							indice++;
						}
						break;
					case SDLK_q:
						if(indice<30)
						{
							nomeJogador[indice] = 'q';
							indice++;
						}
						break;
					case SDLK_r:
						if(indice<30)
						{
							nomeJogador[indice] = 'r';
							indice++;
						}
						break;
					case SDLK_s:
						if(indice<30)
						{
							nomeJogador[indice] = 's';
							indice++;
						}
						break;
					case SDLK_t:
						if(indice<30)
						{
							nomeJogador[indice] = 't';
							indice++;
						}
						break;
					case SDLK_u:
						if(indice<30)
						{
							nomeJogador[indice] = 'u';
							indice++;
						}
						break;
					case SDLK_v:
						if(indice<30)
						{
							nomeJogador[indice] = 'v';
							indice++;
						}
						break;
					case SDLK_w:
						if(indice<30)
						{
							nomeJogador[indice] = 'w';
							indice++;
						}
						break;
					case SDLK_x:
						if(indice<30)
						{
							nomeJogador[indice] = 'x';
							indice++;
						}
						break;
					case SDLK_y:
						if(indice<30)
						{
							nomeJogador[indice] = 'y';
							indice++;
						}
						break;
					case SDLK_z:
						if(indice<30)
						{
							nomeJogador[indice] = 'z';
							indice++;
						}
						break;

					case SDLK_1:
						if(indice<30)
						{
							nomeJogador[indice] = '1';
							indice++;
						}
						break;

					case SDLK_2:
						if(indice<30)
						{
							nomeJogador[indice] = '2';
							indice++;
						}
						break;

					case SDLK_3:
						if(indice<30)
						{
							nomeJogador[indice] = '3';
							indice++;
						}
						break;

					case SDLK_4:
						if(indice<30)
						{
							nomeJogador[indice] = '4';
							indice++;
						}
						break;

					case SDLK_5:
						if(indice<30)
						{
							nomeJogador[indice] = '5';
							indice++;
						}
						break;

					case SDLK_6:
						if(indice<30)
						{
							nomeJogador[indice] = '6';
							indice++;
						}
						break;

					case SDLK_7:
						if(indice<30)
						{
							nomeJogador[indice] = '7';
							indice++;
						}
						break;

					case SDLK_8:
						if(indice<30)
						{
							nomeJogador[indice] = '8';
							indice++;
						}
						break;

					case SDLK_9:
						if(indice<30)
						{
							nomeJogador[indice] = '9';
							indice++;
						}
						break;

					case SDLK_0:
						if(indice<30)
						{
							nomeJogador[indice] = '0';
							indice++;
						}
						break;

					case SDLK_BACKSPACE:
						// apagando o que foi digitado
						if(indice>0)
						{
							indice--;
							nomeJogador[indice] = ' ';
						}
						break;
					default:
						break;
				}
			}
            else if(event.type == SDL_MOUSEBUTTONDOWN)
            {
            	if((event.button.x >= 700 && event.button.x <= 760) && (event.button.y >= 530 && event.button.y <= 570))
            	{
                    //começar
                    strcpy(nj,nomeJogador); // copia string temporaria p/ string global que será usada mais tarde 
                    return 1;
            	}
            }

        }

        SDL_UpdateWindowSurface(window);
        SDL_BlitSurface(fundoColocarNome,NULL,tela,NULL);

        ttfMensagem = TTF_RenderText_Blended(fonte, mensagem, corTexto);
        ttfNome = TTF_RenderText_Blended(fonte, nomeJogador, corTexto);
		ttfTraco = TTF_RenderText_Blended(fonte, "____________________", corTexto);
		SDL_BlitSurface( ttfMensagem, NULL, tela, &posicaoMsg);
		SDL_BlitSurface( ttfNome, NULL,tela, &posicao);
		SDL_BlitSurface( ttfTraco, NULL,tela, &posicao);

    }

	//return 0;
}

int jogo()
{

	int loop = 0;
	tamOrda = 5;
	int i=0;
	int j=0;
	carregarSprite();
	inicializarEstadoZumbis();
	inicializarEstadoTorre();

	int colocarOrda = 0;
	int pausaOrda = 0;
	int ativarSelecao = 0;
	qtdTorres = 5;
	int colocarTorre = FALSE;

	rectVidas.x = 30;
	rectVidas.y = 600; 

	while(loop == 0)
	{
		if(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				return 0;

			}else if(event.type == SDL_MOUSEMOTION)
            {
            	//printf("X:%d\n",event.motion.x);
            	//printf("Y:%d\n",event.motion.y);
            	/*
            	if(ativarSelecao)
            	{
            		rectMouse.x = event.motion.x;
            		rectMouse.y = event.motion.y;
            		
            	}
            	*/

            }else if(event.type == SDL_MOUSEBUTTONDOWN)
            {

            	if((event.button.x >= 779 && event.button.x <= 858) && (event.button.y >= 30 && event.button.y <= 70))
            	{
            		// começar

                    colocarOrda = 1;
            	}else if((event.button.x >= 779 && event.button.x <= 858) && (event.button.y >= 79 && event.button.y <= 119))
            	{
            		//pausar
            		colocarOrda = 0;
                    pausaOrda = 1;

            	}else if((event.button.x >= 779 && event.button.x <= 858) && (event.button.y >= 129 && event.button.y <= 179))
            	{
            		//sair
            		Jogador *j1 = criarJogador(nj,60);
					inserirOrdenado(lista,j1);
					

            		loop = 1;

            	}else if((event.button.x >= 770 && event.button.x <= 810) && (event.button.y >= 210 && event.button.y <= 240))
            	{
            		rectSelecao.x = 770;
            		rectSelecao.y = 200;
            		ativarSelecao = TRUE;
            		
            	}else if((event.button.x >= 0 && event.button.x <= 760) && (event.button.y >= 0 && event.button.y <= 600) && (ativarSelecao == TRUE))
            	{
            		/*
            		colocarTorre = TRUE;

					if(j<qtdTorres)
					{
						inicializarTorres(j,(int)rectMouse.x,(int)rectMouse.x);
						printf("Entrei no if inicializarTorres\n");
            			j++;
					}

					ativarSelecao = FALSE;
					*/
            	}

            }
			else if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
                        return 0;
				}
			}
		}

		SDL_BlitSurface(fundo, NULL, tela, NULL);

		if(i<tamOrda && colocarOrda)
		{
			if(i == 0)
			{
				inicializarZumbis(i);
				i++;
			}
			else if(z1[i-1].rect.x == 25)
			{
				inicializarZumbis(i);
				i++;
			}

		}


		blitarMapa();

		//if(colocarTorre == TRUE)
			//blitarTorres();

		if(pausaOrda)
		{
			blitarZumbis();
		}

		if(colocarOrda)
		{
			andarZumbi2();
      		controlarFrame();
      		blitarZumbis();
		}

	
		if(ativarSelecao)
		{
			SDL_BlitSurface(selecao,NULL,tela,&rectSelecao);
			//SDL_BlitSurface(mouse,NULL,tela,&rectMouse);
		}

		SDL_BlitSurface(vidas,NULL,tela,&rectVidas);
		SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela

	}

	return 1;

}

int developers()
{
    int loop = 0;

    while(loop == 0)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                return 0;
            }
            else if(event.type == SDL_MOUSEBUTTONDOWN)
            {
            	if((event.button.x >= 700 && event.button.x <= 780) && (event.button.y >= 530 && event.button.y <= 570))
            	{
                    //printf("Cliquei em voltar\n");
                    return 1;
            	}
            }

        }

        SDL_UpdateWindowSurface(window);
        SDL_BlitSurface(desenvolvedores,NULL,tela,NULL);
    }
}

// Main
int main(void)
{
	inicializarComponentes();
    inicializaFonte();
    carregarSurfaces();
    escolherFase(1);
    carregarFase(nomeFase);

    // lista para score
    lista = criarLista();
    carregarLista("ranking.txt",lista);

    int loop = 0;
    int ativarGarrinha = 0;

    while (loop == 0) // Loop principal
    {
      // Lê a fila de eventos e põe o evento mais antigo em "event"
        while (SDL_PollEvent(&event)) // Loop de eventos
        {

            // Verifica se o evento mais antigo é do tipo SDL_QUIT
            if(event.type == SDL_QUIT) // Se o usuário clicou para fechar a janela
            {
                loop = 1; // Encerre o loop
            }

            else if(event.type == SDL_MOUSEMOTION)
            {
            	/*printf("X:%d\n",event.motion.x);
            	printf("Y:%d\n",event.motion.y);
            	*/

            	if((event.motion.x >= 535 && event.motion.x <= 670) && (event.motion.y >= 240 && event.motion.y <= 290))
            	{
            		//printf("mouse em cima do icone jogar\n");

            		ativarGarrinha = 1;
            		rectGarrinha.x = 465;
            		rectGarrinha.y = 225;
            	}

            	else if((event.motion.x >= 535 && event.motion.x <= 741) && (event.motion.y >= 316 && event.motion.y <= 365))
            	{
                    //printf("Cliquei nas instrucoes\n");
                    ativarGarrinha = 1;
            		rectGarrinha.x = 465;
            		rectGarrinha.y = 300;
            	}

            	else if((event.motion.x >= 535 && event.motion.x <= 844) && (event.motion.y >= 390 && event.motion.y <= 438))
            	{
                    //printf("Cliquei nos desenvolvedores\n");
                    ativarGarrinha = 1;
            		rectGarrinha.x = 465;
            		rectGarrinha.y = 375;
            	}
            	else if((event.motion.x >= 535 && event.motion.x <= 693) && (event.motion.y >= 470 && event.motion.y <= 518))
            	{
                    //printf("Cliquei no recorde\n");
                    ativarGarrinha = 1;
            		rectGarrinha.x = 465;
            		rectGarrinha.y = 455;
            	}
            	else if((event.motion.x >= 535 && event.motion.x <= 613) && (event.motion.y >= 559 && event.motion.y <= 600))
            	{
                    //printf("Cliquei em sair\n");
                    ativarGarrinha = 1;
            		rectGarrinha.x = 465;
            		rectGarrinha.y = 540;
            	}else
            	{
                    ativarGarrinha = 0;
            	}
            }

            else if(event.type == SDL_MOUSEBUTTONDOWN)
            {

            	if((event.button.x >= 535 && event.button.x <= 670) && (event.button.y >= 240 && event.button.y <= 290))
            	{
            		//printf("Cliquei no icone iniciar\n");
            		if(!inserirNome())
            		{
            			loop = 1;

            		}else if(!jogo())
                    {
                        loop = 1;
                    }
            	}

            	else if((event.button.x >= 535 && event.button.x <= 741) && (event.button.y >= 316 && event.button.y <= 365))
            	{
                    //printf("Cliquei nas instrucoes\n");
            	}

            	else if((event.button.x >= 535 && event.button.x <= 844) && (event.button.y >= 390 && event.button.y <= 438))
            	{
                    //printf("Cliquei nos desenvolvedores\n");
                    if(!developers())
                    {
                        loop = 1;
                    }
            	}
            	else if((event.button.x >= 535 && event.button.x <= 693) && (event.button.y >= 470 && event.button.y <= 518))
            	{
                    //printf("Cliquei no recorde\n");
                    if(!recorde())
                    {
                        loop = 1;
                    }
            	}
            	else if((event.button.x >= 535 && event.button.x <= 613) && (event.button.y >= 559 && event.button.y <= 600))
            	{
                    //printf("Cliquei em sair\n");
                    loop = 1;
            	}

           	}
        }


        SDL_UpdateWindowSurface(window); // esta linha é essencial sem ela a imagem de fundo nao aparece na tela
        SDL_BlitSurface(fundoMenu,NULL,tela,NULL); // Blita a imagem em dest

        if(ativarGarrinha)
        {
            SDL_BlitSurface(garrinha, NULL, tela, &rectGarrinha);
        }

    }

    // guarda scores no txt
    guardarLista("ranking.txt",lista);
    // apaga lista encadeada com os scores
    apagarLista(lista);

	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit(); // Fecha o SDL
  	return 0;

}

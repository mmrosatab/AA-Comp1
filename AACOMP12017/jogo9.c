#include <SDL2/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "score.h"

#define MAX 5
#define MENU 0
#define INICIAR 1
#define INSTRUCOES 2
#define RECORDES 3
#define CREDITOS 4
#define SAIR 5
#define ABERTURA 6
#define TAM_TILE 40
#define NUM_LINHAS 15
#define NUM_COLUNAS 19
#define CAMINHO_TORRE 0
#define CAMINHO_ZUMBI 1
#define NUM_ZUMBIS 15
#define VIVO 1
#define MORTO 0


// Para compilar: gcc jogo4.c -o jogo4 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm
// Para executar: ./jogo4

// Var.  globais


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
	SDL_Rect rect;
	TIRO tiros[MAX];
	double angulo;

}TORRE;

typedef struct _ZUMBI
{	
	int estado; //morto ou vivo
	SDL_Rect rect;
	SDL_Rect sprites[2];
	int xVel;
	int yVel;
	int frame;

}ZUMBI;


SDL_Window *window;  // --- SDL_Surface* window; mudanças sdl2, agora existe uma SDL_Window dedicada p/ fazer janelas
SDL_Surface *tela;		// de qq forma precisaremos de uma surface p/ tela, mais abaixo usaremos ela
SDL_Surface *fundo;
SDL_Event event;

SDL_Surface *torre;
SDL_Surface *tiro;
SDL_Surface *transformedSurface;

SDL_Surface *selecao;
SDL_Surface *zumbi[NUM_ZUMBIS];

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


SDL_Rect rectSelecao;
SDL_Rect campo[NUM_LINHAS][NUM_COLUNAS];


// Var

int mapa[NUM_LINHAS][NUM_COLUNAS];
char *nomeFase;
//int i = 0;
//int loop = 1;
int colocarZumbi = 1;
int tamOrda = 0;

ZUMBI z2[NUM_ZUMBIS];

// Funções


// inicia sistema de video e etc
void inicializarComponentes()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG);

}

// carrega window e todas as surfaces utilizadas no jogo e checa se foram encontradas
void carregarSurfaces()
{
	// 880 largura e 640 altura
    window = SDL_CreateWindow("Tower Defense",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,880,640,SDL_WINDOW_OPENGL);
    tela = SDL_GetWindowSurface(window); //
	int i;
	torre = IMG_Load("Imagens/torre.png");
	fundo = IMG_Load("Imagens/fundo.png");
	tiro = IMG_Load("Imagens/tiro.png");
	selecao = IMG_Load("Imagens/selecao.png");
	/*
	for(i=0;i<NUM_ZUMBIS;i++)
	{
		zumbi[i] = IMG_Load("Imagens/z1.png");
	}
	*/
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


	// checo se imagens foram achadas na pasta imagens
	// importante colocar no print qual imagem não foi achada
	// fica mais facil pra achar caso ocorra erro

	if(!window)
	{
		printf("Nao foi possivel carregar window\n");
	}

	if(!tela)
	{
		printf("Nao foi possivel carregar surface tela\n");
	}

	if(!torre)
	{
		printf("Nao foi possível carrega surface torre\n");
	}

	if(!fundo)
	{
		printf("Nao foi possível carrega surface fundo\n");

	}
	
	if(!selecao)
	{
		printf("Nao foi possivel carregar imagem selecao\n");
	}
	if(!zumbi)
	{
		printf("Nao foi possivel carregar imagem zumbi\n");
	}
	if (!zero ||!um || !dois || !tres || !quatro||!cinco ||!seis || !sete || !oito|| !nove|| !dez 
		|| !onze|| !doze|| !treze || !quatorze)
	{
		printf("Nao foi possivel carregar imagem tiles\n");
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
		z2[i].estado = MORTO;
		zumbi[i] = IMG_Load("Imagens/z1.png");
	}

}

// prepara qtd de zumbis p/ uma orda especifica
void inicializarZumbis(int i)
{
	
	//for (int i = 0; i < quant; i++)
	//{
		z2[i].rect.x = 0; // 20 é o espacamento entre os zumbis 
		z2[i].rect.y = 80;
		z2[i].estado = VIVO;
	//}
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

		if(z2[i].estado == VIVO)
		{
			if(z2[i].rect.x < 165)
			{
				zumbi[i] = IMG_Load("Imagens/z1lado.png");

				z2[i].rect.x++;
			}
			else if(z2[i].rect.x == 165 && z2[i].rect.y != 360)
			{
				zumbi[i] = IMG_Load("Imagens/z1descendo.png");
				z2[i].rect.y++;

			}else if(z2[i].rect.y == 360 && z2[i].rect.x != 405 && z2[i].rect.x != 605)
			{
				zumbi[i] = IMG_Load("Imagens/z1lado.png");
				z2[i].rect.x++;

			}else if(z2[i].rect.x == 405 && z2[i].rect.y != 118)
			{
				zumbi[i] = IMG_Load("Imagens/z1subindo.png");
				z2[i].rect.y--;

			}else if(z2[i].rect.y == 118 && z2[i].rect.x != 605)
			{
				zumbi[i] = IMG_Load("Imagens/z1lado.png");
				z2[i].rect.x++;

			}
			else if(z2[i].rect.x == 605 && z2[i].rect.y != 480)
			{
				zumbi[i] = IMG_Load("Imagens/z1descendo.png");
				z2[i].rect.y++;
			}
			else if(z2[i].rect.y == 480 && z2[i].rect.x != 720)
			{
				zumbi[i] = IMG_Load("Imagens/z1lado.png");
				z2[i].rect.x++;

			}else if(z2[i].rect.x == 720) 
			{
				z2[i].rect.x = 0;
				z2[i].rect.y = 80;
			}	
		}
	}

}


void controlarFrame()
{
	for (int i = 0; i < tamOrda; i++)
	{
		z2[i].frame++; 

    	if(z2[i].frame == 2)
   		{
        	z2[i].frame = 0;
    	}
	}

}

void blitarZumbis()
{
	for (int i = 0; i < tamOrda; i++)
	{
		if(z2[i].estado == VIVO)
			SDL_BlitSurface(zumbi[i],&z2[i].sprites[z2[i].frame], tela, &z2[i].rect);
	}
}

void carregarSprite()
{
	
   for (int i = 0; i < NUM_ZUMBIS; i++)
	{
		z2[i].sprites[0].x = 7;
    	z2[i].sprites[0].y = 0;
    	z2[i].sprites[0].w = 40;
    	z2[i].sprites[0].h = 80;

    	z2[i].sprites[1].x = 47;
    	z2[i].sprites[1].y = 0;
    	z2[i].sprites[1].w = 40;
    	z2[i].sprites[1].h = 80;

    	z2[i].frame = 0;
    }
}
// criar vetor de torres para colocar mais torres no jogo

int jogo()
{

	int loop = 1;
	tamOrda = 15;
	int i=0;
	carregarSprite();
	inicializarEstadoZumbis();
	//inicializarZumbis(tamOrda);

	while(loop)
	{
		if(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{	
				return 0;
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
	
		if(i<tamOrda)
		{
			if(i == 0)
			{
				inicializarZumbis(i);
				i++;
			}
			else if(z2[i-1].rect.x == 25)
			{
				inicializarZumbis(i);
				i++;
			}
			
		}	
	

		blitarMapa();
      	andarZumbi2();
      	controlarFrame();
      	blitarZumbis();
      	
		SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela
	}

}

// Main
int main(void)
{
	inicializarComponentes();
    carregarSurfaces();
    escolherFase(1);
    carregarFase(nomeFase);

    jogo();

	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit(); // Fecha o SDL
  	return 0;

}

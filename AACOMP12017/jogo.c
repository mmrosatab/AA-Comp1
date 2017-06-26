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


// Para compilar: gcc jogo.c -o jogo -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm

// Var.  globais

SDL_Window *window;  // --- SDL_Surface* window; mudanças sdl2, agora existe uma SDL_Window dedicada p/ fazer janelas
SDL_Surface *tela;		// de qq forma precisaremos de uma surface p/ tela, mais abaixo usaremos ela
SDL_Surface *fundo;
SDL_Event event;

SDL_Surface *torre;
SDL_Surface *tiro;
SDL_Surface *transformedSurface;
//SDL_Surface *livre;

int mapa[15][19];
char *nomeFase;
int i = 0;
int atirar = 0;

int loop = 1;

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
	// sdl 2 ---
    window = SDL_CreateWindow("Tower Defense",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,760,600,SDL_WINDOW_OPENGL);
    tela = SDL_GetWindowSurface(window); //

	torre = IMG_Load("Imagens/torre.png");
	fundo = IMG_Load("Imagens/fundo.png");
	tiro = IMG_Load("Imagens/tiro.png");

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
int controlarBlitTiro2(SDL_Rect rect)
{
   if((rect.y > 0) && (rect.y < 600) && (rect.x > 0) && (rect.x < 760))
   {
      return 1;
   }
   else
   {
      return 0;
   }

}

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
void carregarFase(char const nomeArquivo[])
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
    int i,j;

    for(i=0; i<15; i++)
    {
        for(j=0; j<19; j++)
        {

            fscanf(arquivo,"%d",&mapa[i][j]);

        }
    }

    fclose(arquivo);

}

// quando usuario escolher a fase, o nome de um txt será
// associado ao nome do arquivo txt que será lido para carregar a fase na funcao carregarFase()
void escolherFase(int opcao)
{

	switch(opcao)
	{
		case 1:
			nomeFase = "Fase/fase1.txt";
		break;

		default:
			break;
	}
}

// Main

int main(void)
{
	inicializarComponentes();
    carregarSurfaces();


    //int j = 0;
	TORRE torre1;  // em breve será vetor de torres

	SDL_Rect rect;

	torre1.angulo = 0;
	torre1.rect.x = tela->w/2;
	torre1.rect.y = tela->h/2;

	// inicia o vetor de tiros com 0's em todas posiçoes (tiros nao ativos)
	inicializarTiros(torre1);


	while(loop)
	{
		if(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
				loop = 0;

			if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{

					case SDLK_UP:    torre1.rect.y -= 10; break;
					case SDLK_DOWN:  torre1.rect.y += 10; break;
					case SDLK_LEFT:  torre1.rect.x -= 10; break;
					case SDLK_RIGHT: torre1.rect.x += 10; break;


					case SDLK_SPACE:

						// apertou tecla space, tiro receberá posição e angulo da torre

						if (i == MAX) // controle número de tiros
						{
                     		i = 0;
						}
						
						torre1.tiros[i].tiroAtivo = 1;
						torre1.tiros[i].rect.x = rect.x;
						torre1.tiros[i].rect.y = rect.y;
						torre1.tiros[i].angulo = torre1.angulo;
						
						i++;

						break;

					case SDLK_r: torre1.angulo += 5; break;
					case SDLK_t: torre1.angulo -= 5; break;

					default: break;
				}
			}
		}

		SDL_BlitSurface(fundo, NULL, tela, NULL);

		transformedSurface = rotozoomSurface(torre,torre1.angulo,1.0,SMOOTHING_ON);
		SDL_Surface *tiroT;

    	// rotacionando sobre o proprio eixo
    	rect.x = torre1.rect.x;
    	rect.y = torre1.rect.y;

    	rect.x -= transformedSurface->w/2 - torre->w/2;
    	rect.y -= transformedSurface->h/2 - torre->h/2;

    	// tiro

      	for(int j = 0;j < MAX;j++)
      	{
         	if(torre1.tiros[j].tiroAtivo == 1)
         	{
         		//printf("valor de j %d\n",j);

            	tiroT = rotozoomSurface(tiro,torre1.tiros[j].angulo,1.0,SMOOTHING_ON);
            	torre1.tiros[j].rect.x -= sin(torre1.tiros[j].angulo*M_PI/180.0)*5.0;
            	torre1.tiros[j].rect.y -= cos(torre1.tiros[j].angulo*M_PI/180.0)*5.0;

            	//printf("pontos no x: %d\n",torre1.tiros[j].rect.x);
            	//printf("pontos no y: %d\n",torre1.tiros[j].rect.y);

               	//if (torre1.tiros[j].rect.x < 0 || torre1.tiros[j].rect.y < 0 || torre1.tiros[j].rect.x > 760 || torre1.tiros[j].rect.y > 600)
				if(controlarBlitTiro(torre1.tiros[j].rect))
					torre1.tiros[j].tiroAtivo = 0;
				else
					SDL_BlitSurface(tiroT, NULL, tela, &torre1.tiros[j].rect);
            
         	}

      	}

      	inicializarTiros(torre1);


		SDL_BlitSurface(transformedSurface, NULL, tela, &rect);
		SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela

		SDL_FreeSurface(transformedSurface);

	}

	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit(); // Fecha o SDL
  	return 0;

}

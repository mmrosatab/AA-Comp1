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

#define CAMINHO_TORRE 0
#define CAMINHO_SOLDADO 1


// Para compilar: gcc jogo2.c -o jogo2 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm

// Var.  globais

SDL_Window *window;  // --- SDL_Surface* window; mudanças sdl2, agora existe uma SDL_Window dedicada p/ fazer janelas
SDL_Surface *tela;		// de qq forma precisaremos de uma surface p/ tela, mais abaixo usaremos ela
SDL_Surface *fundo;
SDL_Event event;

SDL_Surface *torre;
SDL_Surface *tiro;
SDL_Surface *transformedSurface;
SDL_Surface *caminhoSoldado;
SDL_Surface *caminhoTorre;
SDL_Surface *selecao;
SDL_Surface *soldado;

SDL_Rect rectSelecao;
SDL_Rect campo[15][19];
SDL_Rect rectSoldado;

int mapa[15][19];
char *nomeFase;
int i = 0;
int loop = 1;
int colocarTorre = 0;

// Structs

typedef struct _TIRO
{
	SDL_Surface *surface;
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

	torre = IMG_Load("Imagens/torre1.png");
	fundo = IMG_Load("Imagens/fundo1.png");
	tiro = IMG_Load("Imagens/tiro1.png");
	caminhoSoldado = IMG_Load("Imagens/caminhoSoldado.png");
	caminhoTorre = IMG_Load("Imagens/caminhoTorre.png");
	selecao = IMG_Load("Imagens/selecao.png");
	soldado = IMG_Load("Imagens/alunos.png");

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

	}if(!caminhoSoldado)
	{
		printf("Nao foi possivel carregar a imagem caminhoSoldado\n");

	}if(!caminhoTorre)
	{
		printf("Nao foi possivel carregar a imagem caminhoTorre");

	}if(!selecao)
	{
		printf("Nao foi possivel carregar imagem selecao\n");
	}
	if(!soldado)
	{
		printf("Nao foi possivel carregar imagem soldado\n");
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

    for(int i=0; i<15; i++)
    {
        for(int j=0; j<19; j++)
        {

            fscanf(arquivo,"%d",&mapa[i][j]);

        }
    }

    fclose(arquivo);

}

void blitarMapa()
{
	 // blitando o mapa de azulejos
    for(int i=0; i<15; i++)
    {
        for(int j=0; j<19; j++)
        {

            campo[i][j].x = j*40;
            campo[i][j].y = i*40;
            campo[i][j].w = 40;
            campo[i][j].h = 40;

            if(mapa[i][j] == CAMINHO_SOLDADO)
            {
                SDL_BlitSurface(caminhoSoldado,NULL,tela,&campo[i][j]);
            }
            if(mapa[i][j] == CAMINHO_TORRE)
            {
                SDL_BlitSurface(caminhoTorre,NULL,tela,&campo[i][j]);
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
			nomeFase = "Fases/fase1.txt";
		break;

		default:
			break;
	}
}

// continuar esta função (esta incompleta)
int andarSoldado()
{
	if(rectSoldado.y < 320)
	{
		rectSoldado.y++;
	}
	if(rectSoldado.y == 320 && rectSoldado.x < 240)
	{
		rectSoldado.x++;
	}

}

// criar vetor de torres para colocar mais torres no jogo
// 

// Main

int main(void)
{
	inicializarComponentes();
    carregarSurfaces();
    escolherFase(1);
    carregarFase(nomeFase);

	TORRE torre1;  // em breve será vetor de torres
	for (int i = 0; i < MAX; ++i)
	{
		torre1.tiros[i].surface = IMG_Load("Imagens/tiro1.png");
		torre1.tiros[i].tiroAtivo = 0;
	}

	rectSelecao.x = 320;
	rectSelecao.y = 240;

	rectSoldado.x = 120;
	rectSoldado.y = 0;

	SDL_Rect rect;

	//torre1.angulo = 0;
	//torre1.rect.x = 0;
	//torre1.rect.y = 0;

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

					case SDLK_UP:    rectSelecao.y -= 40; break;
					case SDLK_DOWN:  rectSelecao.y += 40; break;
					case SDLK_LEFT:  rectSelecao.x -= 40; break;
					case SDLK_RIGHT: rectSelecao.x += 40; break;
					case SDLK_SPACE:

						// apertou tecla space, tiro receberá posição e angulo da torre
						/*
						
						*/
						break;

					case SDLK_x:

						//colocar a torre
						torre1.angulo = 0;
						torre1.rect.x = rectSelecao.x;
						torre1.rect.y = rectSelecao.y;
						colocarTorre = 1;

						break;
					case SDLK_r: torre1.angulo += 5; break;
					case SDLK_t: torre1.angulo -= 5; break;

					default: break;
				}
			}
		}

		SDL_BlitSurface(fundo, NULL, tela, NULL);

		blitarMapa();
		andarSoldado();

		transformedSurface = rotozoomSurface(torre,torre1.angulo,1.0,SMOOTHING_ON);
		SDL_Surface *tiroT;

    	// rotacionando sobre o proprio eixo
    	rect.x = torre1.rect.x;
    	rect.y = torre1.rect.y;

    	rect.x -= transformedSurface->w/2 - torre->w/2;
    	rect.y -= transformedSurface->h/2 - torre->h/2;

    	// tiro
    	/*
    	if(rectSoldado.y >= 320)
    	{
    		if (i == MAX) // controle número de tiros
			{
                i = 0;
			}
						
			torre1.tiros[i].tiroAtivo = 1;
			torre1.tiros[i].rect.x = rect.x;
			torre1.tiros[i].rect.y = rect.y;
			torre1.tiros[i].angulo = torre1.angulo;
						
			i++;
    	}
    	*/
		
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
					SDL_BlitSurface(torre1.tiros[j].surface, NULL, tela, &torre1.tiros[j].rect);
            
         	}

      	}

      	inicializarTiros(torre1);

      	if(colocarTorre)
      	{
      		controlarPosicaoTorre(rect);

			SDL_BlitSurface(transformedSurface, NULL, tela, &rect);	
      	}
      	
      	
      	SDL_BlitSurface(soldado, NULL, tela, &rectSoldado);	
      	
		SDL_BlitSurface(selecao, NULL, tela, &rectSelecao);

		SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela

		SDL_FreeSurface(transformedSurface);

	}

	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit(); // Fecha o SDL
  	return 0;

}

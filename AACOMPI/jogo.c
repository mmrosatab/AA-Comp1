#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "score.h"

#define NUM_TIROS 5
#define TAM_TILE 40
#define NUM_LINHAS 15
#define NUM_COLUNAS 19
#define NUM_ZUMBIS 15
#define NUM_TORRES 15
#define VIVO 1
#define MORTO 0
#define ATIVADA 1
#define DESATIVADA 0
#define ATIRANDO 2
#define EM_ESPERA 1
#define DESATIVADO 0

// Para compilar: gcc jogo10.c -o jogo10 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx -lm
// Para executar: ./jogo10

// Structs

typedef struct _TIRO
{
	int estado;
	SDL_Surface *surface;
	SDL_Rect rect;
	double angulo;
	int alcance;
	int xVel;
	int yVel;

}TIRO;


typedef struct _TORRE
{
	int estado; //ativada ou desativada
	SDL_Surface *surface;
	SDL_Rect rect;
	SDL_Rect aux;
	TIRO tiros[NUM_TIROS];
	double angulo;
	int alcanceTiro;
	float xVel;
	float yVel;

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
SDL_Surface *tela;	 // de qq forma precisaremos de uma surface p/ tela, mais abaixo usaremos ela
SDL_Surface *fundo;
SDL_Surface *fundoMenu;
SDL_Surface *fundoColocarNome;
SDL_Event event;

// surfaces menu score
SDL_Surface *fundoScore;
SDL_Surface *texto1;  // texto nome
SDL_Surface *texto2; // texto pontuação
SDL_Rect rectTexto1;
SDL_Rect rectTexto2;
TTF_Font *fonte;
SDL_Color corTexto;

SDL_Surface *selecao;
SDL_Surface *mouse;
SDL_Surface *vidas;
SDL_Surface *numVidas;
SDL_Surface *surfMortos;
SDL_Surface *surfScore;
SDL_Surface *fundoGameOver;
SDL_Surface *fundoInstrucoes;
SDL_Surface *ganhou;
SDL_Surface *audio;
SDL_Surface *torresColocadas;
SDL_Surface *torresDisponiveis;
SDL_Surface *barra;

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
SDL_Rect rectGarrinha;
SDL_Rect campo[NUM_LINHAS][NUM_COLUNAS];
SDL_Rect rectVidas; // NÃO ESTOU USANDO
SDL_Rect rectNumVidas;
SDL_Rect rectMortos;
SDL_Rect rectScore;
SDL_Rect rectAudio;
SDL_Rect rectTC;
SDL_Rect rectTD;
SDL_Rect rectBarra;

Mix_Music *musica;

/* Variáveis */
int xZumbi,yZumbi;
int mapa[NUM_LINHAS][NUM_COLUNAS];
int tamOrda 	 = 0;
int qtdTorres 	 = 0;
int numeroVidas	 = 6;
int zumbisMortos = 0;
int score 		 = 0;
int resistir 	 = 0;
int mortos       = 0;
int n 			 = 1; // controla orda
char *nomeFase;
char nj[30]; // nome do jogador
char nv[4]; // vetor que armazena o numero de vidas
char pb[16]; // buffer para pontuacao
char n_mortos[4]; // armazena número de zumbis mortos
char vetScore[4]; // armazena pontuação
char tc[4];
char td[4];

//Uint32 start;
Lista *lista; // lista de scores

ZUMBI z1[NUM_ZUMBIS];
TORRE t1[NUM_TORRES];

/* * * * * * * * * * Protótipos * * * * * * * * */

float distancia(SDL_Rect, SDL_Rect);
int jogo();
int recorde();
int temTorre();
int developers();
int inserirNome();
int controlarBlitTiro(SDL_Rect);
int tiroSaiuTela(SDL_Rect);
int colisao(SDL_Rect, SDL_Rect);
int extremosSelecao(SDL_Rect);
int torreNoCaminho(SDL_Rect);
int zumbiAlcance2();
int semZumbi();
int gameOver();
int instrucoes();
void atirar();
void carregarFase(char *);
void controlarPosicaoTorre(SDL_Rect);
void inicializarComponentes();
void inicializaFonte();
void carregarSurfaces();
void blitarMapa();
void escolherFase(int);
void inicializarZumbi(int);
void inicializarEstadoTorres();
void inicializarEstadoTiros();
void inicializarEstadoZumbis();
void inicializarTorre(int, int, int);
void andarZumbi2();
void controlarFrame();
void blitarZumbis();
void blitarTorres();
void zumbiAlcance();
void carregarSprite();
void matarZumbi(ZUMBI *);
void inicializarTiro();
void verificarVidas();
void resistencia();
TTF_Font *carregarFonte(const char*, int);

/* * * * * * * * * * * * * * * * * * * * * * * * */

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

                    numeroVidas  = 6;
                    zumbisMortos = 0;
                    tamOrda 	 = 5;
                    mortos 		 = 0;
                    score 		 = 0;
            	}

            	else if((event.button.x >= 535 && event.button.x <= 741) && (event.button.y >= 316 && event.button.y <= 365))
            	{
                    //printf("Cliquei nas instrucoes\n");
                    if(!instrucoes())
                    {
                    	loop = 1;
                    }
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

int jogo()
{

	int loop = 0;
	tamOrda = 5;
	qtdTorres = 15;
	int i=0;
	int j=0;
	int k = 0;
	int atire = FALSE;
	//int pontuacao = 0;
	carregarSprite();
	inicializarEstadoZumbis();
	inicializarEstadoTorres();
	inicializarEstadoTiros();

	int colocarOrda = 0;
	int pausaOrda = 0;
	int ativarSelecao = 0;
	int colocarTorre = FALSE;
	int mute = 0;

	rectNumVidas.x = 90;
	rectNumVidas.y = 603;
	rectMortos.x   = 245;
	rectMortos.y   = 603;
	rectScore.x    = 375;
	rectScore.y    = 603;
	rectAudio.x    = 793;
	rectAudio.y    = 412;

    rectTC.x       = 610;
    rectTC.y       = 603;
    rectBarra.x    = 640;
    rectBarra.y    = 603;
    rectTD.x       = 660;
    rectTD.y       = 603;
    


	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	//musica = Mix_LoadMUS("Musica/suspense.mp3");
    musica = Mix_LoadMUS("Musica/thriller.mp3");
	Mix_PlayMusic(musica,-1);

	SDL_Rect rectMouse;
	int x,y;


	while(loop == 0)
	{
		//start=SDL_GetTicks();

		if(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{
				return 0;

			}else if(event.type == SDL_MOUSEMOTION)
            {

            	if(ativarSelecao)
            	{
                    SDL_GetMouseState(&x,&y);
            		rectMouse.x = x;
            		rectMouse.y = y;
            	}

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
            		Jogador *j1 = criarJogador(nj,score);
					inserirOrdenado(lista,j1);
            		loop = 1;
            		Mix_PauseMusic();

            	}else if((event.button.x >= 770 && event.button.x <= 810) && (event.button.y >= 210 && event.button.y <= 240))
            	{
            		rectSelecao.x = 770;
            		rectSelecao.y = 200;
            		ativarSelecao = TRUE;

            	}else if((event.button.x >= 0 && event.button.x <= 760) && (event.button.y >= 0 && event.button.y <= 600) && (ativarSelecao == TRUE))
            	{
					if(j<qtdTorres && !torreNoCaminho(rectMouse))
					{
						inicializarTorre(j,x,y);
            			j++;
					}

					ativarSelecao = FALSE;
            	}
            	else if((event.button.x >= 790 && event.button.x <= 842) && (event.button.y >= 420 && event.button.y <= 460))
            	{
            		if(mute == 0)
            		{
            			mute = 1;
            			Mix_PauseMusic();
            		}
            		else
            		{
            			mute = 0;
            			Mix_ResumeMusic();
            		}
            	}
            }

		}

		SDL_BlitSurface(fundo, NULL, tela, NULL);

		if(i<tamOrda && colocarOrda)
		{
			if(i == 0)
			{
				inicializarZumbi(i);
				i++;
			}
			else if(z1[i-1].rect.x == 40)
			{
				inicializarZumbi(i);
				i++;
			}

		}


		blitarMapa();
        blitarTorres();

		if(pausaOrda)
		{
			blitarZumbis();
		}

		if(colocarOrda && temTorre())
		{

			andarZumbi2();
      		controlarFrame();
      		blitarZumbis();
      		zumbiAlcance();
			
      		inicializarTiro();
      		atirar();
      		matarZumbi(z1);

      		if(semZumbi() && tamOrda < NUM_ZUMBIS)
      		{
      			tamOrda++;
      			inicializarEstadoZumbis();
      			resistencia();
      			i=0;
      		}
      		
      		verificarVidas();
		}


		if(ativarSelecao)
		{
			SDL_BlitSurface(selecao,NULL,tela,&rectSelecao);

			if(!extremosSelecao(rectMouse))
				SDL_BlitSurface(mouse,NULL,tela,&rectMouse);
		}

		//atirar();
		//verificarVidas();

		sprintf(nv, "%d",numeroVidas);
		sprintf(n_mortos, "%d", zumbisMortos);
		sprintf(vetScore, "%d", score);

        // torre colocadas e torres disponiveis
        sprintf(tc, "%d", j);
        sprintf(td, "%d", NUM_TORRES);

        numVidas   = TTF_RenderText_Solid(fonte, nv, corTexto);
        surfMortos = TTF_RenderText_Solid(fonte, n_mortos, corTexto);
        surfScore  = TTF_RenderText_Solid(fonte, vetScore, corTexto);

        torresColocadas = TTF_RenderText_Solid(fonte, tc, corTexto);
        barra = TTF_RenderText_Solid(fonte, "/", corTexto);
        torresDisponiveis = TTF_RenderText_Solid(fonte, td, corTexto);

        SDL_BlitSurface(numVidas, NULL, tela, &rectNumVidas);
        SDL_BlitSurface(surfMortos, NULL, tela,  &rectMortos);
        SDL_BlitSurface(surfScore, NULL, tela,  &rectScore);

        SDL_BlitSurface(torresColocadas, NULL, tela,  &rectTC);
        SDL_BlitSurface(barra, NULL, tela,  &rectBarra);
        SDL_BlitSurface(torresDisponiveis, NULL, tela,  &rectTD);

        if(mute)
        {
        	SDL_BlitSurface(audio, NULL, tela, &rectAudio);
        }

		//SDL_BlitSurface(vidas,NULL,tela,&rectVidas);
		SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela
		/*
		if((SDL_GetTicks()-start) < 1000/25)
		{
			SDL_Delay(1000/25 -(SDL_GetTicks() - start));
		}
		*/

		if(numeroVidas == 0)// || tamOrda == NUM_ZUMBIS) // PODE DAR RUIM
		{
			if(!semZumbi())
			{
				Jogador *j1 = criarJogador(nj,score);
				inserirOrdenado(lista,j1);
				Mix_PauseMusic();

				if(gameOver())
				{
					loop = 1;
				}
				else
				{
					return 0;
				}
			}
			
		}

		else if(numeroVidas > 0 && (tamOrda == NUM_ZUMBIS && semZumbi())) // 110 valor da fórmula da P.A.
		{
			Mix_PauseMusic();
			SDL_BlitSurface(ganhou, NULL, tela, NULL);
			SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela
			SDL_Delay(3000);
			Jogador *j1 = criarJogador(nj,score);
			inserirOrdenado(lista,j1);
			loop = 1;
		}
	}

	return 1;
}

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
    window 			 = SDL_CreateWindow("Tower Defense",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,880,640,SDL_WINDOW_OPENGL);
    tela 			 = SDL_GetWindowSurface(window);
	fundo 			 = IMG_Load("Imagens/fundo3.png");
	fundoMenu 		 = IMG_Load("Imagens/fundoMenu2.png");
	selecao 		 = IMG_Load("Imagens/selecao.png");
	mouse 			 = IMG_Load("Imagens/selecao.png");
	desenvolvedores  = IMG_Load("Imagens/desenvolvedores3.png");
	fundoScore 		 = IMG_Load("Imagens/menuRanking3.png");
	fundoGameOver 	 = IMG_Load("Imagens/gameOver.png");
	fundoInstrucoes  = IMG_Load("Imagens/instrucoes.png");
	ganhou 			 = IMG_Load("Imagens/ganhou.png");
	fundoColocarNome = IMG_Load("Imagens/fundoColocarNome.png");

	/* Tiles */

	zero 	 = IMG_Load("Imagens/0.png");
	um 		 = IMG_Load("Imagens/1.png");
	dois	 = IMG_Load("Imagens/2.png");
	tres 	 = IMG_Load("Imagens/3.png");
	quatro 	 = IMG_Load("Imagens/4.png");
	cinco 	 = IMG_Load("Imagens/5.png");
	seis 	 = IMG_Load("Imagens/6.png");
	sete 	 = IMG_Load("Imagens/7.png");
	oito 	 = IMG_Load("Imagens/8.png");
	nove 	 = IMG_Load("Imagens/9.png");
	dez 	 = IMG_Load("Imagens/10.png");
	onze 	 = IMG_Load("Imagens/11.png");
	doze 	 = IMG_Load("Imagens/12.png");
	treze 	 = IMG_Load("Imagens/13.png");
	quatorze = IMG_Load("Imagens/14.png");
	garrinha = IMG_Load("Imagens/mao_com_sangue.png");
	audio 	 = IMG_Load("Imagens/mute.png");


	for(int i=0;i<NUM_ZUMBIS;i++)
	{
		z1[i].surface = IMG_Load("Imagens/z1.png");
	}

	for(int i=0;i<NUM_TORRES;i++)
	{
		t1[i].surface = IMG_Load("Imagens/torre.png");

		for (int j=0;j<NUM_TIROS;j++)
		{
			t1[i].tiros[j].surface = IMG_Load("Imagens/tiro1.png");

			if(!t1[i].tiros[j].surface)
			{
				printf("Deu ruim na imagem tiro1");
			}
		}
	}


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
	if(!fundoColocarNome)
	{
		printf("Nao foi possivel carregar imagem fundoColocarNome\n");
	}
	if(!fundoGameOver)
	{
		printf("Nao foi possivel carregar imagem fundoGameOver\n");
	}
	if(!audio)
	{
		printf("Nao foi possivel carregar imagem audio\n");
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

// funcao que ajusta torre dentro dos limites da tela
void controlarPosicaoTorre(SDL_Rect rect)
{
	if(rect.x < 0)
	{
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

void inicializarEstadoTiros()
{
	// inicia o vetor de tiros com 0's em todas posiçoes (tiros nao ativos)
	for(int i = 0;i < NUM_TORRES;i++)
	{
		for(int j = 0;j < NUM_TIROS;j++)
		{
			t1[i].tiros[j].estado = EM_ESPERA;
		}
	}

}

void inicializarEstadoZumbis()
{
	for (int i = 0; i < NUM_ZUMBIS; i++)
	{
		z1[i].estado 	  = MORTO;
		z1[i].resistencia = 0;
	}
}

// prepara qtd de zumbis p/ uma orda especifica
void inicializarZumbi(int i)
{
	z1[i].rect.x = 0;
	z1[i].rect.y = 80;
	z1[i].estado = VIVO;
}

void inicializarEstadoTorres()
{
	for (int i = 0; i < NUM_TORRES; i++)
	{
		t1[i].estado = DESATIVADA;
		t1[i].alcanceTiro = 100;
	}

}

void inicializarTorre(int i, int x,int y)
{
	t1[i].rect.x = x;
	t1[i].rect.y = y;
	t1[i].estado = ATIVADA;

	for (int j = 0; j < NUM_TIROS; j++)
	{
		t1[i].tiros[j].rect.x = x;
		t1[i].tiros[j].rect.y = y+10;
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
				n++;

				if(n == tamOrda && tamOrda < NUM_ZUMBIS)
				{
					tamOrda++;
					n = 1;
				}
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

int tiroSaiuTela(SDL_Rect tiro)
{
	if(tiro.x <= 0|| tiro.x >= 760 || tiro.y >= 600 || tiro.y <= 0)
	{
		return TRUE;
	}
	return FALSE;
}

void atirar()
{
	int x;
	int y;

	for(int i = 0;i < qtdTorres; i++)
    {
    	if(t1[i].estado == ATIVADA)
    	{
    		x = t1[i].rect.x;
    		y = t1[i].rect.y;

    		for (int j = 0; j < NUM_TIROS; j++)
    		{
    			if(t1[i].tiros[j].estado == ATIRANDO)
    			{
    				
	            	if(!tiroSaiuTela(t1[i].tiros[j].rect) && zumbiAlcance2())
	            	{
	            		t1[i].tiros[j].rect.x += (xZumbi - x) * 0.15;
    					t1[i].tiros[j].rect.y += (yZumbi - y) * 0.15;
	            		SDL_BlitSurface(t1[i].tiros[j].surface, NULL, tela, &t1[i].tiros[j].rect);
	            		break;
	            	}
	            	else
	            	{
            			int x = t1[i].rect.x;
						int y = t1[i].rect.y;

						t1[i].tiros[j].rect.x = x;
						t1[i].tiros[j].rect.y = y+10;
            			t1[i].tiros[j].estado = DESATIVADO;
	            	}
    			}

    		}
    	}
    }
}

int colisao(SDL_Rect rect1, SDL_Rect rect2)
{
	 if(rect2.x + rect2.w < rect1.x)
	    return 0;
	 if(rect2.x > rect1.x + rect1.w)
	    return 0;
	 if(rect2.y + rect2.h < rect1.y)
	    return 0;
	 if(rect2.y > rect1.y + rect1.h)
	    return 0;

	 return 1;
}

void matarZumbi(ZUMBI *zumbi)
{
	for (int i = 0; i < tamOrda; i++)
	{
		if(zumbi[i].estado == VIVO)
		{
			for (int j = 0; j < qtdTorres; j++)
			{
				if (t1[j].estado == ATIVADA)
				{
					for (int k = 0; k < NUM_TIROS; k++)
					{
						if (t1[j].tiros[k].estado == ATIRANDO)
						{
							if(colisao(zumbi[i].rect,t1[j].tiros[k].rect))
							{
								// se houve colisao entre zumbi e tiro
								// estado zumbi recebe MORTO

								if(zumbi[i].resistencia == 0)
								{
									zumbi[i].estado = MORTO;
									score += (zumbi[i].resistencia/100) + 3; 
									zumbisMortos++;
									mortos++;

									if(numeroVidas > 0 && mortos == tamOrda)
									{
										numeroVidas++;
										mortos = 0;
										//printf("ENTREI AQUI, AUMENTA MINHA VIDA");
									}
								}
								else
								{
									zumbi[i].resistencia = zumbi[i].resistencia - 1;
								}
							}
							
						}
						
					}
				}	
				
			}
		}
		
	}

}

// funcao exibe recordes do jogo na tela

int extremosSelecao(SDL_Rect mouse)
{
	if((mouse.x > 760) || (mouse.y > 600))
	{
		return TRUE;
	}
	return FALSE;
}

int temTorre()
{
	for (int i = 0; i < qtdTorres; i++)
	{
		if(t1[i].estado == ATIVADA)
		{
			return TRUE;
		}
	}
	return FALSE;
}

float distancia(SDL_Rect zumbi, SDL_Rect torre)
{
	int xzumbi = zumbi.x;
	int yzumbi = zumbi.y;

	int xTorre = torre.x;
	int yTorre = torre.y;

	double distancia = sqrt(pow(yzumbi-yTorre,2) + pow(xzumbi-xTorre,2));

	// retornar a distancia , na outra funcao guardar a menor distancia e o indice da menor distancia
	return distancia;
}

void inicializarTiro()
{
	for (int i = 0; i < qtdTorres; i++)
	{
		if(t1[i].estado == ATIVADA)
		{

			for (int j = 0; j < NUM_TIROS; j++)
			{
				if(t1[i].tiros[j].estado == EM_ESPERA)
	        	{

	        		//printf("tiro indice j %d \n",j);
	        		t1[i].tiros[j].estado = ATIRANDO;
	            	break;


	        	}else

				if(t1[i].tiros[j].estado == DESATIVADO)
				{
					int x = t1[i].rect.x;
					int y = t1[i].rect.y;

					t1[i].tiros[j].rect.x = x;
					t1[i].tiros[j].rect.y = y+10;
					t1[i].tiros[j].estado = EM_ESPERA;
				}


			}
			//printf("entrei\n");
		}

	}
}

void verificarVidas()
{
	for(int i = 0; i < tamOrda; i++)
	{
		if(z1[i].estado == ATIVADA && z1[i].rect.x == 719 && z1[i].rect.y == 480)
			numeroVidas--;
	}
}

int gameOver()
{
    
    int loop = 0; // Variável de controle do loop
	
	SDL_BlitSurface(fundoGameOver,NULL,tela,NULL);

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

int recorde()
{
    rectTexto1.x = 250;
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
	SDL_Rect posicaoTracro;
	char nomeJogador[30];

	posicaoMsg.x = 300;
	posicaoMsg.y = 200;
	posicao.x = 290;
	posicao.y = 300;
	posicaoTracro.x = 290;
	posicaoTracro.y = 305;

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
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_b:
						if(indice<30)
						{
							nomeJogador[indice] = 'b';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_c:
						if(indice<30)
						{
							nomeJogador[indice] = 'c';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_d:
						if(indice<30)
						{
							nomeJogador[indice] = 'd';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_e:
						if(indice<30)
						{
							nomeJogador[indice] = 'e';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_f:
						if(indice<30)
						{
							nomeJogador[indice] = 'f';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_g:
						if(indice<30)
						{
							nomeJogador[indice] = 'g';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_h:
						if(indice<30)
						{
							nomeJogador[indice] = 'h';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_i:
						if(indice<30)
						{
							nomeJogador[indice] = 'i';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_j:
						if(indice<30)
						{
							nomeJogador[indice] = 'j';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_k:
						if(indice<30)
						{
							nomeJogador[indice] = 'k';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_l:
						if(indice<30)
						{
							nomeJogador[indice] = 'l';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_m:
						if(indice<30)
						{
							nomeJogador[indice] = 'm';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_n:
						if(indice<30)
						{
							nomeJogador[indice] = 'n';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_o:
						if(indice<30)
						{
							nomeJogador[indice] = 'o';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_p:
						if(indice<30)
						{
							nomeJogador[indice] = 'p';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_q:
						if(indice<30)
						{
							nomeJogador[indice] = 'q';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_r:
						if(indice<30)
						{
							nomeJogador[indice] = 'r';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_s:
						if(indice<30)
						{
							nomeJogador[indice] = 's';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_t:
						if(indice<30)
						{
							nomeJogador[indice] = 't';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_u:
						if(indice<30)
						{
							nomeJogador[indice] = 'u';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_v:
						if(indice<30)
						{
							nomeJogador[indice] = 'v';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_w:
						if(indice<30)
						{
							nomeJogador[indice] = 'w';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_x:
						if(indice<30)
						{
							nomeJogador[indice] = 'x';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_y:
						if(indice<30)
						{
							nomeJogador[indice] = 'y';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;
					case SDLK_z:
						if(indice<30)
						{
							nomeJogador[indice] = 'z';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_1:
						if(indice<30)
						{
							nomeJogador[indice] = '1';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_2:
						if(indice<30)
						{
							nomeJogador[indice] = '2';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_3:
						if(indice<30)
						{
							nomeJogador[indice] = '3';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_4:
						if(indice<30)
						{
							nomeJogador[indice] = '4';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_5:
						if(indice<30)
						{
							nomeJogador[indice] = '5';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_6:
						if(indice<30)
						{
							nomeJogador[indice] = '6';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_7:
						if(indice<30)
						{
							nomeJogador[indice] = '7';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_8:
						if(indice<30)
						{
							nomeJogador[indice] = '8';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_9:
						if(indice<30)
						{
							nomeJogador[indice] = '9';
							indice++;
							nomeJogador[indice] = '\0';
						}
						break;

					case SDLK_0:
						if(indice<30)
						{
							nomeJogador[indice] = '0';
							indice++;
							nomeJogador[indice] = '\0';
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
		SDL_BlitSurface( ttfTraco, NULL,tela, &posicaoTracro);

    }

	//return 0;
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

void zumbiAlcance()
{
	int dist;
	int menorDist = 3000;
	int indZumbi;

	for (int i = 0; i < qtdTorres; i++)
	{
		if(t1[i].estado == ATIVADA)
		{
			for (int j = 0; j < tamOrda; j++)
			{
				if (z1[j].estado == VIVO)
				{
					dist = distancia(t1[i].rect,z1[j].rect);

					if(dist < menorDist)
					{
						menorDist = dist;
						indZumbi = j;
					}
				}
			}

			t1[i].xVel = z1[indZumbi].rect.x;
			t1[i].yVel = z1[indZumbi].rect.y;
			menorDist = 3000;
		}

	}
}

int zumbiAlcance2()
{
	int dist;

	for (int i = 0; i < qtdTorres; i++)
	{
		if(t1[i].estado == ATIVADA)
		{
			for (int j = 0; j < tamOrda; j++)
			{
				if (z1[j].estado == VIVO)
				{
					dist = distancia(t1[i].rect,z1[j].rect);

					if (dist <= t1[i].alcanceTiro)
					{
						xZumbi = z1[j].rect.x;
						yZumbi = z1[j].rect.y;
						return TRUE;
					}
				}
			}
		}

	}

	return FALSE;
}


// verifica de torre esta no caminho do zumbi 
//se sim retorna true, se nao retorna falso
int torreNoCaminho(SDL_Rect mouse)
{
	for (int i = 0; i < NUM_LINHAS; i++)
	{
		for (int j = 0; j < NUM_COLUNAS; j++)
		{
			if(colisao(campo[i][j],mouse) == 1)
			{
				if(mapa[i][j] == 1 || mapa[i][j] == 2 || mapa[i][j] == 3||
					mapa[i][j] == 4 || mapa[i][j] == 5 || mapa[i][j] == 6)
            	{
               		return TRUE;
           	 	}
			}
		}
	}
	return FALSE;
}

int semZumbi()
{
	for (int i = 0; i < tamOrda; i++)
	{
		if(z1[i].estado == VIVO)
		{
			return FALSE;
		}
	}

	return TRUE;
}

void resistencia()
{
	resistir = resistir + 100;

	for(int i = 0; i < tamOrda; i++)
	{
		z1[i].resistencia = resistir;
	}
}

int instrucoes()
{
	int loop = 0; // Variável de controle do loop
	
	SDL_BlitSurface(fundoInstrucoes,NULL,tela,NULL);

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
                if((event.button.x >= 100 && event.button.x <= 178) && (event.button.y >= 100 && event.button.y <= 140))
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
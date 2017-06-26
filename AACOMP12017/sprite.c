#include <SDL2/SDL.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Window *window;  // --- SDL_Surface* window; mudanças sdl2, agora existe uma SDL_Window dedicada p/ fazer janelas
SDL_Surface *tela;		// de qq forma precisaremos de uma surface p/ tela, mais abaixo usaremos ela
SDL_Surface *fundo;
SDL_Event event;

SDL_Surface *zumbi;
SDL_Rect destZumbi;
SDL_Rect spriteZumbi[2];
int frame = 0;
int andando = 0;
int loop = 1;

// Main
int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Init(SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG);

    // sdl 2 ---
    window = SDL_CreateWindow("Tower Defense",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,880,640,SDL_WINDOW_OPENGL);
    tela = SDL_GetWindowSurface(window); //
	zumbi = IMG_Load("Imagens/z1.png");
	fundo = IMG_Load("Imagens/fundo.png");

	destZumbi.x = 0;
	destZumbi.y = 120;

	
	spriteZumbi[0].x = 8;
    spriteZumbi[0].y = 0;
    spriteZumbi[0].w = 40;
    spriteZumbi[0].h = 80;

    spriteZumbi[1].x = 47;
    spriteZumbi[1].y = 0;
    spriteZumbi[1].w = 40;
    spriteZumbi[1].h = 80;


	while(loop)
	{
		if(SDL_PollEvent(&event))
		{
			if(event.type == SDL_QUIT)
			{	
				loop = 0;
			}
			else if(event.type == SDL_KEYDOWN)
			{
				switch(event.key.keysym.sym)
				{

					case SDLK_UP:    //rectSelecao.y -= 40; break;
					case SDLK_DOWN:  //rectSelecao.y += 40; break;
					case SDLK_LEFT:  //rectSelecao.x -= 40; break;
					case SDLK_RIGHT: //rectSelecao.x += 40; break;
					case SDLK_SPACE:

					break;

					case SDLK_RETURN: 
						zumbi = IMG_Load("Imagens/z1lado.png"); 
						andando = 1;
						break;

					default: break;
				}
			}
	
		}

		SDL_BlitSurface(fundo, NULL, tela, NULL);

		//verifica se andou e faz o controle do loop dos sprites
	
        if(andando == 1)
        {
        	destZumbi.x++;
            frame++;
        }
        else
        {
            frame = 0;
        }
        if(frame == 2)
        {
            frame = 0;
        }
	
      		

      	if(destZumbi.x > 880)
      	{
      		destZumbi.x = 0;
      	}

      	SDL_BlitSurface(zumbi,&spriteZumbi[frame],tela,&destZumbi);
		SDL_UpdateWindowSurface(window); // essa linha essencial, sem ela nada é atualizado na tela

	}


	SDL_DestroyWindow(window);

	IMG_Quit();
	SDL_Quit(); // Fecha o SDL
  	return 0;

}
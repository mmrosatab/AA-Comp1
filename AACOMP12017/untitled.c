
void andarZumbi3()
{
	for (int i = 0; i < NUM_ZUMBIS; i++)
	{
		if(colocarZumbi)
		{
			printf("entrei %d\n",i);
			if(z2[i].rect.x < 160)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);
			}
			else if(z2[i].rect.x == 160 && z2[i].rect.y != 360)
			{

				z2[i].rect.y++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.y == 360 && z2[i].rect.x != 400 && z2[i].rect.x != 600)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.x == 400 && z2[i].rect.y != 120)
			{
				z2[i].rect.y--;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.y == 120 && z2[i].rect.x != 600)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}
			else if(z2[i].rect.x == 600 && z2[i].rect.y != 480)
			{
				z2[i].rect.y++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);
			}
			else if(z2[i].rect.y == 480 && z2[i].rect.x != 720)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.x == 720) 
			{
				z2[i].rect.x = -1;
				z2[i].rect.y = 80;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);
			}	
		}
	}

}

void andarZumbi2()
{
	for (int i = 0; i < NUM_ZUMBIS; i++)
	{
		if(colocarZumbi)
		{
			printf("entrei %d\n",i);
			if(z2[i].rect.x < 160)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);
			}
			else if(z2[i].rect.x == 160 && z2[i].rect.y != 360)
			{

				z2[i].rect.y++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.y == 360 && z2[i].rect.x != 400 && z2[i].rect.x != 600)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.x == 400 && z2[i].rect.y != 120)
			{
				z2[i].rect.y--;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.y == 120 && z2[i].rect.x != 600)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}
			else if(z2[i].rect.x == 600 && z2[i].rect.y != 480)
			{
				z2[i].rect.y++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);
			}
			else if(z2[i].rect.y == 480 && z2[i].rect.x != 720)
			{
				z2[i].rect.x++;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);

			}else if(z2[i].rect.x == 720) 
			{
				z2[i].rect.x = -1;
				z2[i].rect.y = 80;
				SDL_BlitSurface(zumbi, NULL, tela, &z2[i].rect);
			}	
		}
	}

}

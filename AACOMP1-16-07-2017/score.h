#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define RANKING 10
#define TRUE 1
#define FALSE 0


// estruturas

typedef struct _Jogador
{
	//char nome[60]; //
	char *nome;
	int pontuacao;
	
}Jogador;

typedef struct _No
{
	Jogador *jogador;
	struct _No *prox;
}No;

typedef struct _Lista
{
	No *inicio;
	No *fim;
	int tamanho;

}Lista;

//prototipos das funções
Lista *criarLista();
int vazia(Lista *lista);
Jogador* criarJogador(char *nome,int pontuacao);
int removerFim(Lista *lista);
int inserirFim(Lista *lista, Jogador *jogador);
int inserirOrdenado(Lista *lista, Jogador *jogador);
void imprimirLista(Lista *lista);
void carregarLista(char nomeArquivo[],Lista* lista);
int guardarLista(char nomeArquivo[],Lista *lista);

Lista *criarLista()
{
	
	Lista *lista = (Lista *) malloc (sizeof(Lista));
	
	if(lista != NULL)
	{
		lista->inicio = NULL;
		lista->fim = NULL;
		lista->tamanho = 0;
	}

	return lista;
}

int vazia(Lista *lista)
{
	if(lista->tamanho == 0)
	{
		return TRUE;
	}else
	{
		return FALSE;
	}
}

Jogador* criarJogador(char nome[],int pontuacao)
{
	Jogador *jogador = (Jogador *) malloc (sizeof(Jogador));

	if(jogador != NULL)
	{
		jogador->nome = malloc(60 * sizeof(char));
		strcpy(jogador->nome,nome);
		jogador->pontuacao = pontuacao;

		return jogador;

	}else
	{
		printf("Nao foi possivel alocar memoria\n");
		return NULL;
	}
}

int removerFim(Lista *lista)
{
	No *ant = lista->inicio;
	No *aux = lista->inicio;

	if(!vazia(lista))
	{
		while(aux->prox != NULL)
		{
			ant = aux;
			aux = aux->prox;
		}

		ant->prox = NULL;
		lista->fim = ant; 
		lista->tamanho--;

		return TRUE;
	}else
	{
		return FALSE;
	}
}

int inserirFim(Lista *lista, Jogador *jogador)
{
	No *pnovo = (No*) malloc(sizeof(No));
	
	if(pnovo != NULL)
	{
		pnovo->jogador = jogador;
		pnovo->prox = NULL;
		
		if(lista->inicio == NULL)
		{
			lista->inicio = pnovo;		
		}else
		{
			lista->fim->prox = pnovo;
		}

		lista->fim = pnovo;
		lista->tamanho++;
	    return TRUE;

	}else
	{
		return FALSE;
	}	
}

int inserirOrdenado(Lista *lista, Jogador *jogador)
{
	No *pnovo = (No*) malloc(sizeof(No));

	if(pnovo != NULL)
	{
		pnovo->jogador = jogador;

		//caso a lista esteja vazia insiro no fim
		if(vazia(lista))
		{
			inserirFim(lista,jogador);

		}// caso inserção seja no inicio
		else if(lista->inicio->jogador->pontuacao < pnovo->jogador->pontuacao)
		{
			No *aux = lista->inicio;
			aux->prox = lista->inicio->prox;
			pnovo->prox = aux;
			lista->inicio = pnovo;
			lista->tamanho++;

		}else
		{
			No *aux1 = lista->inicio;
			No *aux2 = lista->inicio->prox;

			while(aux2 != NULL && aux2->jogador->pontuacao > pnovo->jogador->pontuacao)
			{
				aux1 = aux2;
				aux2 = aux2->prox;
			}

			pnovo->prox = aux2;
			aux1->prox = pnovo;
			lista->tamanho++;
		}

		if(lista->tamanho > RANKING)
		{
			// removo ultimo nó da lista
			removerFim(lista);
		}

	    return TRUE;
	}else
	{
		return FALSE;
	}
}


void imprimirLista(Lista *lista)
{
	No *paux = lista->inicio;

	if(!vazia(lista))
	{
		while(paux != NULL)
		{
			printf("Nome: %s || Pontuacao: %d\n",paux->jogador->nome,paux->jogador->pontuacao);
			paux = paux->prox;	
		}
	}
	
}

// esta função pega os dados do txt e retorna-os em uma lista
void carregarLista(char nomeArquivo[],Lista* lista)
{
	FILE *file = fopen(nomeArquivo,"r");

	if(file == NULL)
	{
		printf("Arquivo não encontrado\n");

	}else
	{
		char nome[30];
		int pontuacao;
		int c;

		while(!feof(file))
		{
			if(fscanf(file,"%s",nome) != EOF)
			{
				fscanf(file,"%d",&pontuacao);

				Jogador *jogador = criarJogador(nome,pontuacao);
				inserirFim(lista,jogador);
			}else
			{
				break;
			}
		}
	}
}

int guardarLista(char nomeArquivo[],Lista *lista)
{

	No *aux = lista->inicio;
	
	FILE *file = fopen(nomeArquivo,"w");

	if(file == NULL)
	{
		printf("Arquivo com problema!\n");
		return FALSE;
	}

	while(aux != NULL)
	{
		fprintf(file,"%s",aux->jogador->nome);
		fprintf(file,"\t");
		fprintf(file,"%d",aux->jogador->pontuacao);
		fprintf(file,"\n");

		aux = aux->prox;
	}

	fclose(file);
	return TRUE;
}

void apagarNo(No * n)
{
	free(n->jogador->nome);
	free(n->jogador);
	free(n);
}

void apagarLista(Lista *lista)
{

	No *aux;
	No *remover = lista->inicio;
	
	while(lista->tamanho > 0)
	{

		//anda com 2 ponteiros e apaga sempre o anterior, salvando o endereço do prox no
		//printf("Nome: %s ",remover->jogador->nome);
		//printf("Pontuacao: %d \n",remover->jogador->pontuacao);
		aux = remover->prox;
		apagarNo(remover);
		remover = aux;
		lista->tamanho = lista->tamanho - 1;
	}

	free(lista);
}

/*
int main()
{

	Lista *lista = criarLista();
	carregarLista("ranking.txt",lista);

	//pego score do jogador atual, insiro na lista e gravo no txt em seguida
	Jogador *j1 = criarJogador("Levi",2999);
	
	inserirOrdenado(lista,j1);
	
	imprimirLista(lista);

	guardarLista("ranking.txt",lista);

	

	return 0;
}
*/
#include "listas.h"

// Inicializa lista
void inicializa_lista(lista_t *lista){
	lista->proximo = NULL;
}

// Lista vazia
int vazia_lista(lista_t *lista){
	if(lista->proximo == NULL)
		return 1;
	else
		return 0;
}

// Insere no fim da lista
void insere_lista(lista_t *lista, char *id, char *valor){
  lista_t *novo;
  novo = (lista_t *) malloc (sizeof (lista_t));

  strcpy(novo->id,id);
  strcpy(novo->valor,valor);
  novo->proximo = lista->proximo;

  lista->proximo = novo;
}

// Busca um id na lista e retorna o valor associado ao mesmo
lista_t * busca_lista(lista_t *lista, char *id_buscado){
  lista_t *aux = lista;

  while (aux != NULL){
		// Se o id buscado esta na lista, retorna o EQU encontrado
		if(!strcmp(aux->id, id_buscado)){
			return aux;
		}
		// Percorre a lista
    aux = aux->proximo;
  }
	// Nao achou o EQU na lista
	return NULL;
}

// Mostra a lista
void exibe_lista(lista_t *lista){
  printf("\n");
	if(vazia_lista(lista)){
		printf("Lista vazia!\n\n");
		return;
	}

	lista_t *aux = lista->proximo;

	while(aux!=NULL){
		printf("id: %s ", aux->id);
    printf("valor: %s", aux->valor);
    printf("->");
		aux = aux->proximo;
	}
	printf("\n\n");
}

// Libera a memoria alocada pela lista
void libera_lista(lista_t *lista){
	if(!vazia_lista(lista)){
		lista_t *proximoNo, *noAtual;

		noAtual = lista->proximo;
		while(noAtual != NULL){
			proximoNo = noAtual->proximo;
			free(noAtual);
			noAtual = proximoNo;
		}
	}
}

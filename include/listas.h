#ifndef LISTAS_H
#define LISTAS_H

// Tamanho maximo dos atributos de uma lista
#define TAMAT 10

// Lista para as definicioes de equ
typedef struct lista{
  char id[TAMAT];
  char valor[TAMAT];
  struct lista *proximo;
} lista_t;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arquivo.h"

// Mapa de bits com enderecos de relocacao p/ ligador
extern lista_t *mapa_bits;
extern lista_t *mapa_provisorio;
// Lista de zeros e lista de alterados (verificacao de erros)
// atribs: label
extern lista_t *lista_zeros;
// atribs: label
extern lista_t *lista_ctes;
// atribs: label, tamanho
extern lista_t *lista_tamanhos_vetores;

// Instancia as listas globais
void instancia_listas_globais();

/*********** Funcoes de manipulacao das listas ***********/
void inicializa_lista(lista_t *lista);
int vazia_lista(lista_t *lista);
void insere_elemento(lista_t *lista, char *id, char *valor);
lista_t* insere_lista(lista_t *lista, lista_t *lista2);
lista_t* busca_elemento(lista_t *lista, char *id_buscado);
void exibe_lista(lista_t *lista);
void libera_lista(lista_t *lista);

// Funcoes de encapsulamento
char *pega_valor(lista_t *lista, char *id_buscado);

#endif

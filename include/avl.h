#ifndef AVL_H
#define AVL_H

#include "aluno.h"

typedef struct AVL AVL;

typedef void (*VisitarAluno)(Aluno *aluno);/* A aplicacao fornece para "fazer algo" com cada aluno visitado. */
AVL *avl_criar(int primeira_matricula);
void avl_destruir(AVL *arvore);
int avl_cadastrar(AVL *arvore, const char *nome);
int avl_remover(AVL *arvore,const char *nome, int matricula);
int avl_buscar_nome(AVL *arvore,const char *nome, VisitarAluno visitar);
void avl_listar_em_ordem(AVL *arvore, VisitarAluno visitar);
int avl_proxima_matricula(AVL *arvore);
int avl_tamanho( AVL *arvore);
int avl_altura(AVL *arvore);
int avl_esta_balanceada(AVL *arvore);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aluno.h"
#include "avl.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct no_t {
    Aluno *aluno;
    int altura;
    struct no_t *esq;
    struct no_t *dir;
} no_t;

struct AVL {
    no_t *raiz;
    int proxima_matricula;
    int tamanho;
};


int altura(no_t *no) {
    return no == NULL ? 0 : no->altura;
}

void atualiza_altura(no_t *no) {
    no->altura = 1 + MAX(altura(no->esq), altura(no->dir));
}

int fator_balanceamento(no_t *no) {
    if (no == NULL)
        return 0;
    return altura(no->esq) - altura(no->dir);
}


int comparar_chave(const char *nome, int matricula, Aluno *a) {
    int resultado = strcmp(nome, aluno_obter_nome(a));
    if (resultado != 0)
        return resultado;

    int outra = aluno_obter_matricula(a);
    if (matricula < outra)
        return -1;
    if (matricula > outra)
        return 1;
    return 0;
}

int comparar_alunos(const Aluno *a, Aluno *b) {
    return comparar_chave(aluno_obter_nome(a), aluno_obter_matricula(a), b);
}


no_t *cria_no(Aluno *aluno) {
    no_t *novo = malloc(sizeof(no_t));
    if (novo == NULL)
        return NULL;
    novo->aluno = aluno;
    novo->altura = 1;
    novo->esq = NULL;
    novo->dir = NULL;
    return novo;
}

no_t *rotacao_esq(no_t *no) {
    no_t *filho = no->dir;
    no_t *subarvore = filho->esq;

    filho->esq = no;
    no->dir = subarvore;

    atualiza_altura(no);     
    atualiza_altura(filho);  
    return filho;
}

no_t *rotacao_dir(no_t *no) {
    no_t *filho = no->esq;
    no_t *subarvore = filho->dir;

    filho->dir = no;
    no->esq = subarvore;

    atualiza_altura(no);
    atualiza_altura(filho);
    return filho;
}

no_t *rebalancear(no_t *no) {
    atualiza_altura(no);
    int fb = fator_balanceamento(no);

    if (fb > 1) {                                   
        if (fator_balanceamento(no->esq) < 0)       /* ED */
            no->esq = rotacao_esq(no->esq);
        return rotacao_dir(no);                     /* EE  */
    }

    if (fb < -1) {                                  
        if (fator_balanceamento(no->dir) > 0)       /* DE */
            no->dir = rotacao_dir(no->dir);
        return rotacao_esq(no);                     /* DD  */
    }

    return no;
}


AVL *avl_criar(int primeira_matricula) {
    AVL *arvore = malloc(sizeof(AVL));
    if (arvore == NULL)
        return NULL;
    arvore->raiz = NULL;
    arvore->proxima_matricula = primeira_matricula;
    arvore->tamanho = 0;
    return arvore;
}

void destruir_nos(no_t *no) {
    if (no == NULL)
        return;
    destruir_nos(no->esq);
    destruir_nos(no->dir);
    aluno_destruir(no->aluno);
    free(no);
}

void avl_destruir(AVL *arvore) {
    if (arvore == NULL)
        return;
    destruir_nos(arvore->raiz);
    free(arvore);
}

/*insercao  */

no_t *inserir_no(no_t *no, Aluno *aluno, int *inserido) {
    if (no == NULL) {
        no_t *novo = cria_no(aluno);
        if (novo != NULL)
            *inserido = 1;
        return novo;
    }

    int c = comparar_alunos(aluno, no->aluno);
    if (c < 0)
        no->esq = inserir_no(no->esq, aluno, inserido);
    else if (c > 0)
        no->dir = inserir_no(no->dir, aluno, inserido);
    else
        return no;              /*  se chave duplicada: nao insere */

    return rebalancear(no);
}

int avl_cadastrar(AVL *arvore,const char *nome) {
    if (arvore == NULL || nome == NULL || nome[0] == '\0')
        return -1;

    int matricula = arvore->proxima_matricula;
    Aluno *novo = aluno_criar(nome, matricula);
    if (novo == NULL)
        return -1;

    int inserido = 0;
    arvore->raiz = inserir_no(arvore->raiz, novo, &inserido);

    if (!inserido) {            
        aluno_destruir(novo);
        return -1;
    }

    arvore->proxima_matricula++;    
    arvore->tamanho++;
    return matricula;
}

/* remocao */

no_t *menor_no(no_t *no) {
    while (no->esq != NULL)
        no = no->esq;
    return no;
}

no_t *remover_no(no_t *no, const char *nome, int matricula, int *removido) {
    if (no == NULL)
        return NULL;

    int c = comparar_chave(nome, matricula, no->aluno);

    if (c < 0) {
        no->esq = remover_no(no->esq, nome, matricula, removido);
    } else if (c > 0) {
        no->dir = remover_no(no->dir, nome, matricula, removido);
    } else {
        *removido = 1;

        if (no->esq == NULL) {         
            no_t *aux = no->dir;
            aluno_destruir(no->aluno);
            free(no);
            return aux;
        }
        if (no->dir == NULL) {          
            no_t *aux = no->esq;
            aluno_destruir(no->aluno);
            free(no);
            return aux;
        }

        
        no_t *sucessor = menor_no(no->dir);
        Aluno *a_remover = no->aluno;
        no->aluno = sucessor->aluno;
        sucessor->aluno = a_remover;

        no->dir = remover_no(no->dir,aluno_obter_nome(a_remover),aluno_obter_matricula(a_remover),removido);
    }

    return rebalancear(no);
}

int avl_remover(AVL *arvore, const char *nome, int matricula) {
    if (arvore == NULL || nome == NULL)
        return 0;

    int removido = 0;
    arvore->raiz = remover_no(arvore->raiz, nome, matricula, &removido);
    if (removido)
        arvore->tamanho--;
    return removido;
}


int buscar_no(no_t *no, const char *nome, VisitarAluno visitar) {
    if (no == NULL)
        return 0;

    int c = strcmp(nome, aluno_obter_nome(no->aluno));

    if (c < 0)
        return buscar_no(no->esq, nome, visitar);
    if (c > 0)
        return buscar_no(no->dir, nome, visitar);

    int encontrados = buscar_no(no->esq, nome, visitar);
    visitar(no->aluno);
    encontrados++;
    encontrados += buscar_no(no->dir, nome, visitar);
    return encontrados;
}

int avl_buscar_nome(AVL *arvore, const char *nome, VisitarAluno visitar) {
    if (arvore == NULL || nome == NULL || visitar == NULL)
        return 0;
    return buscar_no(arvore->raiz, nome, visitar);
}

void listar_no(no_t *no, VisitarAluno visitar) {
    if (no == NULL)
        return;
    listar_no(no->esq, visitar);
    visitar(no->aluno);
    listar_no(no->dir, visitar);
}

void avl_listar_em_ordem(AVL *arvore, VisitarAluno visitar) {
    if (arvore == NULL || visitar == NULL)
        return;
    listar_no(arvore->raiz, visitar);
}


int avl_proxima_matricula(AVL *arvore) {
    return arvore == NULL ? -1 : arvore->proxima_matricula;
}

int avl_tamanho(AVL *arvore) {
    return arvore == NULL ? 0 : arvore->tamanho;
}

int avl_altura(AVL *arvore) {
    return arvore == NULL ? 0 : altura(arvore->raiz);
}

/* Devolve a altura real da subarvore, ou -1 se algo estiver errado. */
static int verifica_no(no_t *no) {
    if (no == NULL)
        return 0;

    int he = verifica_no(no->esq);
    int hd = verifica_no(no->dir);
    if (he < 0 || hd < 0)
        return -1;
    if (he - hd > 1 || hd - he > 1)
        return -1;                          
    if (no->altura != 1 + MAX(he, hd))
        return -1;                          
    return 1 + MAX(he, hd);
}

int avl_esta_balanceada(AVL *arvore) {
    return arvore != NULL && verifica_no(arvore->raiz) >= 0;
}
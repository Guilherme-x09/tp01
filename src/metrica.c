#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avl.h"

/* ---------- BST minima (somente insercao e altura) ---------- */

typedef struct NoBST {
    char nome[16];
    struct NoBST *esq;
    struct NoBST *dir;
} NoBST;

static NoBST *bst_inserir(NoBST *raiz, const char *nome)
{
    NoBST *novo = malloc(sizeof(NoBST));
    if (novo == NULL)
        exit(EXIT_FAILURE);
    strcpy(novo->nome, nome);
    novo->esq = NULL;
    novo->dir = NULL;

    if (raiz == NULL)
        return novo;

    NoBST *atual = raiz;
    for (;;) {                                  /* iterativo: BST degenerada e funda */
        if (strcmp(nome, atual->nome) < 0) {
            if (atual->esq == NULL) { atual->esq = novo; break; }
            atual = atual->esq;
        } else {
            if (atual->dir == NULL) { atual->dir = novo; break; }
            atual = atual->dir;
        }
    }
    return raiz;
}

static int bst_altura(NoBST *no)
{
    if (no == NULL)
        return 0;
    int he = bst_altura(no->esq);
    int hd = bst_altura(no->dir);
    return 1 + (he > hd ? he : hd);
}

static void bst_destruir(NoBST *no)
{
    if (no == NULL)
        return;
    bst_destruir(no->esq);
    bst_destruir(no->dir);
    free(no);
}

/* ---------- limites teoricos (so aritmetica inteira) ---------- */

/* Menor altura possivel de qualquer arvore binaria com n nos: ceil(log2(n+1)) */
static int altura_minima(int n)
{
    int h = 0;
    while ((1L << h) < (long)n + 1)
        h++;
    return h;
}

/* Maior altura possivel de uma AVL com n nos.
 * Menor numero de nos de uma AVL de altura h: N(h) = N(h-1) + N(h-2) + 1. */
static int altura_maxima_avl(int n)
{
    long anterior = 0, atual = 1;       /* N(0) = 0, N(1) = 1 */
    int h = 1;
    for (;;) {
        long proximo = atual + anterior + 1;
        if (proximo > n)
            return h;
        anterior = atual;
        atual = proximo;
        h++;
    }
}

/* ---------- experimento ---------- */

static void embaralhar(int v[], int n)
{
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

static void medir(int n, int aleatoria, int tentativas)
{
    char (*nomes)[16] = malloc((size_t)n * sizeof(*nomes));
    int *ordem = malloc((size_t)n * sizeof(int));
    if (nomes == NULL || ordem == NULL)
        exit(EXIT_FAILURE);

    for (int i = 0; i < n; i++)
        snprintf(nomes[i], sizeof(nomes[i]), "N%06d", i);

    double soma_bst = 0, soma_avl = 0;
    for (int t = 0; t < tentativas; t++) {
        for (int i = 0; i < n; i++)
            ordem[i] = i;
        if (aleatoria) {
            srand(42 + t);              /* sementes fixas: resultado reproduzivel */
            embaralhar(ordem, n);
        }

        NoBST *bst = NULL;
        AVL *avl = avl_criar(1001);
        for (int k = 0; k < n; k++) {
            bst = bst_inserir(bst, nomes[ordem[k]]);
            avl_cadastrar(avl, nomes[ordem[k]]);
        }
        soma_bst += bst_altura(bst);
        soma_avl += avl_altura(avl);
        bst_destruir(bst);
        avl_destruir(avl);
    }

    printf("%7d  %-10s  %10.1f  %10.1f  %8d  %8d\n",
           n, aleatoria ? "aleatoria" : "crescente",
           soma_bst / tentativas, soma_avl / tentativas,
           altura_minima(n), altura_maxima_avl(n));

    free(nomes);
    free(ordem);
}

int main(void)
{
    const int tamanhos[] = {100, 1000, 10000};

    printf("Altura da arvore apos n insercoes (aleatoria = media de 10 sequencias)\n\n");
    printf("%7s  %-10s  %10s  %10s  %8s  %8s\n",
           "n", "ordem", "BST", "AVL", "minima", "max AVL");
    printf("-------  ----------  ----------  ----------  --------  --------\n");

    for (int i = 0; i < 3; i++) {
        medir(tamanhos[i], 0, 1);
        medir(tamanhos[i], 1, 10);
    }

    printf("\nminima  = ceil(log2(n+1)): melhor altura que qualquer arvore binaria pode ter\n");
    printf("max AVL = pior altura que uma AVL com n nos pode ter\n");
    return 0;
}
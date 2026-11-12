#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avl.h"

#define BASE 1001           /* primeira matricula usada nos testes */
#define MAX_COLETA 2048

/* ------------------------------------------------------------------
 * Como testar um TAD opaco: o TAD chama a funcao 'coletar' para cada
 * aluno visitado e a gente guarda o que chegou. Depois e so conferir.
 * ------------------------------------------------------------------ */
static char nomes_coletados[MAX_COLETA][100];
static int  mats_coletadas[MAX_COLETA];
static int  total_coletado;

static void coletar(Aluno *a)
{
    if (total_coletado < MAX_COLETA) {
        strncpy(nomes_coletados[total_coletado], aluno_obter_nome(a), 99);
        nomes_coletados[total_coletado][99] = '\0';
        mats_coletadas[total_coletado] = aluno_obter_matricula(a);
        total_coletado++;
    }
}

static void zerar_coleta(void)
{
    total_coletado = 0;
}

/* Cria a arvore e cadastra os nomes na ordem dada.
 * O aluno de indice i recebe a matricula BASE + i. */
static AVL *montar(const char *nomes[], int n)
{
    AVL *t = avl_criar(BASE);
    assert(t != NULL);
    for (int i = 0; i < n; i++)
        assert(avl_cadastrar(t, nomes[i]) == BASE + i);
    return t;
}

/* Confere o percurso em ordem contra a lista esperada de nomes. */
static void conferir_ordem(AVL *t, const char *esperado[], int n)
{
    zerar_coleta();
    avl_listar_em_ordem(t, coletar);
    assert(total_coletado == n);
    for (int i = 0; i < n; i++)
        assert(strcmp(nomes_coletados[i], esperado[i]) == 0);
}

static void embaralhar(int v[], int n)
{
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

/* ================= 1. CASOS COMUNS (todos os TADs) ================= */

static void test_estrutura_vazia(void)
{
    AVL *t = avl_criar(BASE);
    assert(t != NULL);
    assert(avl_tamanho(t) == 0);
    assert(avl_altura(t) == 0);
    assert(avl_esta_balanceada(t));
    assert(avl_proxima_matricula(t) == BASE);

    zerar_coleta();
    assert(avl_buscar_nome(t, "Ana", coletar) == 0);
    avl_listar_em_ordem(t, coletar);
    assert(total_coletado == 0);
    assert(avl_remover(t, "Ana", BASE) == 0);

    /* entradas invalidas nao podem derrubar o programa nem gastar matricula */
    assert(avl_cadastrar(t, "") == -1);
    assert(avl_cadastrar(t, NULL) == -1);
    assert(avl_cadastrar(NULL, "Ana") == -1);
    assert(avl_proxima_matricula(t) == BASE);

    avl_destruir(t);
    avl_destruir(NULL);     /* destruir NULL deve ser seguro */
}

static void test_insercao_e_matricula_automatica(void)
{
    const char *nomes[] = {"Maria", "Ana", "Bruno"};
    AVL *t = montar(nomes, 3);       /* montar() ja confere 1001, 1002, 1003 */

    assert(avl_tamanho(t) == 3);
    assert(avl_proxima_matricula(t) == BASE + 3);

    const char *ordem[] = {"Ana", "Bruno", "Maria"};
    conferir_ordem(t, ordem, 3);
    assert(mats_coletadas[0] == BASE + 1);   /* Ana */
    assert(mats_coletadas[1] == BASE + 2);   /* Bruno */
    assert(mats_coletadas[2] == BASE + 0);   /* Maria */
    assert(avl_esta_balanceada(t));
    avl_destruir(t);
}

static void test_busca_existente_e_inexistente(void)
{
    const char *nomes[] = {"Maria", "Ana", "Bruno"};
    AVL *t = montar(nomes, 3);

    zerar_coleta();
    assert(avl_buscar_nome(t, "Ana", coletar) == 1);
    assert(mats_coletadas[0] == BASE + 1);

    zerar_coleta();
    assert(avl_buscar_nome(t, "Zeca", coletar) == 0);
    assert(total_coletado == 0);

    /* comportamento documentado: a busca diferencia maiusculas de minusculas */
    assert(avl_buscar_nome(t, "ana", coletar) == 0);
    /* busca por prefixo NAO e suportada */
    assert(avl_buscar_nome(t, "Mar", coletar) == 0);
    avl_destruir(t);
}

static void test_remocoes_basicas(void)
{
    /* arvore completa de 7 nos; matriculas: Dora=1001 Bia=1002 Gil=1003
     * Ana=1004 Caio=1005 Eva=1006 Hugo=1007 */
    const char *nomes[] = {"Dora", "Bia", "Gil", "Ana", "Caio", "Eva", "Hugo"};
    AVL *t = montar(nomes, 7);
    assert(avl_altura(t) == 3);

    /* remocoes que nao devem remover nada */
    assert(avl_remover(t, "Zeca", 999) == 0);
    assert(avl_remover(t, "Ana", 9999) == 0);     /* nome certo, matricula errada */
    assert(avl_tamanho(t) == 7);

    /* folha */
    assert(avl_remover(t, "Ana", BASE + 3) == 1);
    zerar_coleta();
    assert(avl_buscar_nome(t, "Ana", coletar) == 0);
    assert(avl_tamanho(t) == 6 && avl_esta_balanceada(t));

    /* no com um filho (Bia so tem Caio agora) */
    assert(avl_remover(t, "Bia", BASE + 1) == 1);
    assert(avl_buscar_nome(t, "Caio", coletar) == 1);
    assert(avl_tamanho(t) == 5 && avl_esta_balanceada(t));

    /* no com dois filhos (Gil tem Eva e Hugo) */
    assert(avl_remover(t, "Gil", BASE + 2) == 1);
    assert(avl_buscar_nome(t, "Eva", coletar) == 1);
    assert(avl_buscar_nome(t, "Hugo", coletar) == 1);
    assert(avl_tamanho(t) == 4 && avl_esta_balanceada(t));

    /* remover a raiz */
    assert(avl_remover(t, "Dora", BASE + 0) == 1);
    const char *ordem[] = {"Caio", "Eva", "Hugo"};
    conferir_ordem(t, ordem, 3);
    assert(avl_esta_balanceada(t));

    /* a proxima matricula nao voltou atras com as remocoes */
    assert(avl_proxima_matricula(t) == BASE + 7);

    /* esvazia e confere */
    assert(avl_remover(t, "Caio", BASE + 4) == 1);
    assert(avl_remover(t, "Eva", BASE + 5) == 1);
    assert(avl_remover(t, "Hugo", BASE + 6) == 1);
    assert(avl_tamanho(t) == 0 && avl_altura(t) == 0);
    assert(avl_remover(t, "Hugo", BASE + 6) == 0);
    avl_destruir(t);
}

/* ================= 2. CASOS DA APLICACAO ================= */

static void test_homonimos(void)
{
    const char *nomes[] = {"Maria Silva", "Ana", "Maria Silva", "Zeca",
                           "Maria Silva", "Bia", "Maria Silva"};
    AVL *t = montar(nomes, 7);       /* Marias: 1001, 1003, 1005, 1007 */

    zerar_coleta();
    assert(avl_buscar_nome(t, "Maria Silva", coletar) == 4);
    assert(mats_coletadas[0] == 1001);          /* saem em ordem de matricula */
    assert(mats_coletadas[1] == 1003);
    assert(mats_coletadas[2] == 1005);
    assert(mats_coletadas[3] == 1007);

    /* remover UMA Maria nao pode afetar as outras */
    assert(avl_remover(t, "Maria Silva", 1003) == 1);
    zerar_coleta();
    assert(avl_buscar_nome(t, "Maria Silva", coletar) == 3);
    assert(mats_coletadas[0] == 1001 && mats_coletadas[1] == 1005
           && mats_coletadas[2] == 1007);
    assert(avl_esta_balanceada(t));
    avl_destruir(t);
}

static void test_matricula_nao_e_reutilizada(void)
{
    const char *nomes[] = {"Ana", "Bia", "Caio"};
    AVL *t = montar(nomes, 3);

    assert(avl_remover(t, "Caio", BASE + 2) == 1);   /* remove a ultima matricula */
    assert(avl_proxima_matricula(t) == BASE + 3);    /* continua apontando para 1004 */
    assert(avl_cadastrar(t, "Duda") == BASE + 3);    /* Duda NAO herda a 1003 */
    assert(avl_proxima_matricula(t) == BASE + 4);
    avl_destruir(t);
}

/* ================= 3. CASOS ESPECIFICOS DA AVL ================= */
/* Sem acesso aos nos, a prova da rotacao e: 3 nos so cabem em altura 2
 * se a arvore foi rotacionada (sem rotacao, a altura seria 3). */

static void test_quatro_rotacoes_de_insercao(void)
{
    const char *ll[] = {"C", "B", "A"};
    const char *rr[] = {"A", "B", "C"};
    const char *lr[] = {"C", "A", "B"};
    const char *rl[] = {"A", "C", "B"};
    const char **casos[] = {ll, rr, lr, rl};
    const char *ordem[] = {"A", "B", "C"};

    for (int i = 0; i < 4; i++) {
        AVL *t = montar(casos[i], 3);
        assert(avl_altura(t) == 2);
        assert(avl_esta_balanceada(t));
        conferir_ordem(t, ordem, 3);
        avl_destruir(t);
    }
}

typedef struct {
    const char *nomes[5];
    int n;
    int remover;            /* indice (ordem de insercao) de quem sera removido */
    int altura_final;
    const char *ordem[4];
    int n_final;
} CasoRemocao;

static void test_quatro_rotacoes_de_remocao(void)
{
    /* A remocao deixa a arvore desbalanceada; so passa se rebalancear. */
    CasoRemocao casos[] = {
        /* LL: remove G (dir) -> E fica pesado a esquerda, filho C equilibrado */
        {{"E", "C", "G", "A", "D"}, 5, 2, 3, {"A", "C", "D", "E"}, 4},
        /* LR: remove G -> E pesado a esquerda, filho C pende a direita */
        {{"E", "C", "G", "D"},      4, 2, 2, {"C", "D", "E"},      3},
        /* RR: remove C (esq) -> E pesado a direita, filho G equilibrado */
        {{"E", "C", "G", "F", "H"}, 5, 1, 3, {"E", "F", "G", "H"}, 4},
        /* RL: remove C -> E pesado a direita, filho G pende a esquerda */
        {{"E", "C", "G", "F"},      4, 1, 2, {"E", "F", "G"},      3},
    };

    for (int i = 0; i < 4; i++) {
        CasoRemocao *c = &casos[i];
        AVL *t = montar(c->nomes, c->n);
        assert(avl_remover(t, c->nomes[c->remover], BASE + c->remover) == 1);
        assert(avl_esta_balanceada(t));
        assert(avl_altura(t) == c->altura_final);
        assert(avl_tamanho(t) == c->n_final);
        conferir_ordem(t, c->ordem, c->n_final);
        avl_destruir(t);
    }
}

/* ================= 4. ESTRESSE (comportamento em escala) ================= */

#define N_ORDENADO 1000

static void test_entrada_ordenada(void)
{
    static char nomes[N_ORDENADO][8];
    AVL *t = avl_criar(BASE);

    for (int i = 0; i < N_ORDENADO; i++) {
        snprintf(nomes[i], sizeof(nomes[i]), "N%04d", i);
        assert(avl_cadastrar(t, nomes[i]) == BASE + i);
    }
    assert(avl_esta_balanceada(t));
    /* limite teorico da AVL: ~1,44 * log2(n+2) = 14 para n = 1000 */
    assert(avl_altura(t) <= 14);
    printf("   (%d nomes em ordem crescente -> altura %d; uma BST teria %d)\n",
           N_ORDENADO, avl_altura(t), N_ORDENADO);

    /* remove os de indice par, conferindo o balanceamento a cada remocao */
    for (int i = 0; i < N_ORDENADO; i += 2) {
        assert(avl_remover(t, nomes[i], BASE + i) == 1);
        assert(avl_esta_balanceada(t));
    }
    assert(avl_tamanho(t) == N_ORDENADO / 2);
    avl_destruir(t);        /* destroi com nos ainda dentro: o ASan confere vazamento */
}

#define N_ALEATORIO 500

static void test_ordem_aleatoria(void)
{
    static char nomes[N_ALEATORIO][8];
    int mat[N_ALEATORIO], ordem[N_ALEATORIO];

    for (int i = 0; i < N_ALEATORIO; i++) {
        snprintf(nomes[i], sizeof(nomes[i]), "N%04d", i);
        ordem[i] = i;
    }

    srand(42);              /* semente fixa: o teste e reproduzivel */
    embaralhar(ordem, N_ALEATORIO);

    AVL *t = avl_criar(BASE);
    for (int k = 0; k < N_ALEATORIO; k++) {
        int i = ordem[k];
        mat[i] = avl_cadastrar(t, nomes[i]);
        assert(mat[i] == BASE + k);
        assert(avl_esta_balanceada(t));
    }
    assert(avl_tamanho(t) == N_ALEATORIO);

    /* a listagem tem que sair em ordem alfabetica estrita */
    zerar_coleta();
    avl_listar_em_ordem(t, coletar);
    assert(total_coletado == N_ALEATORIO);
    for (int i = 1; i < total_coletado; i++)
        assert(strcmp(nomes_coletados[i - 1], nomes_coletados[i]) < 0);

    /* remove tudo em outra ordem aleatoria */
    embaralhar(ordem, N_ALEATORIO);
    for (int k = 0; k < N_ALEATORIO; k++) {
        int i = ordem[k];
        assert(avl_remover(t, nomes[i], mat[i]) == 1);
        assert(avl_esta_balanceada(t));
    }
    assert(avl_tamanho(t) == 0 && avl_altura(t) == 0);
    avl_destruir(t);
}

#define RODAR(teste) do { teste(); printf("[OK] %s\n", #teste); } while (0)

int main(void)
{
    RODAR(test_estrutura_vazia);
    RODAR(test_insercao_e_matricula_automatica);
    RODAR(test_busca_existente_e_inexistente);
    RODAR(test_remocoes_basicas);
    RODAR(test_homonimos);
    RODAR(test_matricula_nao_e_reutilizada);
    RODAR(test_quatro_rotacoes_de_insercao);
    RODAR(test_quatro_rotacoes_de_remocao);
    RODAR(test_entrada_ordenada);
    RODAR(test_ordem_aleatoria);
    printf("\nTodos os testes passaram!\n");
    return 0;
}
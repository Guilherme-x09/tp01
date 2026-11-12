#include <stdlib.h>
#include <string.h>

#include "aluno.h"

struct Aluno {
    char *nome;        
    int matricula;
};

Aluno *aluno_criar(const char *nome, int matricula) {
    if (nome == NULL || nome[0] == '\0')
        return NULL;

    Aluno *aluno = malloc(sizeof(Aluno));
    if (aluno == NULL)
        return NULL;

    size_t tamanho = strlen(nome) + 1;      /* +1 para o '\0' */
    aluno->nome = malloc(tamanho);
    if (aluno->nome == NULL) {
        free(aluno);                        
        return NULL;
    }

    memcpy(aluno->nome, nome, tamanho);
    aluno->matricula = matricula;
    return aluno;
}

void aluno_destruir(Aluno *aluno) {
    if (aluno == NULL)
        return;
    free(aluno->nome);      
    free(aluno);           
}

const char *aluno_obter_nome(const Aluno *aluno) {
    return aluno == NULL ? NULL : aluno->nome;
}

int aluno_obter_matricula(const Aluno *aluno) {
    return aluno == NULL ? -1 : aluno->matricula;
}
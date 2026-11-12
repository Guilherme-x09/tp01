#ifndef ALUNO_H
#define ALUNO_H


typedef struct Aluno Aluno;
Aluno *aluno_criar(const char *nome, int matricula);
void aluno_destruir(Aluno *aluno);
const char *aluno_obter_nome(const Aluno *aluno);
int aluno_obter_matricula(const Aluno *aluno);

#endif
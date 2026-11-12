
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl.h"
 
#define MATRICULA_INICIAL 1001
#define TAM_NOME 100
 
enum menu_opcoes {
    SAIR = 0,
    CADASTRAR = 1,
    BUSCAR = 2,
    REMOVER = 3,
    LISTAR = 4,
    PROXIMA = 5,
    ESTATISTICAS = 6
};
 
static void imprimir_menu(void) {
    printf("\n");
    printf("==================================\n");
    printf("     CADASTRO DE ALUNOS (AVL)\n");
    printf("==================================\n");
    printf("1 - Cadastrar aluno\n");
    printf("2 - Buscar matricula pelo nome\n");
    printf("3 - Remover aluno\n");
    printf("4 - Listar alunos (ordem alfabetica)\n");
    printf("5 - Proxima matricula disponivel\n");
    printf("6 - Altura e total de alunos\n");
    printf("0 - Sair\n");
    printf("==================================\n");
}
 
int ler_linha(char *prompt, char *buffer, size_t tamanho) {
    printf("%s", prompt);
    if (fgets(buffer, (int)tamanho, stdin) == NULL) {
        buffer[0] = '\0';
        return 0;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}
 
int ler_inteiro(char *prompt, int *valor) {
    char linha[32];
    if (!ler_linha(prompt, linha, sizeof(linha)))
        return 0;
    return sscanf(linha, "%d", valor) == 1;
}
 
void imprimir_aluno(Aluno *aluno) {
    printf("  Matricula: %d | Nome: %s\n",
           aluno_obter_matricula(aluno),
           aluno_obter_nome(aluno));
}
 
int main(void) {
    AVL *cadastro = avl_criar(MATRICULA_INICIAL);
    if (cadastro == NULL) {
        printf("Erro ao criar o cadastro.\n");
        return EXIT_FAILURE;
    }
 
    int opcao = -1;
    char nome[TAM_NOME];
 
    do {
        imprimir_menu();
 
        if (!ler_inteiro("Opcao: ", &opcao)) {
            if (feof(stdin))
                break;
            opcao = -1;
        }
 
        switch (opcao) {
 
            case CADASTRAR: {
                ler_linha("Nome do aluno: ", nome, sizeof(nome));
                int matricula = avl_cadastrar(cadastro, nome);
                if (matricula < 0)
                    printf("Erro: nome vazio ou falha ao cadastrar.\n");
                else
                    printf("Aluno cadastrado! Matricula: %d\n", matricula);
                break;
            }
 
            case BUSCAR: {
                ler_linha("Nome para buscar: ", nome, sizeof(nome));
                int total = avl_buscar_nome(cadastro, nome, imprimir_aluno);
                if (total == 0)
                    printf("Nenhum aluno encontrado com esse nome.\n");
                else
                    printf("%d aluno(s) encontrado(s).\n", total);
                break;
            }
 
            case REMOVER: {
                int matricula;
                ler_linha("Nome do aluno: ", nome, sizeof(nome));
                if (!ler_inteiro("Matricula: ", &matricula)) {
                    printf("Matricula invalida.\n");
                    break;
                }
                if (avl_remover(cadastro, nome, matricula))
                    printf("Aluno removido. A matricula %d nao sera reutilizada.\n",
                           matricula);
                else
                    printf("Aluno nao encontrado.\n");
                break;
            }
 
            case LISTAR:
                if (avl_tamanho(cadastro) == 0)
                    printf("Cadastro vazio.\n");
                else
                    avl_listar_em_ordem(cadastro, imprimir_aluno);
                break;
 
            case PROXIMA:
                printf("Proxima matricula disponivel: %d\n",
                       avl_proxima_matricula(cadastro));
                break;
 
            case ESTATISTICAS:
                printf("Total de alunos: %d\n", avl_tamanho(cadastro));
                printf("Altura da arvore: %d\n", avl_altura(cadastro));
                break;
 
            case SAIR:
                printf("Encerrando programa.\n");
                break;
 
            default:
                printf("Opcao invalida.\n");
        }
 
    } while (opcao != SAIR);
 
    avl_destruir(cadastro);
    return EXIT_SUCCESS;
}
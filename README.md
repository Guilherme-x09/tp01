# Tp1_aeds_2

# Cadastro de Alunos com Busca por Nome (Árvore AVL)

**Disciplina:** Algoritmos e Estruturas de Dados II (UFV, Campus Florestal)
**Integrantes:** Guilherme Ximenes Silva 


## 1. Problema e aplicação

Em uma instituição de ensino, é comum precisar descobrir a **matrícula de um aluno pelo nome** e saber **qual matrícula usar no próximo cadastro**. A aplicação é um cadastro que:

- cadastra alunos com matrícula automática (a partir de 1001);
- busca pelo nome, mostrando **todos** os homônimos;
- remove alunos e lista o cadastro em ordem alfabética;
- informa a próxima matrícula, o total de alunos e a altura da árvore.

## 2. Estrutura de dados e justificativa

**Árvore AVL**, implementada com chave **(nome, matrícula)**: compara-se o nome com `strcmp` e, se for igual, desempata-se pela matrícula.

- **Operações principais:** busca por nome, cadastro e listagem alfabética. A árvore de busca atende as três (o percurso em ordem já devolve a lista ordenada).
- **Por que não BST:** a altura depende da ordem de inserção. Com entrada ordenada, a BST vira uma lista (busca O(n)). A AVL garante O(log n) em qualquer ordem.
- **Por que não Hash:** busca em O(1) em média, mas não mantém ordem. Listar em ordem alfabética exigiria ordenar tudo a cada vez.
- **Custo da AVL:** guardar a altura em cada nó e rotacionar após inserir e remover.
- **Prefixos ou bits das chaves:** não são necessários, então a Patricia não traz vantagem.

**Decisões de projeto:**
- A struct `AVL` guarda a **próxima matrícula**. Ela é usada no cadastro e depois incrementada, e **nunca é reutilizada**, mesmo após remoção (a matrícula está ligada ao histórico do aluno).
- Na busca, quando o nome do nó é igual ao procurado, a busca desce **pelos dois lados**: homônimos de matrícula menor ficam à esquerda e os de matrícula maior, à direita.
- As structs `AVL` e `Aluno` são opacas, e o TAD não imprime nada: a aplicação passa uma função que é chamada para cada aluno visitado.

## 3. Operações implementadas

| Operação | Função | Complexidade |
|---|---|---|
| Criar / destruir | `avl_criar`, `avl_destruir` | O(1) / O(n) |
| Cadastrar (matrícula automática) | `avl_cadastrar` | O(log n) |
| Remover por (nome, matrícula) | `avl_remover` | O(log n) |
| Buscar por nome (todos os homônimos) | `avl_buscar_nome` | O(log n + k) |
| Listar em ordem alfabética | `avl_listar_em_ordem` | O(n) |
| Próxima matrícula, total, altura | `avl_proxima_matricula`, `avl_tamanho`, `avl_altura` | O(1) |

O rebalanceamento (`rebalancear`) trata os casos **LL, RR, LR e RL** na inserção e na remoção.

## 4. Organização dos arquivos

```
projeto/
├── include/   aluno.h, avl.h
├── src/       aluno.c, avl.c, main.c, metrica.c
├── testes/    testes.c
├── Makefile
└── README.md
```

## 5. Compilação e execução

Requisitos: `gcc` e `make` (Linux).

```bash
make               # compila app, teste e metrica
./app              # executa o cadastro
make test          # roda os testes
make asan          # testes com AddressSanitizer (detecta vazamento de memória)
make experimento   # roda o experimento da seção 7
make clean         # remove os executáveis
```

## 6. Testes

10 testes em `testes/testes.c` (usando `assert`), verificando tudo pela interface pública:

- árvore vazia e entradas inválidas;
- inserção, busca existente e inexistente;
- remoção de folha, nó com um filho, nó com dois filhos e raiz;
- homônimos e matrícula não reutilizada;
- os 4 casos de rotação (LL, RR, LR, RL), na inserção e na remoção;
- estresse: 1000 inserções ordenadas e 500 aleatórias, com o balanceamento conferido a cada passo (`avl_esta_balanceada`).

Com `make asan`, nenhum vazamento de memória ou acesso inválido foi reportado.

## 7. Resultados experimentais

**Métrica:** altura da árvore após *n* inserções, AVL do projeto contra uma BST simples sem balanceamento (existe só em `src/metrica.c`, para comparação). A altura é também o máximo de comparações de uma busca. A entrada aleatória é a média de 10 sequências, com sementes fixas.

| n | Ordem de inserção | Altura BST | Altura AVL |
|---:|---|---:|---:|
| 1000 | crescente | 1000 | 10 |
| 10000 | crescente | 10000 | 14 |
| 1000 | aleatória | 23,0 | 12,0 |
| 10000 | aleatória | 31,7 | 16,0 |

(A tabela completa, com n = 100 e os limites teóricos, sai de `make experimento`.)

**Análise:**
- Com **entrada ordenada**, a BST degenera em lista (altura = n), e a AVL fica em 14 para 10000 nomes, que é o mínimo possível. Isso é o O(n) contra O(log n) da teoria.
- Com **entrada aleatória**, a BST fica cerca de 2 vezes mais alta que a AVL, o que é bem menos grave. A vantagem da AVL é, sobretudo, garantir o desempenho mesmo quando a ordem de chegada é ruim (por exemplo, uma lista importada já ordenada).
- A altura da AVL ficou sempre dentro dos limites teóricos, o que é coerente com o rebalanceamento funcionando.

## 8. Limitações e melhorias

- Os dados não são gravados em arquivo: somem ao fechar o programa. 
- Melhoria: salvar e carregar de um arquivo.
- A busca não diferencia maiúsculas de minúsculas e acentos (`strcmp` compara bytes).
- Não há busca por prefixo nem por matrícula (seria O(n), pois a árvore é ordenada por nome).

## 9. Referências e uso de IA

- Material da disciplina AEDS II e documentação da função `strcmp`.
- Uso de IA generativa (Claude): como instrutor, para esclarecer conceitos e revisar o código, os testes e o experimento.
- 

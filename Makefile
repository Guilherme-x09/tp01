CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -Iinclude
LIB    = src/avl.c src/aluno.c

all: app teste metrica

app: src/main.c $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

# o executavel se chama "teste" (nao "testes", que e o nome da pasta)
teste: testes/testes.c $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

metrica: src/metrica.c $(LIB)
	$(CC) $(CFLAGS) $^ -o $@

# roda os testes
test: teste
	./teste

# roda os testes com AddressSanitizer: acusa vazamento e acesso invalido
asan: testes/testes.c $(LIB)
	$(CC) $(CFLAGS) -fsanitize=address,undefined $^ -o teste_asan
	./teste_asan

# roda o experimento da secao 8 (altura AVL x BST)
experimento: metrica
	./metrica

clean:
	rm -f app teste teste_asan metrica

.PHONY: all test asan experimento clean
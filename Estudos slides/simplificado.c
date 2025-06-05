#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct elemento {
    char nome[50];
    struct elemento* prox;
} Elemento;

typedef struct {
    Elemento* ultimo;
    int qtd;
} Lista;

void inicializar(Lista* l) {
    l->ultimo = NULL;
    l->qtd = 0;
}

void adicionar(FILE* out, Lista* l, char* nome) {
    if (strlen(nome) >= 50) {
        fprintf(out, "[FAILURE] ADD=%s\n", nome);
        return;
    }

    // Verifica duplicata
    if (l->ultimo) {
        Elemento* atual = l->ultimo->prox;
        do {
            if (strcmp(atual->nome, nome) == 0) {
                fprintf(out, "[FAILURE] ADD=%s\n", nome);
                return;
            }
            atual = atual->prox;
        } while (atual != l->ultimo->prox);
    }

    Elemento* novo = malloc(sizeof(Elemento));
    if (!novo) {
        fprintf(out, "[FAILURE] ADD=%s\n", nome);
        return;
    }

    strcpy(novo->nome, nome);

    if (!l->ultimo) {
        novo->prox = novo;
    } else {
        novo->prox = l->ultimo->prox;
        l->ultimo->prox = novo;
    }

    l->ultimo = novo;
    l->qtd++;
    fprintf(out, "[SUCCESS] ADD=%s\n", nome);
}

Elemento* remover(FILE* out, Lista* l, char* nome) {
    if (!l->ultimo) {
        fprintf(out, "[FAILURE] REMOVE=%s\n", nome);
        return NULL;
    }

    Elemento *atual = l->ultimo->prox, *ant = l->ultimo;

    do {
        if (strcmp(atual->nome, nome) == 0) {
            if (atual == l->ultimo && atual->prox == atual) {
                l->ultimo = NULL;
            } else {
                ant->prox = atual->prox;
                if (atual == l->ultimo)
                    l->ultimo = ant;
            }

            l->qtd--;
            fprintf(out, "[SUCCESS] REMOVE=%s\n", nome);
            return atual;
        }
        ant = atual;
        atual = atual->prox;
    } while (atual != l->ultimo->prox);

    fprintf(out, "[FAILURE] REMOVE=%s\n", nome);
    return NULL;
}

void mostrar(FILE* out, Lista* l, char* nome) {
    if (!l->ultimo) {
        fprintf(out, "[FAILURE] SHOW=?<-%s->?\n", nome);
        return;
    }

    Elemento *atual = l->ultimo->prox, *ant = l->ultimo;

    do {
        if (strcmp(atual->nome, nome) == 0) {
            fprintf(out, "[SUCCESS] SHOW=%s<-%s->%s\n", ant->nome, atual->nome, atual->prox->nome);
            return;
        }
        ant = atual;
        atual = atual->prox;
    } while (atual != l->ultimo->prox);

    fprintf(out, "[FAILURE] SHOW=?<-%s->?\n", nome);
}

void destruir(Lista* l) {
    if (!l->ultimo) return;

    Elemento* atual = l->ultimo->prox;
    Elemento* temp;

    while (atual != l->ultimo) {
        temp = atual;
        atual = atual->prox;
        free(temp);
    }

    free(l->ultimo);
    l->ultimo = NULL;
    l->qtd = 0;
}

void tratarEspacos(char* dest, const char* src) {
    for (int i = 0; src[i] && i < 49; i++) {
        dest[i] = (src[i] == ' ') ? '_' : src[i];
        dest[i + 1] = '\0';
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }

    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen(argv[2], "w");
    if (!in || !out) return 1;

    Lista l;
    inicializar(&l);

    char linha[256], cmd[10], nome[50], nomeFormatado[50];

    while (fgets(linha, sizeof(linha), in)) {
        sscanf(linha, "%s %[^\n]", cmd, nome);
        tratarEspacos(nomeFormatado, nome);

        if (strcmp(cmd, "ADD") == 0) {
            adicionar(out, &l, nomeFormatado);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            Elemento* e = remover(out, &l, nomeFormatado);
            if (e) free(e);
        } else if (strcmp(cmd, "SHOW") == 0) {
            mostrar(out, &l, nomeFormatado);
        }
    }

    destruir(&l);
    fclose(in);
    fclose(out);
    return 0;
}

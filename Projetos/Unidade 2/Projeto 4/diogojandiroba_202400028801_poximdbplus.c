#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

typedef struct key {
    char nome[100];
    char hash[33];
    uint64_t tamanhoArq;
} key;

typedef struct node {
    key *chaves;
    struct node **ArrayPonteiros;
    int qtd_chaves_atual;
    bool is_folha;
    struct node *Prox;
    struct node *pai;
} node;

typedef struct BTreePlus {
    node *root;
    int ordem;
} BTreePlus;

node* cria_no_folha(int ordem, bool is_folha) {
    node* novo = (node*)malloc(sizeof(node));
    novo->is_folha = is_folha;
    novo->chaves = (key*)malloc(sizeof(key) * (ordem));
    novo->ArrayPonteiros = (node**)malloc(sizeof(node*) * ordem);
    novo->qtd_chaves_atual = 0;
    novo->Prox = NULL;
    novo->pai = NULL;
    return novo;
}

BTreePlus* inicia_arvore(int ordem) {
    BTreePlus *arvore = (BTreePlus*)malloc(sizeof(BTreePlus));
    arvore->ordem = ordem;
    arvore->root = cria_no_folha(ordem, true);
    return arvore;
}

node* encontra_pai(node* raiz, node* filho) {
    if (raiz == NULL || raiz->is_folha) return NULL;

    for (int i = 0; i <= raiz->qtd_chaves_atual; i++) {
        if (raiz->ArrayPonteiros[i] == filho) {
            return raiz;
        } else {
            node* pai = encontra_pai(raiz->ArrayPonteiros[i], filho);
            if (pai != NULL) return pai;
        }
    }
    return NULL;
}

node* search_exata(node* raiz, const char* chave, bool paraIntervalo) {
    node* atual = raiz;

    while (!atual->is_folha) {
        int i = 0;
        while (i < atual->qtd_chaves_atual && strcmp(chave, atual->chaves[i].hash) >= 0) {
            i++;
        }
        atual = atual->ArrayPonteiros[i];
    }

    if (paraIntervalo) {
        return atual;
    } else {
        for (int i = 0; i < atual->qtd_chaves_atual; i++) {
            if (strcmp(chave, atual->chaves[i].hash) == 0) {
                return atual;
            }
        }
        return NULL;
    }
}

node** search_intevalo(BTreePlus* arvore, char* chaveMin, char* chaveMax, int* acc) {
    char inicio[33], final[33];

    if (strcmp(chaveMin, chaveMax) > 0) {
        strcpy(inicio, chaveMax);
        strcpy(final, chaveMin);
    } else {
        strcpy(inicio, chaveMin);
        strcpy(final, chaveMax);
    }

    node* folha_atual = search_exata(arvore->root, inicio, true);
    int capacidade = 100;
    node** vetor_nodes = malloc(sizeof(node*) * capacidade);
    *acc = 0;

    while (folha_atual != NULL) {
        if (strcmp(folha_atual->chaves[0].hash, final) > 0) {
            break;
        }

        // Adiciona o nó folha se ele contém chaves no intervalo
        bool contem_chave_no_intervalo = false;
        for (int i = 0; i < folha_atual->qtd_chaves_atual; i++) {
            if (strcmp(folha_atual->chaves[i].hash, inicio) >= 0 &&
                strcmp(folha_atual->chaves[i].hash, final) <= 0) {
                contem_chave_no_intervalo = true;
                break;
            }
        }

        if (contem_chave_no_intervalo) {
            if (*acc == capacidade - 1) {
                capacidade *= 2;
                vetor_nodes = realloc(vetor_nodes, sizeof(node*) * capacidade);
            }
            vetor_nodes[*acc] = folha_atual;
            (*acc)++;
        }
        
        folha_atual = folha_atual->Prox;
    }

    vetor_nodes[*acc] = NULL;
    return vetor_nodes;
}

node* ordena_chaves(node* no) {
    key temp;
    for (int i = 0; i < no->qtd_chaves_atual; i++) {
        for (int j = 0; j < no->qtd_chaves_atual - 1 - i; j++) {
            if (strcmp(no->chaves[j].hash, no->chaves[j + 1].hash) > 0) {
                temp = no->chaves[j];
                no->chaves[j] = no->chaves[j + 1];
                no->chaves[j + 1] = temp;
            }
        }
    }
    return no;
}

void insereComEspaco(node* noParaSerADD, char* hash, char* nome, uint64_t tamanho) {
    int i = noParaSerADD->qtd_chaves_atual;
    strcpy(noParaSerADD->chaves[i].hash, hash);
    strcpy(noParaSerADD->chaves[i].nome, nome);
    noParaSerADD->chaves[i].tamanhoArq = tamanho;
    noParaSerADD->qtd_chaves_atual++;
    ordena_chaves(noParaSerADD);
}

void split(BTreePlus* arvore, node* nodeSelecionado, char* hash, char* nome, uint64_t tamanho, node** novo_irmao, key* chave_promovida) {
    key* novachave = malloc(sizeof(key));
    strcpy(novachave->hash, hash);
    strcpy(novachave->nome, nome);
    novachave->tamanhoArq = tamanho;

    int i = nodeSelecionado->qtd_chaves_atual;
    nodeSelecionado->chaves[i] = *novachave;
    nodeSelecionado->qtd_chaves_atual++;
    ordena_chaves(nodeSelecionado);

    int mediana = arvore->ordem / 2;
    *chave_promovida = nodeSelecionado->chaves[mediana];

    *novo_irmao = cria_no_folha(arvore->ordem, nodeSelecionado->is_folha);
    (*novo_irmao)->pai = nodeSelecionado->pai;

    int j = 0;
    for (int i = mediana; i < arvore->ordem; i++) {
        (*novo_irmao)->chaves[j++] = nodeSelecionado->chaves[i];
    }
    
    nodeSelecionado->qtd_chaves_atual = mediana;
    (*novo_irmao)->qtd_chaves_atual = j;

    (*novo_irmao)->Prox = nodeSelecionado->Prox;
    nodeSelecionado->Prox = *novo_irmao;
}

void inserir(BTreePlus* arvore, char* hash, char* nome, uint64_t tamanho) {
    node* noParaSerADD = search_exata(arvore->root, hash, true);

    if (noParaSerADD->qtd_chaves_atual < arvore->ordem - 1) {
        insereComEspaco(noParaSerADD, hash, nome, tamanho);
        return;
    } else {
        key chave_promovida;
        node* novo_irmao = NULL;

        split(arvore, noParaSerADD, hash, nome, tamanho, &novo_irmao, &chave_promovida);

        node* pai = noParaSerADD->pai;

        if (pai == NULL) {
            node* nova_raiz = cria_no_folha(arvore->ordem, false);
            nova_raiz->chaves[0] = chave_promovida;
            nova_raiz->ArrayPonteiros[0] = noParaSerADD;
            nova_raiz->ArrayPonteiros[1] = novo_irmao;
            nova_raiz->qtd_chaves_atual = 1;
            arvore->root = nova_raiz;
            noParaSerADD->pai = nova_raiz;
            novo_irmao->pai = nova_raiz;
            return;
        }

        while (pai != NULL) {
            if (pai->qtd_chaves_atual < arvore->ordem - 1) {
                int i = pai->qtd_chaves_atual - 1;
                while (i >= 0 && strcmp(chave_promovida.hash, pai->chaves[i].hash) < 0) {
                    pai->chaves[i+1] = pai->chaves[i];
                    pai->ArrayPonteiros[i+2] = pai->ArrayPonteiros[i+1];
                    i--;
                }
                pai->chaves[i+1] = chave_promovida;
                pai->ArrayPonteiros[i+2] = novo_irmao;
                pai->qtd_chaves_atual++;

                novo_irmao->pai = pai;
                break;
            } else {
                key chave_promovida_pai;
                node* novo_irmao_pai = cria_no_folha(arvore->ordem, false);

                key temp_chaves[arvore->ordem];
                node* temp_ponteiros[arvore->ordem + 1];

                memcpy(temp_chaves, pai->chaves, sizeof(key) * pai->qtd_chaves_atual);
                memcpy(temp_ponteiros, pai->ArrayPonteiros, sizeof(node*) * (pai->qtd_chaves_atual + 1));

                int i = pai->qtd_chaves_atual - 1;
                while (i >= 0 && strcmp(chave_promovida.hash, temp_chaves[i].hash) < 0) {
                    temp_chaves[i+1] = temp_chaves[i];
                    temp_ponteiros[i+2] = temp_ponteiros[i+1];
                    i--;
                }
                temp_chaves[i+1] = chave_promovida;
                temp_ponteiros[i+2] = novo_irmao;

                int mediana = arvore->ordem / 2;
                chave_promovida_pai = temp_chaves[mediana];

                pai->qtd_chaves_atual = mediana;
                memcpy(pai->chaves, temp_chaves, sizeof(key) * mediana);
                memcpy(pai->ArrayPonteiros, temp_ponteiros, sizeof(node*) * (mediana + 1));

                int j = 0;
                for (int k = mediana + 1; k < arvore->ordem; k++) {
                    novo_irmao_pai->chaves[j++] = temp_chaves[k];
                }
                novo_irmao_pai->qtd_chaves_atual = j;

                for (int k = mediana + 1; k <= arvore->ordem; k++) {
                    novo_irmao_pai->ArrayPonteiros[k - mediana - 1] = temp_ponteiros[k];
                }

                for (int k = 0; k <= novo_irmao_pai->qtd_chaves_atual; k++) {
                    if (novo_irmao_pai->ArrayPonteiros[k] != NULL)
                        novo_irmao_pai->ArrayPonteiros[k]->pai = novo_irmao_pai;
                }

                novo_irmao_pai->pai = pai->pai;

                chave_promovida = chave_promovida_pai;
                novo_irmao = novo_irmao_pai;
                pai = pai->pai;

                if (pai == NULL) {
                    node* nova_raiz = cria_no_folha(arvore->ordem, false);
                    nova_raiz->chaves[0] = chave_promovida;
                    nova_raiz->ArrayPonteiros[0] = noParaSerADD;
                    nova_raiz->ArrayPonteiros[1] = novo_irmao;
                    nova_raiz->qtd_chaves_atual = 1;
                    arvore->root = nova_raiz;
                    if (pai != NULL) pai->pai = nova_raiz;
                    novo_irmao->pai = nova_raiz;
                    break;
                }
            }
        }
    }
}

void formataLinhaOUT(char* comando, char* nome, uint64_t* tamanhoArquivo, char* hash, char* linha, char* min, char* max) {
    char temp_linha[500];
    strncpy(temp_linha, linha, 499);
    temp_linha[499] = '\0';

    char* token = strtok(temp_linha, " \n\r");
    if (token) {
        strcpy(comando, token);
    } else {
        comando[0] = '\0';
        return;
    }

    if (strcmp("INSERT", comando) == 0) {
        token = strtok(NULL, " \n\r");
        if (token) strcpy(nome, token);

        token = strtok(NULL, " \n\r");
        if (token) *tamanhoArquivo = strtoull(token, NULL, 10);

        token = strtok(NULL, " \n\r");
        if (token) strcpy(hash, token);

    } else if (strcmp("SELECT", comando) == 0) {
        token = strtok(NULL, " \n\r");
        if (!token) return;

        if (strcmp("RANGE", token) == 0) {
            strcpy(comando, "SELECT_RANGE");

            token = strtok(NULL, " \n\r");
            if (token) strcpy(min, token);

            token = strtok(NULL, " \n\r");
            if (token) strcpy(max, token);
        } else {
            strcpy(hash, token);
        }
    }
}

void formataLinhaIN(char* nome, uint64_t* tamanhoArquivo, char* hash, char* linha) {
    char* token = strtok(linha, " \n\r");
    if (token != NULL) strcpy(nome, token);
    token = strtok(NULL, " \n\r");
    if (token != NULL) *tamanhoArquivo = strtoull(token, NULL, 10);
    token = strtok(NULL, " \n\r");
    if (token != NULL) strcpy(hash, token);
}

void processamento(FILE *input, FILE *output) {
    int ordem, qtdArq, qtdOP;
    key chaveTemp;
    char linha[500];
    char comando[10] = "", nome[100] = "", hash[33] = "";
    char min[33], max[33];
    uint64_t tamanho = 0;
    int qtdNos;

    fscanf(input, "%d", &ordem);
    BTreePlus* arvore = inicia_arvore(ordem);
    
    fscanf(input, "%d", &qtdArq);
    fgetc(input); // Consume newline

    for (int i = 0; i < qtdArq; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            break;
        }
        formataLinhaIN(chaveTemp.nome, &chaveTemp.tamanhoArq, chaveTemp.hash, linha);
        inserir(arvore, chaveTemp.hash, chaveTemp.nome, chaveTemp.tamanhoArq);
    }

    fscanf(input, "%d", &qtdOP);
    fgetc(input);

    for (int i = 0; i < qtdOP; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            break;
        }
        formataLinhaOUT(comando, nome, &tamanho, hash, linha, min, max);

        if (strcmp("INSERT", comando) == 0) {
            strcpy(chaveTemp.nome, nome);
            strcpy(chaveTemp.hash, hash);
            chaveTemp.tamanhoArq = tamanho;
            inserir(arvore, hash, nome, tamanho);

        } else if (strcmp("SELECT", comando) == 0) {
            fprintf(output, "[%s]\n", hash);
            node* result_node = search_exata(arvore->root, hash, false);
            if (result_node != NULL) {
                // Imprime todas as chaves do nó onde a chave foi encontrada
                for (int j = 0; j < result_node->qtd_chaves_atual; j++) {
                    fprintf(output, "%s:size=%llu,hash=%s\n",
                        result_node->chaves[j].nome,
                        (unsigned long long)result_node->chaves[j].tamanhoArq,
                        result_node->chaves[j].hash);
                }
            } else {
                fprintf(output, "-\n");
            }
      } else if (strcmp("SELECT_RANGE", comando) == 0) {
    fprintf(output, "[%s,%s]\n", min, max);
    node** result_nodes = search_intevalo(arvore, min, max, &qtdNos);
    if (result_nodes == NULL || qtdNos == 0) {
        fprintf(output, "-\n");
    } else {
        // Imprime todas as chaves em todos os nós retornados
        for (int j = 0; j < qtdNos; j++) {
            for (int k = 0; k < result_nodes[j]->qtd_chaves_atual; k++) {
                fprintf(output, "%s:size=%llu,hash=%s\n",
                    result_nodes[j]->chaves[k].nome,
                    (unsigned long long)result_nodes[j]->chaves[k].tamanhoArq,
                    result_nodes[j]->chaves[k].hash);
            }
        }
    }
    free(result_nodes);
}
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }
    FILE* input = fopen(argv[1], "r");
    if (!input) { perror("Erro input"); return 1; }
    FILE* output = fopen(argv[2], "w");
    if (!output) { perror("Erro output"); fclose(input); return 1; }
    processamento(input, output);
    fclose(input);
    fclose(output);
    return 0;
}
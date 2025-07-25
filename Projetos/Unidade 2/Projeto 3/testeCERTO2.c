#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

// --- ESTRUTURAS ---
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
} node;

typedef struct BTree {
    node *root;
    int ordem;
} BTree;

// --- DECLARAÇÕES (OPCIONAL) ---
node* cria_no(int ordem, bool is_folha);
BTree* inicia_arvore(int ordem);
void insert_non_full(node *x, key k, int ordem);
void split_child(node *x, int i, int ordem);
void inserir_na_arvore(BTree* arvore, key nova_chave);
node* search(node* x, const char* chave);

// --- FUNÇÃO DE CRIAÇÃO DE NÓ ---
node* cria_no(int ordem, bool is_folha) {
    node* novo = (node*)malloc(sizeof(node));
    novo->is_folha = is_folha;
    novo->chaves = (key*)malloc(sizeof(key) * (ordem - 1));
    novo->ArrayPonteiros = (node**)malloc(sizeof(node*) * ordem);
    novo->qtd_chaves_atual = 0;
    return novo;
}

// --- FUNÇÃO DE INICIALIZAÇÃO DA ÁRVORE ---
BTree* inicia_arvore(int ordem) {
    BTree *arvore = (BTree*)malloc(sizeof(BTree));
    arvore->ordem = ordem;
    arvore->root = cria_no(ordem, true);
    return arvore;
}

// --- LÓGICA DE BUSCA ---
node* search(node* x, const char* chave) {
    int i = 0;
    while (i < x->qtd_chaves_atual && strcmp(chave, x->chaves[i].hash) > 0) {
        i++;
    }
    if (i < x->qtd_chaves_atual && strcmp(chave, x->chaves[i].hash) == 0) {
        return x;
    }
    if (x->is_folha) {
        return NULL;
    }
    return search(x->ArrayPonteiros[i], chave);
}

// --- LÓGICA DE INSERÇÃO ---
void split_child(node *x, int i, int ordem) {
    node *y = x->ArrayPonteiros[i];
    node *z = cria_no(ordem, y->is_folha);
    int min_degree = (ordem + 1) / 2;
    z->qtd_chaves_atual = min_degree - 1;
    for (int j = 0; j < min_degree - 1; j++) {
        z->chaves[j] = y->chaves[j + min_degree];
    }
    if (!y->is_folha) {
        for (int j = 0; j < min_degree; j++) {
            z->ArrayPonteiros[j] = y->ArrayPonteiros[j + min_degree];
        }
    }
    y->qtd_chaves_atual = min_degree - 1;
    for (int j = x->qtd_chaves_atual; j >= i + 1; j--) {
        x->ArrayPonteiros[j + 1] = x->ArrayPonteiros[j];
    }
    x->ArrayPonteiros[i + 1] = z;
    for (int j = x->qtd_chaves_atual - 1; j >= i; j--) {
        x->chaves[j + 1] = x->chaves[j];
    }
    x->chaves[i] = y->chaves[min_degree - 1];
    x->qtd_chaves_atual++;
}

void insert_non_full(node *x, key k, int ordem) {
    int i = x->qtd_chaves_atual - 1;
    if (x->is_folha) {
        while (i >= 0 && strcmp(k.hash, x->chaves[i].hash) < 0) {
            x->chaves[i + 1] = x->chaves[i];
            i--;
        }
        x->chaves[i + 1] = k;
        x->qtd_chaves_atual++;
    } else {
        while (i >= 0 && strcmp(k.hash, x->chaves[i].hash) < 0) {
            i--;
        }
        i++;
        if (x->ArrayPonteiros[i]->qtd_chaves_atual == ordem - 1) {
            split_child(x, i, ordem);
            if (strcmp(k.hash, x->chaves[i].hash) > 0) {
                i++;
            }
        }
        insert_non_full(x->ArrayPonteiros[i], k, ordem);
    }
}

void inserir_na_arvore(BTree* arvore, key nova_chave) {
    node *raiz = arvore->root;
    int ordem = arvore->ordem;
    if (raiz->qtd_chaves_atual == ordem - 1) {
        node *nova_raiz = cria_no(ordem, false);
        arvore->root = nova_raiz;
        nova_raiz->ArrayPonteiros[0] = raiz;
        split_child(nova_raiz, 0, ordem);
        insert_non_full(nova_raiz, nova_chave, ordem);
    } else {
        insert_non_full(raiz, nova_chave, ordem);
    }
}

// --- FUNÇÕES DE PROCESSAMENTO ---
void formataLinhaOUT(char* comando, char* nome, uint64_t* tamanhoArquivo, char* hash, char* linha) {
    char temp_linha[500];
    strncpy(temp_linha, linha, 499);
    temp_linha[499] = '\0';
    char* token = strtok(temp_linha, " \n\r");
    if (token) strcpy(comando, token);
    token = strtok(NULL, " \n\r");
    if (!token) return;
    if (strcmp("INSERT", comando) == 0) {
        strcpy(nome, token);
        token = strtok(NULL, " \n\r");
        if (token) *tamanhoArquivo = strtoull(token, NULL, 10);
        token = strtok(NULL, " \n\r");
        if (token) strcpy(hash, token);
    } else {
        strcpy(hash, token);
    }
}

// SUA FUNÇÃO DE FORMATAÇÃO
void formataLinhaIN(char* nome, uint64_t* tamanhoArquivo, char* hash, char* linha){
    char* token;
    token = strtok(linha," \n\r");
    if(token != NULL){
        strcpy(nome, token);
    }

    token = strtok(NULL," \n\r");
    if(token != NULL){
        *tamanhoArquivo = strtoull(token, NULL, 10);
    }

    token = strtok(NULL," \n\r");
    if(token != NULL){
        strcpy(hash, token);
    }
}

// FUNÇÃO DE PROCESSAMENTO MODIFICADA
void processamento(FILE* input, FILE* output) {
    int ordem, qtdArq, qtdOP;
    key chaveTemp;
    char linha[500];

    if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
    }
    if(sscanf(linha, "%d", &ordem) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
    }

    BTree* arvore = inicia_arvore(ordem);
    
   if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
    }
    if(sscanf(linha, "%d", &qtdArq) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
    }
    
    // Loop de leitura inicial usando sua função
    for (int i = 0; i < qtdArq; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            break; // Para se o arquivo acabar antes do esperado
        }
        // Pula linhas em branco que possam existir no arquivo
        if (strlen(linha) < 2) {
            i--; 
            continue;
        }
        formataLinhaIN(chaveTemp.nome, &chaveTemp.tamanhoArq, chaveTemp.hash, linha);
        inserir_na_arvore(arvore, chaveTemp);
    }

if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
if(sscanf(linha, "%d", &qtdOP) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
}



    // Loop de operações (INSERT/SELECT)
    for (int i = 0; i < qtdOP; i++) {
        if (!fgets(linha, sizeof(linha), input) || strlen(linha) < 2) {
            i--; continue;
        }

        char comando[10] = "", nome[100] = "", hash[33] = "";
        uint64_t tamanho = 0;

        formataLinhaOUT(comando, nome, &tamanho, hash, linha);

        if (strcmp("INSERT", comando) == 0) {
            strcpy(chaveTemp.nome, nome);
            strcpy(chaveTemp.hash, hash);
            chaveTemp.tamanhoArq = tamanho;
            inserir_na_arvore(arvore, chaveTemp);
        } else if (strcmp("SELECT", comando) == 0) {
            fprintf(output, "[%s]\n", hash);
            node* result_node = search(arvore->root, hash);
            if (result_node != NULL) {
                for (int j = 0; j < result_node->qtd_chaves_atual; j++) {
                     fprintf(output, "%s:size=%" PRIu64 ",hash=%s\n",
                         result_node->chaves[j].nome,
                         result_node->chaves[j].tamanhoArq,
                         result_node->chaves[j].hash);
                }
            } else {
                fprintf(output, "-\n");
            }
        }
    }
}

// --- FUNÇÃO MAIN ---
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
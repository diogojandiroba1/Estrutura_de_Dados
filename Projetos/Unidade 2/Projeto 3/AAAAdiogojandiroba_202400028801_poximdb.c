#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>


//estrutura da chave
typedef struct key
{
char nome[100];
char hash[33];
uint64_t tamanhoArq;
}key;



//estrutura do no
typedef struct node
{  
key *chaves;
struct node **ArrayPonteiros;
uint32_t qtd_chaves_atual;
bool is_folha;
}node;


typedef struct {
    key chave_subida;
    node* direita;
} ResultadoSplit;


typedef struct {
    key chave_promovida;
    node* novo_no_direita;
    bool houve_split;
} RetornoInsercao;


//estrutura da arvore
typedef struct BTree {
    node *root;
    int ordem;
} BTree;




//formata linha do formato [Nome1] [Tamanho1] [Hash1]
void formataLinhaIN(char* nome, uint64_t* tamanhoArquivo, char* hash, char* linha){
char* parada;
char tamanhoArq[150];
parada = strtok(linha," ");
if(parada !=NULL){
strcpy(nome, parada);
}

parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(tamanhoArq, parada);
*tamanhoArquivo = strtoull(parada, NULL, 10);
}

parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(hash, parada);
}
}



void formataLinhaOUT(char* comando,char* nome, uint64_t* tamanhoArquivo, char* hash, char* linha){
char* parada;
char tamanhoArq[150];


parada = strtok(linha," ");
if(parada !=NULL){
strcpy(comando, parada);
}

if(strcmp("INSERT", comando) == 0){
parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(nome, parada);
}

parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(tamanhoArq, parada);
*tamanhoArquivo = strtoull(parada, NULL, 10);
}

parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(hash, parada);
}
}else{

parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(hash, parada);
}
}
}





//inicia arvore
BTree* inicia_arvore(int ordem){
    BTree *arvore = malloc(sizeof(BTree));
    arvore->root = malloc(sizeof(node));
    arvore->root->chaves = malloc(sizeof(key) * (ordem - 1));
    arvore->root->ArrayPonteiros = malloc(sizeof(node*) * (ordem));
    arvore->root->qtd_chaves_atual = 0;
    arvore->root->is_folha = true;

    for (int i = 0; i < ordem; i++) {
        arvore->root->ArrayPonteiros[i] = NULL;
    }

    return arvore;    
}


node* cria_no(int ordem, bool is_folha) {
    node* novo = malloc(sizeof(node));
    novo->chaves = malloc(sizeof(key) * (ordem - 1));
    novo->ArrayPonteiros = malloc(sizeof(node*) * ordem);
    novo->qtd_chaves_atual = 0;
    novo->is_folha = is_folha;

    // Inicializa os ponteiros com NULL
    for (int i = 0; i < ordem; i++) {
        novo->ArrayPonteiros[i] = NULL;
    }

    return novo;
}



//Função de busca
node* buscaNaArvore(node* current_node, char chave[33]) {
    if (current_node == NULL){
    return NULL;
    } 
    int acc = 0;
    while (acc < current_node->qtd_chaves_atual && strcmp(chave, current_node->chaves[acc].hash) > 0) {
        acc++;
    }

    if (acc < current_node->qtd_chaves_atual && strcmp(chave, current_node->chaves[acc].hash) == 0) {
        return current_node; 
    } else if (current_node->is_folha) {
        return NULL; 
    } else {
        return buscaNaArvore(current_node->ArrayPonteiros[acc], chave); 
    }
}



//Função que mantem a ordem no vetor de chaves
void manter_ordem(key *chaves, int num_chaves) {
    for (int i = 0; i < num_chaves - 1; i++) {
        for (int j = 0; j < num_chaves - i - 1; j++) {
            if (strcmp(chaves[j].hash, chaves[j + 1].hash) > 0) {
                key temp = chaves[j];
                chaves[j] = chaves[j + 1];
                chaves[j + 1] = temp;
            }
        }
    }
}


/**
 * @brief Insere a chave em um nó que garantidamente não está cheio.
 * @param no O nó para inserir a chave.
 * @param chave A chave a ser inserida.
 * @param ponteiro_direita O ponteiro para o filho à direita da nova chave (se houver).
 */
void insere_chave_em_no_nao_cheio(node* no, key chave, node* ponteiro_direita) {
    int i = no->qtd_chaves_atual - 1;
    // Desloca as chaves e ponteiros maiores para a direita
    while (i >= 0 && strcmp(chave.hash, no->chaves[i].hash) < 0) {
        no->chaves[i + 1] = no->chaves[i];
        if (!no->is_folha) {
            no->ArrayPonteiros[i + 2] = no->ArrayPonteiros[i + 1];
        }
        i--;
    }
    // Insere a nova chave e o ponteiro da direita na posição correta
    no->chaves[i + 1] = chave;
    if (!no->is_folha) {
        no->ArrayPonteiros[i + 2] = ponteiro_direita;
    }
    no->qtd_chaves_atual++;
}

/**
 * @brief Divide um nó cheio em dois.
 * @param ordem A ordem da árvore.
 * @param no_a_dividir O nó que está cheio e será dividido.
 * @return Um struct ResultadoSplit com a chave promovida e o novo nó da direita.
 */
ResultadoSplit split(int ordem, node* no_a_dividir) {
    ResultadoSplit resultado;
    int mediana_idx = (ordem - 1) / 2;

    resultado.chave_subida = no_a_dividir->chaves[mediana_idx];

    node* direita = cria_no(ordem, no_a_dividir->is_folha);
    
    // Copia a segunda metade das chaves para o novo nó (direita)
    int chaves_na_direita = 0;
    for (int i = mediana_idx + 1; i < ordem - 1; i++) {
        direita->chaves[chaves_na_direita++] = no_a_dividir->chaves[i];
    }
    direita->qtd_chaves_atual = chaves_na_direita;
    
    // Se não for folha, copia os ponteiros correspondentes
    if (!no_a_dividir->is_folha) {
        int ponteiros_na_direita = 0;
        for (int i = mediana_idx + 1; i <= ordem - 1; i++) {
            direita->ArrayPonteiros[ponteiros_na_direita++] = no_a_dividir->ArrayPonteiros[i];
        }
    }
    
    // Atualiza o número de chaves no nó original (esquerda)
    no_a_dividir->qtd_chaves_atual = mediana_idx;
    resultado.direita = direita;
    return resultado;
}



/**
 * @brief Função recursiva para inserir uma chave na árvore.
 * @param current_node O nó atual.
 * @param nova_chave A chave a ser inserida.
 * @param ordem A ordem da árvore.
 * @return Um struct RetornoInsercao indicando se ocorreu split.
 */
RetornoInsercao insere_chaves_recursivo(node* current_node, key nova_chave, int ordem) {
    RetornoInsercao retorno;
    retorno.houve_split = false;

    // Caso 1: O nó é uma folha.
    if (current_node->is_folha) {
        insere_chave_em_no_nao_cheio(current_node, nova_chave, NULL);
        if (current_node->qtd_chaves_atual == ordem - 1) { // Estourou o limite de chaves
            ResultadoSplit s = split(ordem, current_node);
            retorno.chave_promovida = s.chave_subida;
            retorno.novo_no_direita = s.direita;
            retorno.houve_split = true;
        }
        return retorno;
    }

    // Caso 2: O nó é interno. Encontra o filho para descer.
    int i = 0;
    while (i < current_node->qtd_chaves_atual && strcmp(nova_chave.hash, current_node->chaves[i].hash) > 0) {
        i++;
    }

    // Se o filho para onde vamos descer está cheio, faz o split antes.
    if (current_node->ArrayPonteiros[i]->qtd_chaves_atual == ordem - 1) {
        ResultadoSplit s = split(ordem, current_node->ArrayPonteiros[i]);
        insere_chave_em_no_nao_cheio(current_node, s.chave_subida, s.direita);
        // Após o split, decide para qual dos novos filhos descer
        if (strcmp(nova_chave.hash, current_node->chaves[i].hash) > 0) {
            i++;
        }
    }
    
    // Chama a inserção recursivamente para o filho apropriado.
    RetornoInsercao filho_retorno = insere_chaves_recursivo(current_node->ArrayPonteiros[i], nova_chave, ordem);

    return retorno; // Em uma inserção top-down com split preventivo, o retorno não é usado.
}

void inserir_na_arvore(BTree* arvore, key nova_chave) {
    // Se a raiz está cheia, ela precisa ser dividida antes da inserção
    if (arvore->root->qtd_chaves_atual == arvore->ordem - 1) {
        node* antiga_raiz = arvore->root;
        node* nova_raiz = cria_no(arvore->ordem, false);
        arvore->root = nova_raiz;
        nova_raiz->ArrayPonteiros[0] = antiga_raiz;
        
        ResultadoSplit s = split(arvore->ordem, antiga_raiz);
        insere_chave_em_no_nao_cheio(nova_raiz, s.chave_subida, s.direita);
        
        RetornoInsercao r = insere_chaves_recursivo(nova_raiz, nova_chave, arvore->ordem);
    } else {
        RetornoInsercao r = insere_chaves_recursivo(arvore->root, nova_chave, arvore->ordem);
    }
}


void processamento(FILE* input, FILE* output){
char linha[500];
int64_t ordemArvore;
int64_t qtdArq;
int64_t qtdOP;
//Variaveis temps:
key chaveTemp;
char nomeTemp[100];
uint32_t tamanhoArqTemp;
char hashTemp[33];
char  comandoTemp[100];

//inserir na arvore: ------------------------------------------------------------------------

if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
if(sscanf(linha, "%"SCNu64, &ordemArvore) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
}

BTree* arvore = inicia_arvore(ordemArvore);

if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
if(sscanf(linha, "%"SCNu64, &qtdArq) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
}

for(int i = 0; i < qtdArq; i++){
if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
formataLinhaIN(chaveTemp.nome,&chaveTemp.tamanhoArq,chaveTemp.hash,linha);
inserir_na_arvore(arvore, chaveTemp);
}

//Operações: ------------------------------------------------------------------------

if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
if(sscanf(linha, "%"SCNu64, &qtdOP) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
}


for(int i = 0; i < qtdOP; i++){
if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
formataLinhaOUT(comandoTemp,nomeTemp,tamanhoArqTemp,hashTemp,linha);


if(strcmp("INSERT", comandoTemp) == 0){
strcpy(chaveTemp.nome, nomeTemp);
strcpy(chaveTemp.hash, hashTemp);
chaveTemp.tamanhoArq = tamanhoArqTemp;
inserir_na_arvore(arvore, chaveTemp); 

}else if(strcmp("SELECT", comandoTemp) == 0){
fprintf(output,"[%s]\n", hashTemp);
node* resultado = buscaNaArvore(arvore->root, hashTemp);
if(resultado != NULL){
    // se achar
for(int i = 0; i < resultado->qtd_chaves_atual; i++){
      fprintf(output, "%s:size=%llu,hash=%s\n", resultado->chaves[i].nome, (unsigned long long)resultado->chaves[i].tamanhoArq,resultado->chaves[i].hash);
}
}else{
    //se não achar
      fprintf(output, "-\n");
}
}
}
}


int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");

processamento(input,output);

fclose(input);
fclose(output);
return 0;
}


//ajeitar parsing

// O código que você forneceu chama várias funções (cria_no, split, insere_chave_em_no_nao_cheio, insere_chaves_recursivo), mas a definição (o corpo) dessas funções não está no arquivo.
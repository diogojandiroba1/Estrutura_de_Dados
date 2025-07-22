#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

//Tipo de dado que representa a cada arquivo/elemento
typedef struct arquivo{
char nome[50];
char tipoLeitura[3];
uint64_t tamanhoArquivo;
uint16_t id;
struct arquivo* esquerda;
struct arquivo* direita;
} arquivo;


//Tipo de dado que representa a arvore
typedef struct arvore{
arquivo* raiz;
}arvore;


//Função que formata entrada
void formataLinha(char* nome, char* tipoLeitura, uint64_t* tamanhoArquivo, char* linha){
char* parada;
char tamanhoArq[150];
parada = strtok(linha," ");
if(parada !=NULL){
strcpy(nome, parada);
}
parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(tipoLeitura, parada);
}
parada = strtok(NULL," ");
if(parada !=NULL){
strcpy(tamanhoArq, parada);
*tamanhoArquivo = strtoull(parada, NULL, 10);
}
}


//Função que inicia arvore deixando a raiz nula;
void iniciaArvore(arvore* arvoreBusca){
arvoreBusca->raiz = NULL; 
}

//Função que add elementos a arvore
void addArquivoArvore(char* nome, char* tipoLeitura, uint64_t tamanhoArquivo, arvore* arvoreBusca, uint16_t* idElemento){
    if(arvoreBusca->raiz == NULL){
        arquivo* novoArquivo = malloc(sizeof(arquivo));
        strcpy(novoArquivo->nome, nome);
        strcpy(novoArquivo->tipoLeitura, tipoLeitura);
        novoArquivo->tamanhoArquivo = tamanhoArquivo;
        novoArquivo->id = *idElemento;
        novoArquivo->esquerda = novoArquivo->direita = NULL;
        arvoreBusca->raiz = novoArquivo;
        (*idElemento)++;  // <-- aqui
        return;
    }

    arquivo* atual = arvoreBusca->raiz;
    arquivo* pai = NULL;

    while(atual != NULL){
        pai = atual;
        if(strcmp(atual->nome, nome) == 0){
            if(strcmp(atual->tipoLeitura, "ro") == 0) return;
            else{
                strcpy(atual->tipoLeitura, tipoLeitura);
                atual->tamanhoArquivo = tamanhoArquivo;
                // id não muda!
                return;
            }
        }

        if(strcmp(nome, atual->nome) > 0){
            atual = atual->direita;
        }else{
            atual = atual->esquerda;
        }
    }

    arquivo* novoArquivo = malloc(sizeof(arquivo));
    strcpy(novoArquivo->nome, nome);
    strcpy(novoArquivo->tipoLeitura, tipoLeitura);
    novoArquivo->tamanhoArquivo = tamanhoArquivo;
    novoArquivo->id = *idElemento;
    novoArquivo->esquerda = novoArquivo->direita = NULL;

    if(strcmp(nome, pai->nome) > 0){
        pai->direita = novoArquivo;
    }else{
        pai->esquerda = novoArquivo;
    }

    (*idElemento)++; // <-- aqui
}

//Função recursiva para em Ordem
void BuscaEmOrdemArvore(arquivo* raizBusca, FILE* output) {
    if (raizBusca == NULL) return;

    BuscaEmOrdemArvore(raizBusca->esquerda, output);

    if(raizBusca->tamanhoArquivo == 1){
    fprintf(output, "%d:%s|%s|%" PRIu64 "_byte\n", raizBusca->id, raizBusca->nome, raizBusca->tipoLeitura, (uint64_t)raizBusca->tamanhoArquivo);
    }
    else{
    fprintf(output, "%d:%s|%s|%" PRIu64 "_bytes\n", raizBusca->id, raizBusca->nome, raizBusca->tipoLeitura, (uint64_t)raizBusca->tamanhoArquivo); 
    }

    BuscaEmOrdemArvore(raizBusca->direita, output);
}

//Função recursiva para pre ordem (feita)
void BuscaPreOrdemArvore(arquivo* raizBusca, FILE* output) {
    if (raizBusca == NULL) return;

    if(raizBusca->tamanhoArquivo == 1){
    fprintf(output, "%d:%s|%s|%" PRIu64 "_byte\n", raizBusca->id, raizBusca->nome, raizBusca->tipoLeitura, (uint64_t)raizBusca->tamanhoArquivo);
    }
    else{
    fprintf(output, "%d:%s|%s|%" PRIu64 "_bytes\n", raizBusca->id, raizBusca->nome, raizBusca->tipoLeitura, (uint64_t)raizBusca->tamanhoArquivo); 
    }

    BuscaPreOrdemArvore(raizBusca->esquerda, output);
    BuscaPreOrdemArvore(raizBusca->direita, output);
}

//Função recursiva para pos ordem
void BuscaPosOrdemArvore(arquivo* raizBusca, FILE* output) {
    if (raizBusca == NULL) return;

    BuscaPosOrdemArvore(raizBusca->esquerda, output);
    BuscaPosOrdemArvore(raizBusca->direita, output);

    if(raizBusca->tamanhoArquivo == 1){
    fprintf(output, "%d:%s|%s|%" PRIu64 "_byte\n", raizBusca->id, raizBusca->nome, raizBusca->tipoLeitura, (uint64_t)raizBusca->tamanhoArquivo);
    }
    else{
    fprintf(output, "%d:%s|%s|%" PRIu64 "_bytes\n", raizBusca->id, raizBusca->nome, raizBusca->tipoLeitura, (uint64_t)raizBusca->tamanhoArquivo); 
    }

}


void processamento(FILE* input, FILE* output){

//Inicia arvore e variaveis temporarias    
arvore* arvoreBinaria = malloc(sizeof(arvore));
iniciaArvore(arvoreBinaria);
char linha[150];
char nomeTemp[50];
char formatoTemp[3];
uint64_t tamanhoTemp = 0;
uint64_t qtdArquivos;
uint16_t idElemento = 0;

//Pega a qtd de arquivos para add
if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
if(sscanf(linha, "%"SCNu64, &qtdArquivos) != 1){
    printf("Erro ao ler qtd arquivos\n");
    return;
}

//Pega linha por linha, processa a linha, e add na arvore
for(unsigned long long i = 0; i < qtdArquivos; i++){

if (fgets(linha, sizeof(linha), input) == NULL) {
     printf("Erro na leitura da linha\n");
    return;
}
formataLinha(nomeTemp, formatoTemp, &tamanhoTemp, linha);
addArquivoArvore(nomeTemp, formatoTemp, tamanhoTemp, arvoreBinaria, &idElemento);
}

arquivo* atual = arvoreBinaria->raiz;

//Processamento EPD
fprintf(output,"[EPD]\n");
BuscaEmOrdemArvore(atual,output);

//Processamento PED
fprintf(output,"[PED]\n");
BuscaPreOrdemArvore(atual,output);

//Processamento EDP
fprintf(output,"[EDP]\n");
BuscaPosOrdemArvore(atual,output);

}


int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");

processamento(input,output);

fclose(input);
fclose(output);
return 0;
}
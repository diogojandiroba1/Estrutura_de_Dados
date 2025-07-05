#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct impressoras{
    char* nome;
    uint32_t pagPimp;
} impressoras;

typedef struct doc{
    char* nome;
    int pag;
} doc;

typedef struct filaDoc{
    size_t capacidade;
    size_t inicio;
    size_t fim;
    size_t tamanho;
    doc* vetor;
} filaDoc;

void iniciarFilaDoc(filaDoc* fila, int qtdDoc){
    if(fila == NULL){
        printf("Falha1");
        return;
    }

    fila->capacidade = qtdDoc;
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;

    fila->vetor = (doc*)malloc(sizeof(doc) * qtdDoc);
    if(fila->vetor == NULL){
        printf("Falha na alocação!2");
        return;
    }
}

void iniciarImpressoras(impressoras** listaIMP, uint32_t qtdIMP){
    *listaIMP = (impressoras*)malloc(sizeof(impressoras) * qtdIMP);
    if(*listaIMP == NULL){
        printf("Falha na alocação!3");
        return;
    }
}

void adicionarDoc(filaDoc* fila, char* nomeDoc, uint32_t pagDoc){
    if(fila == NULL || fila->vetor == NULL){
        printf("Falha na fila\n");
        return;
    }

    if (fila->tamanho == fila->capacidade) { 
        printf("Fila cheia\n"); 
        return;
    }

    fila->vetor[fila->fim].pag = pagDoc;
    fila->vetor[fila->fim].nome = strdup(nomeDoc);
    if (fila->vetor[fila->fim].nome == NULL) {
        printf("Falha ao duplicar nome\n");  
        return;
    }

    fila->fim = (fila->fim + 1) % fila->capacidade; 
    fila->tamanho++; 
}

doc desenfileirar(filaDoc* fila){
    doc vazio = {NULL, 0};

    if(fila == NULL || fila->vetor == NULL){
        printf("Falha fila\n");
        return vazio;
    }

    if (fila->tamanho == 0) {
        printf("Fila vazia\n");
        return vazio;
    }

    doc documentoRemovido = fila->vetor[fila->inicio];

    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;

    return documentoRemovido;   
}

int menorImp(impressoras* vetorIMP, uint32_t qtdIMP){
    int menor = 0;
    for (uint32_t j = 1; j < qtdIMP; j++) {
        if (vetorIMP[j].pagPimp < vetorIMP[menor].pagPimp)
            menor = j;
    }
    return menor;
}

void processamento(FILE* input, impressoras** listaImpressoras, uint32_t* pQtdImp, filaDoc* filaDeDocumentos, uint32_t* pQtdDoc){
    char linha[100];
    char bufferNome[50];
    uint32_t pagDoc;

    if (fgets(linha, sizeof(linha), input) == NULL) {
        printf("Erro na leitura qtd impressoras\n");
        return;
    }
    if(sscanf(linha, "%u", pQtdImp) != 1){
        printf("Erro ao ler qtd impressoras\n");
        return;
    }

    iniciarImpressoras(listaImpressoras, *pQtdImp);
    if (*listaImpressoras == NULL) {
        printf("Falha ao alocar impressoras\n");
        return;
    }

    for (uint32_t i = 0; i < *pQtdImp; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            printf("Erro na leitura nome impressora\n");
            return;
        }
        linha[strcspn(linha, "\n")] = 0; // Remove '\n'
        (*listaImpressoras)[i].nome = strdup(linha);
        (*listaImpressoras)[i].pagPimp = 0; // inicializa páginas impressas
    }

    if (fgets(linha, sizeof(linha), input) == NULL || sscanf(linha, "%u", pQtdDoc) != 1) {
        printf("Erro na leitura qtd documentos\n");
        return;
    }

    iniciarFilaDoc(filaDeDocumentos, *pQtdDoc);
   
    for (uint32_t i = 0; i < *pQtdDoc; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            printf("Erro na leitura documento\n");
            return;
        }

        if (sscanf(linha, "%s %u", bufferNome, &pagDoc) != 2) {
            printf("Erro no formato do documento\n");
            return;
        }

        adicionarDoc(filaDeDocumentos, bufferNome, pagDoc);
    }
}

void processamentoSaida(FILE* output, impressoras* vetorIMP, uint32_t qtdIMP, filaDoc* filaDeDoc) {
    doc* historicoGeral = malloc(sizeof(doc) * filaDeDoc->capacidade);
    int tamHistGeral = 0;

    doc** historicoPorIMP = malloc(sizeof(doc*) * qtdIMP);
    int* tamHistorico = calloc(qtdIMP, sizeof(int));

    for (uint32_t i = 0; i < qtdIMP; i++) {
        historicoPorIMP[i] = malloc(sizeof(doc) * filaDeDoc->capacidade);
    }

    while (filaDeDoc->tamanho > 0) {
        doc d = desenfileirar(filaDeDoc);
        if (d.nome == NULL) {
            break; // fila vazia ou erro
        }

        historicoGeral[tamHistGeral++] = d;

        int idxMenor = menorImp(vetorIMP, qtdIMP);

        vetorIMP[idxMenor].pagPimp += d.pag;
        historicoPorIMP[idxMenor][tamHistorico[idxMenor]++] = d;

        fprintf(output, "%s:", vetorIMP[idxMenor].nome);

        for (int k = tamHistorico[idxMenor] - 1; k >= 0; k--) {
            fprintf(output, "%s-%dp", historicoPorIMP[idxMenor][k].nome, historicoPorIMP[idxMenor][k].pag);
            if (k > 0) fprintf(output, ",");
        }
        fprintf(output, "\n");
    }

    uint32_t totalPaginas = 0;
    for (int i = 0; i < tamHistGeral; i++) {
        totalPaginas += historicoGeral[i].pag;
    }
    fprintf(output, "%dp\n", totalPaginas);

    for (int i = tamHistGeral - 1; i >= 0; i--) {
        fprintf(output, "%s-%dp\n", historicoGeral[i].nome, historicoGeral[i].pag);
    }

    for (uint32_t i = 0; i < qtdIMP; i++) {
        free(historicoPorIMP[i]);
    }
    free(historicoPorIMP);
    free(tamHistorico);
    free(historicoGeral);
}

int main(int argc, char* argv[]){
    if (argc < 3) {
        printf("Use: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1],"r");
    FILE* output = fopen(argv[2],"w");

    if (input == NULL) {
        printf("Erro ao abrir arquivo de entrada\n");
        return 1;
    }
    if (output == NULL) {
        printf("Erro ao abrir arquivo de saída\n");
        fclose(input);
        return 1;
    }

    uint32_t qtdImp = 0;
    uint32_t qtdDoc = 0;
    impressoras* listaImpressoras = NULL;
    filaDoc filaDeDocumentos;

    processamento(input, &listaImpressoras, &qtdImp, &filaDeDocumentos, &qtdDoc);
    processamentoSaida(output, listaImpressoras, qtdImp, &filaDeDocumentos);

    // Libera memória alocada para nomes
    for (uint32_t i = 0; i < qtdImp; i++) {
        free(listaImpressoras[i].nome);
    }
    free(listaImpressoras);

    for (size_t i = 0; i < filaDeDocumentos.capacidade; i++) {
        free(filaDeDocumentos.vetor[i].nome);
    }
    free(filaDeDocumentos.vetor);

    fclose(input);
    fclose(output);
    return 0;
}

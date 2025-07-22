#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>

// Estrutura do elemento (nó) da árvore, agora chamada de "ElementoDoDicionario"
typedef struct ElementoDoDicionario {
    int8_t altitude; // 'height' virou 'altitude'
    int8_t contagemDeSinonimos; // 'qtd_synonyms' virou 'contagemDeSinonimos'
    char verbetePrincipal[31]; // 'key_word' virou 'verbetePrincipal'
    char palavrasSemelhantes[10][31]; // 'synonyms' virou 'palavrasSemelhantes'
    struct ElementoDoDicionario* ramoDireito; // 'right' virou 'ramoDireito'
    struct ElementoDoDicionario* ramoEsquerdo; // 'left' virou 'ramoEsquerdo'
} ElementoDoDicionario;

// Estrutura da Árvore AVL, agora chamada de "IndiceRemissivoAVL"
typedef struct IndiceRemissivoAVL {
    ElementoDoDicionario* raizDaArvore; // 'root' virou 'raizDaArvore'
} IndiceRemissivoAVL;

// Função para obter a altitude (altura) de uma sub-árvore
int8_t obterAltitude(ElementoDoDicionario* elementoAtual) {
    if (elementoAtual == NULL) return -1;
    return elementoAtual->altitude;
}

// Função para obter o fator de balanceamento
int32_t calcularFatorBalanceamento(ElementoDoDicionario* elementoAtual) {
    if (elementoAtual == NULL) return 0;
    return obterAltitude(elementoAtual->ramoEsquerdo) - obterAltitude(elementoAtual->ramoDireito);
}

// Função para decodificar (formatar) uma linha do arquivo de entrada
void decodificarLinha(char* verbeteDestino, char arrSinonimosDestino[10][31], char* linhaOriginal, int64_t* contadorDeSinonimos) {
    char* token;
    char bufferQtd[150];

    token = strtok(linhaOriginal, " ");
    if (token != NULL) {
        strcpy(verbeteDestino, token);
    }

    token = strtok(NULL, " ");
    if (token != NULL) {
        strcpy(bufferQtd, token);
        *contadorDeSinonimos = strtoull(token, NULL, 10);
    }

    for (int i = 0; i < *contadorDeSinonimos; i++) {
        token = strtok(NULL, " ");
        if (token != NULL) {
            strcpy(arrSinonimosDestino[i], token);
        } else {
            break;
        }
    }
}

// Função para iniciar a estrutura da árvore
void iniciarIndiceRemissivo(IndiceRemissivoAVL* indiceParaIniciar) {
    indiceParaIniciar->raizDaArvore = NULL;
}

// Lógica de rotações da árvore
// Função para rotacionar a sub-árvore para a esquerda
ElementoDoDicionario* rotacaoSimplesEsquerda(ElementoDoDicionario* pivo) {
    ElementoDoDicionario* novaRaiz = pivo->ramoDireito;
    pivo->ramoDireito = novaRaiz->ramoEsquerdo;
    novaRaiz->ramoEsquerdo = pivo;

    pivo->altitude = fmax(obterAltitude(pivo->ramoEsquerdo), obterAltitude(pivo->ramoDireito)) + 1;
    novaRaiz->altitude = fmax(obterAltitude(novaRaiz->ramoEsquerdo), obterAltitude(novaRaiz->ramoDireito)) + 1;

    return novaRaiz;
}

// Função para rotacionar a sub-árvore para a direita
ElementoDoDicionario* rotacaoSimplesDireita(ElementoDoDicionario* pivo) {
    ElementoDoDicionario* novaRaiz = pivo->ramoEsquerdo;
    pivo->ramoEsquerdo = novaRaiz->ramoDireito;
    novaRaiz->ramoDireito = pivo;

    pivo->altitude = fmax(obterAltitude(pivo->ramoEsquerdo), obterAltitude(pivo->ramoDireito)) + 1;
    novaRaiz->altitude = fmax(obterAltitude(novaRaiz->ramoEsquerdo), obterAltitude(novaRaiz->ramoDireito)) + 1;

    return novaRaiz;
}

// Função para alocar um novo elemento (nó)
ElementoDoDicionario* alocarNovoElemento(char* verbete, char arrSinonimos[10][31], int totalSinonimos) {
    ElementoDoDicionario* novoElemento = (ElementoDoDicionario*)malloc(sizeof(ElementoDoDicionario));

    strcpy(novoElemento->verbetePrincipal, verbete);

    for (int i = 0; i < 10; i++) {
        novoElemento->palavrasSemelhantes[i][0] = '\0';
    }

    for (int i = 0; i < totalSinonimos && i < 10; i++) {
        strcpy(novoElemento->palavrasSemelhantes[i], arrSinonimos[i]);
    }

    novoElemento->altitude = 0;
    novoElemento->contagemDeSinonimos = totalSinonimos;
    novoElemento->ramoEsquerdo = NULL;
    novoElemento->ramoDireito = NULL;
    return novoElemento;
}

// Função para inserir um verbete (nó) na árvore
ElementoDoDicionario* inserirVerbete(ElementoDoDicionario* elementoAtual, char* verbeteParaInserir, char arrSinonimos[10][31], int totalSinonimos) {
    // Caso base: Folha da árvore
    if (elementoAtual == NULL) {
        return alocarNovoElemento(verbeteParaInserir, arrSinonimos, totalSinonimos);
    }
    
    int resultadoComparacao = strcmp(verbeteParaInserir, elementoAtual->verbetePrincipal);

    if (resultadoComparacao < 0) {
        elementoAtual->ramoEsquerdo = inserirVerbete(elementoAtual->ramoEsquerdo, verbeteParaInserir, arrSinonimos, totalSinonimos);
    } else if (resultadoComparacao > 0) {
        elementoAtual->ramoDireito = inserirVerbete(elementoAtual->ramoDireito, verbeteParaInserir, arrSinonimos, totalSinonimos);
    } else {
        // A chave já existe. Neste código, a lógica original inseria à esquerda, o que não é o padrão para BSTs/AVLs
        // Mantendo a lógica original, mas idealmente deveria haver um tratamento para chaves duplicadas (ignorar ou atualizar).
        elementoAtual->ramoEsquerdo = inserirVerbete(elementoAtual->ramoEsquerdo, verbeteParaInserir, arrSinonimos, totalSinonimos);
    }

    elementoAtual->altitude = 1 + fmax(obterAltitude(elementoAtual->ramoEsquerdo), obterAltitude(elementoAtual->ramoDireito));

    int fatorDeEquilibrio = calcularFatorBalanceamento(elementoAtual);

    // Caso Esquerda-Esquerda
    if (fatorDeEquilibrio > 1 && strcmp(verbeteParaInserir, elementoAtual->ramoEsquerdo->verbetePrincipal) < 0) {
        return rotacaoSimplesDireita(elementoAtual);
    }
    // Caso Direita-Direita
    if (fatorDeEquilibrio < -1 && strcmp(verbeteParaInserir, elementoAtual->ramoDireito->verbetePrincipal) > 0) {
        return rotacaoSimplesEsquerda(elementoAtual);
    }

    // Caso Esquerda-Direita
    if (fatorDeEquilibrio > 1 && strcmp(verbeteParaInserir, elementoAtual->ramoEsquerdo->verbetePrincipal) > 0) {
        elementoAtual->ramoEsquerdo = rotacaoSimplesEsquerda(elementoAtual->ramoEsquerdo);
        return rotacaoSimplesDireita(elementoAtual);
    }
    // Caso Direita-Esquerda
    if (fatorDeEquilibrio < -1 && strcmp(verbeteParaInserir, elementoAtual->ramoDireito->verbetePrincipal) < 0) {
        elementoAtual->ramoDireito = rotacaoSimplesDireita(elementoAtual->ramoDireito);
        return rotacaoSimplesEsquerda(elementoAtual);
    }

    return elementoAtual;
}

// Função para localizar um verbete na árvore
ElementoDoDicionario* localizarVerbete(ElementoDoDicionario* elementoAtual, char* verbeteAlvo, FILE* arquivoDeSaida) {
    if (elementoAtual == NULL) {
        fprintf(arquivoDeSaida, "?");
        return NULL;
    }
    fprintf(arquivoDeSaida, "%s", elementoAtual->verbetePrincipal);
    int resultadoComparacao = strcmp(verbeteAlvo, elementoAtual->verbetePrincipal);

    if (resultadoComparacao == 0) {
        return elementoAtual;
    } else if (resultadoComparacao < 0) {
        fprintf(arquivoDeSaida, "->");
        return localizarVerbete(elementoAtual->ramoEsquerdo, verbeteAlvo, arquivoDeSaida);
    } else {
        fprintf(arquivoDeSaida, "->");
        return localizarVerbete(elementoAtual->ramoDireito, verbeteAlvo, arquivoDeSaida);
    }
}

// Função que libera a memória alocada para a árvore
void liberarIndiceCompleto(ElementoDoDicionario* elementoAtual) {
    if (elementoAtual != NULL) {
        liberarIndiceCompleto(elementoAtual->ramoEsquerdo);
        liberarIndiceCompleto(elementoAtual->ramoDireito);
        free(elementoAtual);
    }
}

// Função que gerencia o processamento principal do código
void gerenciarOperacoes(FILE* arquivoDeEntrada, FILE* arquivoDeSaida) {
    // Inicia a árvore
    IndiceRemissivoAVL* meuDicionario = (IndiceRemissivoAVL*)malloc(sizeof(IndiceRemissivoAVL));
    iniciarIndiceRemissivo(meuDicionario);

    ElementoDoDicionario* elementoEncontrado;

    char bufferDeLinha[400];
    char verbeteTemporario[31];
    char verbeteDeBusca[31];
    char sinonimosTemporarios[10][31];
    int64_t totalDeVerbetes;
    int64_t totalDeBuscas;
    int64_t numeroDeSinonimos;

    // Obtém o número de palavras-chave e as processa
    if (fgets(bufferDeLinha, sizeof(bufferDeLinha), arquivoDeEntrada) == NULL) {
        printf("Erro na leitura da linha de contagem de palavras\n");
        return;
    }
    if (sscanf(bufferDeLinha, "%" SCNu64, &totalDeVerbetes) != 1) {
        printf("Erro ao ler a quantidade de palavras-chave\n");
        return;
    }

    // Pega linha por linha, processa e adiciona à árvore
    for (int i = 0; i < totalDeVerbetes; i++) {
        if (fgets(bufferDeLinha, sizeof(bufferDeLinha), arquivoDeEntrada) == NULL) {
            printf("Erro na leitura da linha do verbete\n");
            return;
        }
        decodificarLinha(verbeteTemporario, sinonimosTemporarios, bufferDeLinha, &numeroDeSinonimos);
        meuDicionario->raizDaArvore = inserirVerbete(meuDicionario->raizDaArvore, verbeteTemporario, sinonimosTemporarios, numeroDeSinonimos);
    }

    // Pega as entradas para busca
    if (fgets(bufferDeLinha, sizeof(bufferDeLinha), arquivoDeEntrada) == NULL) {
        printf("Erro na leitura da linha de contagem de buscas\n");
        return;
    }
    if (sscanf(bufferDeLinha, "%" SCNu64, &totalDeBuscas) != 1) {
        printf("Erro ao ler a quantidade de palavras de busca\n");
        return;
    }

    // BUSCA PELO VERBETE
    for (int i = 0; i < totalDeBuscas; i++) {
        if (fgets(bufferDeLinha, sizeof(bufferDeLinha), arquivoDeEntrada) == NULL) {
            printf("Erro na leitura da linha de busca\n");
            return;
        }
        if (sscanf(bufferDeLinha, "%s", verbeteDeBusca) != 1) {
            printf("Erro ao ler a palavra de busca\n");
            return;
        }

        fprintf(arquivoDeSaida, "[");
        elementoEncontrado = localizarVerbete(meuDicionario->raizDaArvore, verbeteDeBusca, arquivoDeSaida);
        fprintf(arquivoDeSaida, "]\n");

        if (elementoEncontrado == NULL) {
            fprintf(arquivoDeSaida, "-\n");
        } else {
            for (int j = 0; j < elementoEncontrado->contagemDeSinonimos; j++) {
                fprintf(arquivoDeSaida, "%s", elementoEncontrado->palavrasSemelhantes[j]);
                if (j < elementoEncontrado->contagemDeSinonimos - 1) {
                    fprintf(arquivoDeSaida, ",");
                }
            }
             fprintf(arquivoDeSaida, "\n");
        }
    }

    liberarIndiceCompleto(meuDicionario->raizDaArvore);
    free(meuDicionario);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* arquivoDeEntrada = fopen(argv[1], "r");
    FILE* arquivoDeSaida = fopen(argv[2], "w");

    if (arquivoDeEntrada == NULL || arquivoDeSaida == NULL) {
        perror("Erro ao abrir os arquivos");
        return 1;
    }

    gerenciarOperacoes(arquivoDeEntrada, arquivoDeSaida);

    fclose(arquivoDeEntrada);
    fclose(arquivoDeSaida);
    return 0;
}
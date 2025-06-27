#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct livros
{
    char autor[50];
    char titulo[100];
    uint64_t isbn;
} livros;

void ordenacao(livros *biblioteca, uint32_t qtdLivros)
{
    for (uint32_t i = 1; i < qtdLivros; i++)
    {
        livros chave = biblioteca[i];
        int j = i - 1;
        while (j >= 0 && biblioteca[j].isbn > chave.isbn)
        {
            biblioteca[j + 1] = biblioteca[j];
            j--;
        }
        biblioteca[j + 1] = chave;
    }
}

void processamentoEntrada(FILE *input, livros **biblioteca, uint32_t *qtdLivros, int64_t **vetorBuscado, uint32_t *qtdBuscas)
{
    char linha[256];

    if (fgets(linha, sizeof(linha), input) == NULL || sscanf(linha, "%u", qtdLivros) != 1)
    {
        printf("Erro ao ler qtd de livros\n");
        return;
    }

    *biblioteca = malloc(sizeof(livros) * (*qtdLivros));
    if (*biblioteca == NULL)
    {
        printf("Erro de alocacao biblioteca\n");
        return;
    }

    for (uint32_t i = 0; i < *qtdLivros; i++)
    {
        if (fgets(linha, sizeof(linha), input) == NULL)
        {
            printf("Erro ao ler livro\n");
            return;
        }
        linha[strcspn(linha, "\n")] = '\0';

        char *isbn = strtok(linha, " ");
        char *autor = strtok(NULL, "&");
        char *titulo = strtok(NULL, "");

        (*biblioteca)[i].isbn = strtoull(isbn, NULL, 10);
        strcpy((*biblioteca)[i].autor, autor);
        strcpy((*biblioteca)[i].titulo, titulo);
    }

    if (fgets(linha, sizeof(linha), input) == NULL || sscanf(linha, "%u", qtdBuscas) != 1)
    {
        printf("Erro ao ler qtd de buscas\n");
        return;
    }

    *vetorBuscado = malloc(sizeof(int64_t) * (*qtdBuscas));
    if (*vetorBuscado == NULL)
    {
        printf("Erro de alocacao vetorBuscado\n");
        return;
    }

    for (uint32_t i = 0; i < *qtdBuscas; i++)
    {
        if (fgets(linha, sizeof(linha), input) == NULL || sscanf(linha, "%llu", &((*vetorBuscado)[i])) != 1)
        {
            printf("Erro ao ler isbn buscado\n");
            return;
        }
    }
}

int32_t buscaBinaria(livros *biblioteca, uint32_t qtdLivros, uint64_t isbn_alvo, uint32_t *qtdChamadas)
{
    *qtdChamadas = 0;
    int32_t i = 0, j = qtdLivros - 1;

    while (i <= j)
    {
        int32_t p = i + (j - i) / 2;
        (*qtdChamadas)++;
        if (biblioteca[p].isbn == isbn_alvo)
            return p;
        if (biblioteca[p].isbn > isbn_alvo)
            j = p - 1;
        else
            i = p + 1;
    }
    return -1;
}

int32_t buscaInterpolada(livros *biblioteca, uint32_t qtdLivros, uint64_t isbn_alvo, uint32_t *qtdChamadas)
{
    *qtdChamadas = 0;
    int32_t i = 0, j = qtdLivros - 1;

   
    while (i <= j && isbn_alvo >= biblioteca[i].isbn && isbn_alvo <= biblioteca[j].isbn)
    {
        (*qtdChamadas)++; 

        if (biblioteca[i].isbn == biblioteca[j].isbn)
        {
            if (biblioteca[i].isbn == isbn_alvo)
                return i;
            break; 
        }

        
        int32_t p = i + ((biblioteca[j].isbn - biblioteca[i].isbn)) % (j - i + 1);

      
        if (p < i) p = i;
        if (p > j) p = j;

        if (biblioteca[p].isbn == isbn_alvo)
            return p;

        if (biblioteca[p].isbn < isbn_alvo)
            i = p + 1;
        else
            j = p - 1;
    }
    

    if (*qtdChamadas == 0 && qtdLivros > 0) {
        (*qtdChamadas)++;
    }

    if (isbn_alvo != biblioteca[i].isbn && isbn_alvo != biblioteca[j].isbn && *qtdChamadas == 1 && qtdLivros > 0) {
        (*qtdChamadas)++;
    }


    return -1; 
}

void substituirEspacos(char *str)
{
    for (; *str; str++)
    {
        if (*str == ' ')
            *str = '_';
    }
}

void processamento(FILE *input, FILE *output)
{
    livros *biblioteca = NULL;
    int64_t *vetorBuscado = NULL;
    uint32_t qtdLivros = 0, qtdBuscas = 0;

    processamentoEntrada(input, &biblioteca, &qtdLivros, &vetorBuscado, &qtdBuscas);
    if (!biblioteca || !vetorBuscado)
        return;

    ordenacao(biblioteca, qtdLivros);

    uint32_t somaBin = 0, somaInt = 0;
    uint32_t vitoriasBin = 0, vitoriasInt = 0;

    for (uint32_t i = 0; i < qtdBuscas; i++)
    {
        uint32_t chamadasBin = 0, chamadasInt = 0;
        int32_t posBin = buscaBinaria(biblioteca, qtdLivros, vetorBuscado[i], &chamadasBin);
        int32_t posInt = buscaInterpolada(biblioteca, qtdLivros, vetorBuscado[i], &chamadasInt);

        somaBin += chamadasBin;
        somaInt += chamadasInt;

        if (chamadasBin < chamadasInt)
            vitoriasBin++;
        else if (chamadasInt <= chamadasBin)
            vitoriasInt++;

        if (posBin != -1)
        {
            char autor[50], titulo[100];
            strcpy(autor, biblioteca[posBin].autor);
            strcpy(titulo, biblioteca[posBin].titulo);
            substituirEspacos(autor);
            substituirEspacos(titulo);

            fprintf(output, "[%llu]B=%u|I=%u|Author:%s,Title:%s\n",
                    vetorBuscado[i], chamadasBin, chamadasInt,
                    autor, titulo);
        }
        else
        {
            fprintf(output, "[%llu]B=%u|I=%u|ISBN_NOT_FOUND\n",
                    vetorBuscado[i], chamadasBin, chamadasInt);
        }
    }

    uint32_t mediaBin = (qtdBuscas > 0) ? (somaBin / qtdBuscas) : 0;
    uint32_t mediaInt = (qtdBuscas > 0) ? (somaInt / qtdBuscas) : 0;

    fprintf(output, "BINARY=%u:%u\n", vitoriasBin, mediaBin);
    fprintf(output, "INTERPOLATION=%u:%u\n", vitoriasInt, mediaInt);

    free(biblioteca);
    free(vetorBuscado);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Uso: %s <input> <output>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    if (!input || !output)
    {
        printf("Erro ao abrir arquivos\n");
        return 1;
    }

    processamento(input, output);

    fclose(input);
    fclose(output);
    return 0;
}

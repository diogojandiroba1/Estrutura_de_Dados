#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct livros{

char autor[50];
char titulo[100];
uint64_t isbn;

} livros;


void ordenacao(livros* biblioteca, uint32_t qtdLivros){

for (uint32_t i = 1; i < qtdLivros; i++) {
livros chave = biblioteca[i]; 
int j = i - 1;
  
while (j >= 0 && biblioteca[j].isbn > chave.isbn) {
    biblioteca[j + 1] = biblioteca[j];
    j--;
}
biblioteca[j + 1] = chave;
}
}


void processamentoEntrada(FILE* input, livros** biblioteca, uint32_t* qtdLivros){
char linha[100];    
  
if(fgets(linha, sizeof(linha), input) == NULL) {
    printf("Erro na leitura da linha\n");
    return;
}
if(sscanf(linha, "%d", qtdLivros) != 1){
    printf("Erro ao ler qtd entradas e servers\n");
    return;
}

*biblioteca = malloc(sizeof(livros) * (*qtdLivros));

if(biblioteca == NULL){
    return;
}

for(int i = 0; i < *qtdLivros; i++){

if (fgets(linha, sizeof(linha), input) == NULL) {
    printf("Erro na leitura da linha\n");
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
}


int32_t buscaBinaria(livros *biblioteca, uint32_t qtdLivros, uint64_t isbn_alvo){

uint32_t qtdChamadas = 0;
int32_t i = 0, j = qtdLivros - 1;

while (i <= j) {
int32_t p = i + (j - i) / 2;
qtdChamadas++;

if(biblioteca[p].isbn == isbn_alvo)
return qtdChamadas;

else if (biblioteca[p].isbn > isbn_alvo)
j = p - 1;
    
else
i = p + 1;
}

return -1;
}


int32_t buscaInterpolada(livros *biblioteca, uint32_t qtdLivros, uint64_t isbn_alvo){

uint32_t qtdChamadas = 0;
int32_t i = 0, j = qtdLivros - 1;

while (i <= j && isbn_alvo >= biblioteca[i].isbn && isbn_alvo <= biblioteca[j].isbn) {

int32_t mod = (j - i + 1);
int32_t dif = biblioteca[j].isbn - biblioteca[i].isbn;
if (dif == 0) break; 
if (mod == 0) break;  

int32_t p = i + (dif % mod);
qtdChamadas++;

if (p < i || p > j) break;

if(biblioteca[p].isbn == isbn_alvo)
return qtdChamadas;

else if (biblioteca[p].isbn > isbn_alvo)
j = p - 1;
    
else
i = p + 1;
}

return -1;
}


void processamento(FILE* input){

livros* biblioteca = NULL;
uint32_t qtdLivros = 0;
processamentoEntrada(input, &biblioteca, &qtdLivros);
ordenacao(biblioteca, qtdLivros);

for(int i = 0; i < qtdLivros; i++){
int32_t chamadasBinarias = buscaBinaria()



}
}


int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");

processamento(input);


fclose(input);
fclose(output);
return 0;
}

// Processar o input verdadeiro
// Gerar saida
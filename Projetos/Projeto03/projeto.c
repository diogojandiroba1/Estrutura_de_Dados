#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct entrada{

char nome[100];

}entrada;


typedef struct servidor{
   
entrada* listaEntradas;
uint32_t numEntradasAlocadas;

} servidor;

servidor* criaArray(uint32_t numServers, uint32_t numEntradas){ 

servidor* servidores = (servidor*) malloc(sizeof(servidor) * numServers);
if(servidores == NULL) return NULL;
memset(servidores, 0, sizeof(servidor) * numServers);
for(int i = 0; i < numServers; i++){

servidores[i].listaEntradas = (entrada*) malloc(sizeof(entrada) * numEntradas);
memset(servidores[i].listaEntradas, 0, sizeof(entrada) * numEntradas);
servidores[i].numEntradasAlocadas = numEntradas;

}

return servidores;
}

char* espacoPorUnderline(char* nome){
    uint32_t acc = 0;
    uint32_t acc2 = 0;
while(nome[acc] != '\0'){
if(isdigit(nome[acc])){
acc++;
}else{
if(nome[acc] == ' '){
    nome[acc2] = '_';
}else{
    nome[acc2] = nome[acc];
}
acc++;
acc2++;
}}
nome[acc2] = '\0';
return nome;
}

uint8_t somaASCI(char* nome){
if(nome == NULL) return 0;
uint8_t soma = 0;

for(int i = 0; nome[i] != '\0';i++){
if(nome[i] == '_') continue;
soma ^= (unsigned int)nome[i];

}
return soma;
}

uint32_t hash1(char* nome){

uint32_t hash1 = 0;
hash1 = ( 7919 * somaASCI(nome) ) % 10007;
return hash1;
}

uint32_t hash2(char* nome){

uint32_t hash2 = 0;
hash2 = (104729 * somaASCI(nome) + 123) % 10007;
return hash2;
}


void processamento(FILE* input){

    uint32_t numServers;
    uint32_t capacidade;
    uint32_t numEntradas;
    uint32_t hashAtual;
    char nomeTemp[100];
    char linha[200];
    if (fgets(linha, sizeof(linha), input) == NULL) {
        printf("Erro na leitura da linha\n");
        return;
    }
    if(sscanf(linha, "%u %u", &numServers, &capacidade) != 2){
        printf("Erro ao ler qtd entradas e servers\n");
        return;
    }

    if (fgets(linha, sizeof(linha), input) == NULL) {
        printf("Erro na leitura da linha\n");
        return;
    }
    if(sscanf(linha, "%u", &numEntradas) != 1){
        printf("Erro ao ler qtd entradas\n");
        return;
    }
   servidor* Lista = criaArray(numServers, capacidade);

        for (uint32_t i = 0; i < numEntradas; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            printf("Erro na leitura da linha\n");
            return;
        }
        strcpy(nomeTemp,espacoPorUnderline(linha));
        hashAtual = hash1(nomeTemp);

        if(Lista->listaEntradas[])
    }



}



int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");


processamento(input);

fclose(input);
fclose(output);
return 1;
}
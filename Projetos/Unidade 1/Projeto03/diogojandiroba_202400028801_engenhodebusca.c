#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct entrada{

char nome[1025];

}entrada;


typedef struct servidor{
   
entrada* listaEntradas;
uint32_t capacidadeMaxima;
uint32_t entradasAlocadas;

} servidor;

servidor* criaArray(uint32_t numServers, uint32_t capacidade){ 

servidor* servidores = (servidor*) malloc(sizeof(servidor) * numServers);
if(servidores == NULL) return NULL;
memset(servidores, 0, sizeof(servidor) * numServers);
for(int i = 0; i < numServers; i++){
servidores[i].listaEntradas = (entrada*) malloc(sizeof(entrada) * capacidade);
        if (servidores[i].listaEntradas == NULL) {
          
            for (uint32_t j = 0; j < i; j++) {
                free(servidores[j].listaEntradas);
            }
            free(servidores);
            return NULL;
        }
memset(servidores[i].listaEntradas, 0, sizeof(entrada) * capacidade);
servidores[i].entradasAlocadas = 0;
servidores[i].capacidadeMaxima = capacidade;
}
return servidores;
}


void liberaArray(servidor* servidores, uint32_t numServers){
    if (servidores == NULL) return;
    for(int i = 0; i < numServers; i++){
        free(servidores[i].listaEntradas);
    }
    free(servidores);
}


char* processaEntrada(char* linha) {

    char* inicio_nome = strchr(linha, ' ');


    if (inicio_nome != NULL) {
        inicio_nome++;
    } else {
        inicio_nome = linha;
    }


    size_t len = strlen(inicio_nome);
    if (len > 0 && inicio_nome[len - 1] == '\n') {
        inicio_nome[len - 1] = '\0';
        len--;
    }

    char* nomeProcessado = malloc(len + 1);
    if (nomeProcessado == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        if (inicio_nome[i] == ' ') {
            nomeProcessado[i] = '_';
        } else {
            nomeProcessado[i] = inicio_nome[i];
        }
    }
    nomeProcessado[len] = '\0';

    return nomeProcessado;
}


uint32_t somaASCI(char* nome){
if(nome == NULL) return 0;
uint32_t soma = 0;

for(int i = 0; nome[i] != '\0';i++){
if(nome[i] == '_') continue;
soma ^= (unsigned int)nome[i];

}
return soma;
}


uint32_t hash1(char* nome, uint32_t T){

uint32_t hash1 = 0;
hash1 = ( 7919 * (uint32_t)somaASCI(nome) % T );
return hash1;
}

uint32_t hash2(char* nome, uint32_t T){

uint32_t hash2 = 0;
hash2 = ((104729 * (uint32_t)somaASCI(nome) + 123) % T );
return (hash2 == 0) ? 1 : hash2;
}

void imprimirServidor(FILE* output, uint32_t server_idx, servidor* srv_data) {
    fprintf(output, "S%u:", server_idx);
    uint8_t first_item = 1;
    for (uint32_t m = 0; m < srv_data->capacidadeMaxima; m++) {
        if (srv_data->listaEntradas[m].nome[0] != '\0') {
            if (!first_item) {
                fprintf(output, ",");
            }
            fprintf(output, "%s", srv_data->listaEntradas[m].nome);
            first_item = 0;
        }
    }
    fprintf(output, "\n");
}

void processamento(FILE* input, FILE* output){

    uint32_t numServers;
    uint32_t capacidade;
    uint32_t numEntradas;
    char linha[1024];

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
    if (Lista == NULL) {
        printf("Erro ao alocar array de servidores\n");
        return;
    }

        for (uint32_t i = 0; i < numEntradas; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            printf("Erro na leitura da linha\n");
            return;
        }
        char* nomeProcessado = processaEntrada(linha);
        if (nomeProcessado == NULL) {
        printf("Erro ao processar nome da entrada %u\n", i + 1);
        continue;
        }

        uint32_t h1 = hash1(nomeProcessado, numServers);
        uint32_t h2 = hash2(nomeProcessado, numServers);

        uint32_t posicao_orig = h1;
        uint32_t posicao_atual;
        uint8_t inserido = 0;

 for (uint32_t j = 0; j < numServers; j++) {
        posicao_atual = (h1 + j * h2) % numServers;

            if (Lista[posicao_atual].entradasAlocadas < Lista[posicao_atual].capacidadeMaxima) {

                for (uint32_t k = 0; k < Lista[posicao_atual].capacidadeMaxima; k++) {

                    if (Lista[posicao_atual].listaEntradas[k].nome[0] == '\0') { 
                        if (j > 0) { 
                            fprintf(output, "S%u->S%u\n", posicao_orig, posicao_atual);
                        }
                        strncpy(Lista[posicao_atual].listaEntradas[k].nome, nomeProcessado, 1024);
                        Lista[posicao_atual].listaEntradas[k].nome[1024] = '\0';
                        Lista[posicao_atual].entradasAlocadas++;
                        inserido = 1;
                        break; 
                    }
                }

            }

               if (inserido) {
                imprimirServidor(output, posicao_atual, &Lista[posicao_atual]);
                break;
            }

        }
        if (!inserido) {
        printf("Não foi possível alocar a entrada '%s'\n", nomeProcessado);
        }
        free(nomeProcessado);
      
    }
     liberaArray(Lista, numServers);
}


int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");


processamento(input,output);

fclose(input);
fclose(output);
return 0;
}
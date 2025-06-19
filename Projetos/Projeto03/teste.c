#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct entrada{
    char nome[101];
} entrada;

typedef struct servidor{
    entrada* listaEntradas;
    uint32_t capacidadeMaxima; // Capacidade máxima para este servidor
    uint32_t entradasAtuais; // Número de entradas atualmente alocadas
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
        servidores[i].capacidadeMaxima = capacidade;
        servidores[i].entradasAtuais = 0;
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

char* processaEntrada(char* nome){
    uint32_t accLer = 0;
    uint32_t accEscreve = 0;

    size_t len = strlen(nome);
    // Remove o '\n' se existir
    if (len > 0 && nome[len-1] == '\n') {
        len--;
    }
    
    char* nomeProcessado = malloc(len + 1); // +1 para o '\0'
    if (nomeProcessado == NULL) return NULL;

    for(int i = 0; i < len; i++){
        // No exercício, o exemplo tem "1ufs", "2a_b_c". Os dois primeiros caracteres são ignorados.
        // Se a intenção é ignorar os dois primeiros caracteres de cada linha, a lógica é essa.
        // Caso contrário, ajuste 'i <= 1' para 'i < 1' ou remova se a entrada for só o nome.
        if (i < 2) { // Ignora os dois primeiros caracteres (ex: "1 ", "2 ")
            continue;
        }
        if(nome[i] == ' '){
            nomeProcessado[accEscreve] = '_';
        }else{
            nomeProcessado[accEscreve] = nome[i];
        }
        accEscreve++;
    }
    nomeProcessado[accEscreve] = '\0';
    return nomeProcessado;
}

uint8_t somaASCI(char* nome){
    if(nome == NULL) return 0;
    uint8_t soma = 0;
    for(int i = 0; nome[i] != '\0'; i++){
        // O exemplo de checksum com 'ufs' não ignora '_', então talvez não devesse ignorar aqui.
        // O exercício menciona "checksum com 8 bits para cada um dos caracteres".
        // A função de checksum do exercício faz XOR com os valores ASCII.
        // Se o underscore '_' deve ser tratado como um caractere normal no checksum, remova a condição.
        // Por exemplo: checksum("a_b") = 'a' XOR '_' XOR 'b'
        soma ^= (uint8_t)nome[i];
    }
    return soma;
}

uint32_t hash1(char* nome, uint32_t T){
    // H1(x) = 7919 * checksum(x) mod T 
    return (7919 * (uint32_t)somaASCI(nome)) % T;
}

uint32_t hash2(char* nome, uint32_t T){
    // H2(x) = 104729 * checksum(x) + 123 mod T 
    uint32_t h2_val = ((104729 * (uint32_t)somaASCI(nome)) + 123) % T;
    // H2(x) não pode retornar 0 para evitar problemas na sonda linear
    return (h2_val == 0) ? 1 : h2_val; 
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
    char linha[200];

    if (fgets(linha, sizeof(linha), input) == NULL) {
        printf("Erro na leitura da linha de numServers e capacidade\n");
        return;
    }
    if(sscanf(linha, "%u %u", &numServers, &capacidade) != 2){
        printf("Erro ao ler numServers e capacidade\n");
        return;
    }

    if (fgets(linha, sizeof(linha), input) == NULL) {
        printf("Erro na leitura da linha de numEntradas\n");
        return;
    }
    if(sscanf(linha, "%u", &numEntradas) != 1){
        printf("Erro ao ler numEntradas\n");
        return;
    }
    
    servidor* Lista = criaArray(numServers, capacidade);
    if (Lista == NULL) {
        printf("Erro ao alocar array de servidores\n");
        return;
    }

    for (uint32_t i = 0; i < numEntradas; i++) {
        if (fgets(linha, sizeof(linha), input) == NULL) {
            printf("Erro na leitura da linha da entrada %u\n", i + 1);
            break; 
        }
        char* nomeProcessado = processaEntrada(linha);
        if (nomeProcessado == NULL) {
            printf("Erro ao processar nome da entrada %u\n", i + 1);
            continue;
        }

        uint32_t h1_val = hash1(nomeProcessado, numServers);
        uint32_t h2_val = hash2(nomeProcessado, numServers);
        uint32_t pos_original = h1_val;
        uint32_t pos_atual;
        uint8_t inserido = 0;

        for (uint32_t j = 0; j < numServers; j++) { // Iterar para encontrar uma posição usando double hashing
            pos_atual = (h1_val + j * h2_val) % numServers;

            if (Lista[pos_atual].entradasAtuais < Lista[pos_atual].capacidadeMaxima) {
                // Encontrou espaço no servidor pos_atual
                for (uint32_t k = 0; k < Lista[pos_atual].capacidadeMaxima; k++) {
                    if (Lista[pos_atual].listaEntradas[k].nome[0] == '\0') { // Encontrou slot vazio
                        if (j > 0) { // Se houve realocação
                            fprintf(output, "S%u->S%u\n", pos_original, pos_atual);
                        }
                        strncpy(Lista[pos_atual].listaEntradas[k].nome, nomeProcessado, 100);
                        Lista[pos_atual].listaEntradas[k].nome[100] = '\0';
                        Lista[pos_atual].entradasAtuais++;
                        inserido = 1;
                        break; 
                    }
                }
            }
            if (inserido) {
                imprimirServidor(output, pos_atual, &Lista[pos_atual]);
                break;
            }
        }

        if (!inserido) {
            // Caso não consiga inserir em nenhum servidor (todos cheios ou loop)
            printf("Não foi possível alocar a entrada '%s'\n", nomeProcessado);
        }
        free(nomeProcessado); // Liberar a memória alocada para nomeProcessado
    }
    liberaArray(Lista, numServers);
}

int main(int argc, char* argv[]){
    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1],"r");
    if (input == NULL) {
        printf("Erro ao abrir arquivo de entrada: %s\n", argv[1]);
        return 1;
    }

    FILE* output = fopen(argv[2],"w");
    if (output == NULL) {
        printf("Erro ao abrir arquivo de saída: %s\n", argv[2]);
        fclose(input);
        return 1;
    }

    processamento(input,output);

    fclose(input);
    fclose(output);
    return 0; // Retorna 0 para sucesso
}
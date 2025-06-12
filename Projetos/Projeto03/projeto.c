#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct servidor{

char nome[100];    
struct servidor* prox;


} servidor;

servidor* criaArray(uint32_t n){ 

servidor* servidores = malloc(sizeof(servidor) * n);
if(servidores == NULL) return NULL;
memset(servidores, 0, sizeof(servidor));
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

uint32_t somaASCI(char* nome){
if(nome == NULL) return;
uint32_t soma = 0;

for(int i = 0; nome[i] != '\0';i++){
if(nome[i] == '_') continue;
soma += (unsigned int)nome[i];

}
return soma;
}


uint32_t processaHash(char* nome, servidor* servidorArray){

if(nome == NULL) return -1;

uint32_t hash1 = 0;
uint32_t hash2 = 0;

hash1 = ( 7919 * somaASCI(nome) ) % 10007;
hash2 = (104729 * somaASCI(nome) + 123) % 10007;

for(int i = 0; )

}



int main(){



return 1;
}
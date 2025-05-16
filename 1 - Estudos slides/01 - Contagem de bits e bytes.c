#include <stdio.h>
#include <stdint.h>

typedef struct carro{
    uint32_t velocidade;
    uint32_t velocidade2;
    uint32_t velocidade3;
    uint64_t velocidade1;
    char* nome;

} carro;   


int main(){

carro audi;

audi.nome = "Diogo";
audi.velocidade = 200;

printf("Tamanho do struct: %d\n", sizeof(audi));
//Sempre é divisivel por 8 (8;16;24;32...)
//Tem diferença entre sistemas 32 e 64 bits
}
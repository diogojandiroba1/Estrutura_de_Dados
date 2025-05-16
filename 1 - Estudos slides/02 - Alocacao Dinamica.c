#include <stdio.h>
#include <stdint.h>

int main(){

//USANDO MALLOC:

uint32_t* vetor = NULL;

vetor = (uint32_t*)(malloc(100 * sizeof(uint32_t)));

if(vetor == NULL) printf("Erro na alocacao\n");

else printf("Alocacao bem sucedida\n");

vetor[0] = 10;

printf("%d\n\n", vetor[0]);


//USANDO CALLOC

uint32_t* vetor2 = NULL;

vetor2 = (uint32_t*)(calloc(100,sizeof(uint32_t)));

if(vetor2 == NULL) printf("Erro na alocacao\n");

else printf("Alocacao bem sucedida\n");

vetor2[0] = 15;

printf("%d\n\n", vetor2[0]);


//REALOCANDO O PRIMEIRO VETOR

uint32_t* r = (uint32_t*)(realloc(vetor,100 * sizeof(uint32_t)));

if(r == NULL) printf("Erro na Realocacao\n");

else printf("Realocacao bem sucedida\n");

r[0] = 20;

printf("%d\n", r[0]);

free(vetor);
free(r);
free(vetor2);
return 0;

//USAR DR MEMORY PARA VER MEMORYS LEAKS
}
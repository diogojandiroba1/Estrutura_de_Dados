#include <stdio.h>
#include <stdint.h> 

void trocar(uint32_t* a, uint32_t* b){
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

void selection_sort(uint32_t* V, uint32_t n){

for(uint32_t i = 0; i < n -1; i++){
uint32_t min = i;
for(uint32_t j = i + 1; j < n; j++){
if(V[j] < V[min]) min = j;
}
if(i != min) trocar(&V[i], &V[min]);
}
}

void main(){

uint32_t V[] = {2,3,1,4,2,5,6,3,5,6,78,3,5,1,5,66,3,4,98,1};
uint32_t n = sizeof(V) / sizeof(V[0]);

printf("Vetor Original: \n");
for(int i = 0; i < n; i++){

    printf("%u ", V[i]);
}
printf("\n\n");
selection_sort(V, n);

printf("Vetor Ordenado: \n");
for(int i = 0; i < n; i++){

    printf("%u ", V[i]);
}
}
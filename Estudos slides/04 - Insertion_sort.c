#include <stdio.h>
#include <stdint.h>

void trocar(uint32_t* a, uint32_t* b){
    uint32_t temp = *a;
    *a = *b;
    *b = temp;
}

void insection_sort(uint32_t* V, uint32_t n){
for(uint32_t i = 1; i < n; i++){
for(uint32_t j = i; j > 0 && V[j - 1] > V[j]; j--){
trocar(&V[j], &V[j - 1]);
}
}
}




int main(){

uint32_t V[] = {2,3,1,4,2,5,6,3,5,6,78,3,5,1,5,66,3,4,98,1};
uint32_t n = sizeof(V) / sizeof(V[0]);

printf("Vetor Original: \n");
for(int i = 0; i < n; i++){

    printf("%u ", V[i]);
}
printf("\n\n");
insection_sort(V, n);

printf("Vetor Ordenado: \n");
for(int i = 0; i < n; i++){

    printf("%u ", V[i]);
}

return 0;
}
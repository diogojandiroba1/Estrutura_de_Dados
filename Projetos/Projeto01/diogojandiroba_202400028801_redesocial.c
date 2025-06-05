#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


typedef struct elemento{

char nome[50];    
struct elemento* prox;

}elemento;

typedef struct lista{

elemento* ultimo;

}lista;


void iniciaLista(lista* l){
l->ultimo = NULL;
}


void adicionarNome(FILE* out, char* nome, lista* l){

//Verifica lista nula    
if(l == NULL){
    fprintf(out,"[FAILURE]ADD=%s\n",nome);
    return;
}

//Verifica nome maior de 50 caracteres

if(strlen(nome) >= 50){

    fprintf(out,"[FAILURE]ADD=%s\n",nome);
    return;
}


//Caso a lista for vazia:
if(l->ultimo == NULL){
elemento* novoVazia = malloc(sizeof(elemento));
if(novoVazia == NULL){
    fprintf(out,"[FAILURE]ADD=%s\n",nome);
    return;
}

    strcpy(novoVazia->nome, nome);

    novoVazia->prox = novoVazia;
    l->ultimo = novoVazia;

    fprintf(out, "[SUCCESS]ADD=%s\n", nome);
    return;

}

//Verificar duplicada

elemento* atual = l->ultimo->prox;
do{
if(strcmp(atual->nome, nome) == 0){
    fprintf(out,"[FAILURE]ADD=%s\n",nome);
    return;
}

atual = atual->prox;
}while(atual != l->ultimo->prox);

elemento* novoNome = (elemento*) malloc(sizeof(elemento));

if(novoNome == NULL){
    fprintf(out,"[FAILURE]ADD=%s\n",nome);
    return;
}

strcpy(novoNome->nome, nome);

novoNome->prox = l->ultimo->prox;
l->ultimo->prox = novoNome;
l->ultimo = novoNome;

fprintf(out, "[SUCCESS]ADD=%s\n", nome);
}


void removerElemento(FILE* out, char* nome, lista* l){
if(l == NULL){
    fprintf(out,"[FAILURE]REMOVE=%s\n",nome);
    return; 
}
if(l->ultimo == NULL){
    fprintf(out,"[FAILURE]REMOVE=%s\n",nome);
    return; 
}

elemento* atual = l->ultimo->prox;
elemento* anterior = l->ultimo;

do{
if(strcmp(atual->nome, nome) == 0){
    if(atual == l->ultimo && atual->prox == atual){
        l->ultimo = NULL;
    }else{
    anterior->prox = atual->prox;
        if(atual == l->ultimo){
            l->ultimo = anterior;
        }
}
   fprintf(out,"[SUCCESS]REMOVE=%s\n",nome);
   free(atual);
    return; 

}

anterior = atual;
atual = atual->prox;
}while(atual != l->ultimo->prox);
fprintf(out,"[FAILURE]REMOVE=%s\n",nome);
}




void mostrarElemento(lista* l, char* nome, FILE* out){
if(l == NULL){
    fprintf(out,"[FAILURE]SHOW=%s\n",nome);
    return; 
}
if(l->ultimo == NULL){
    fprintf(out,"[FAILURE]SHOW=%s\n",nome);
    return; 
}

elemento* atual = l->ultimo->prox;
elemento* anterior = l->ultimo;

do{
if(strcmp(atual->nome, nome) == 0){
    if(atual == l->ultimo && atual->prox == atual){
        fprintf(out,"[SUCCESS]SHOW=%s<-%s->%s\n",nome,atual->nome,nome);
        return;
    }else{
        fprintf(out,"[SUCCESS]SHOW=%s<-%s->%s\n",anterior->nome,nome,atual->prox->nome);
        return;
}
}

anterior = atual;
atual = atual->prox;
}while(atual != l->ultimo->prox);

fprintf(out,"[FAILURE]SHOW=%s\n",nome);
}


char* espacoPorUnderline(char* nome){
for(int i = 0; i < strlen(nome); i++){
if(nome[i] == ' '){
    nome[i] = '_';
}
}
return nome;
}

void liberarMemoria(lista* l){

if(l == NULL){
    return; 
}
if(l->ultimo == NULL){
    return; 
}

elemento* atual = l->ultimo->prox;
elemento* proximo;
elemento* primeiro = atual;

 if(atual == l->ultimo && atual->prox == atual){
    l->ultimo = NULL;
    free(atual);
    return;
}

do{
proximo = atual -> prox;
free(atual);
atual = proximo;
}while(atual != primeiro);

l->ultimo = NULL;

}

int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");

char linha[100];
char funcao[100];
char nome[100];

if(input == NULL || output == NULL) return 1;


lista minhalista;
iniciaLista(&minhalista);

while(fgets(linha,sizeof(linha),input) != NULL){
sscanf(linha, "%s %[^\n]", funcao, nome);


if(strcmp("ADD",funcao) == 0){
adicionarNome(output,espacoPorUnderline(nome),&minhalista);

}else if(strcmp("REMOVE",funcao) == 0){

removerElemento(output,espacoPorUnderline(nome),&minhalista);

}else if(strcmp("SHOW",funcao) == 0){

mostrarElemento(&minhalista,espacoPorUnderline(nome),output);

}else{

fprintf(output,"[FAILURE] COMANDO INVALIDO: %s\n",funcao);

}
}

liberarMemoria(&minhalista);
fclose(input);
fclose(output);
return 0;   
}

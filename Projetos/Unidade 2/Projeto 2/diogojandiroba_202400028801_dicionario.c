#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>




//Struct of node
typedef struct node{
int8_t height;
int8_t qtd_synonyms;  
char key_word[31];
char synonyms[10][31];
struct node* right;
struct node* left;
} node;




//Struct of AvlTree
typedef struct AVLtree{
node* root;
}AVLtree;




//Function to get the height of subTree
int8_t get_height(node* current_node){
    if(current_node == NULL) return -1;
    return current_node->height;
}




//Function for get the balancing number
int32_t get_balanced(node* current_node) {
 if(current_node == NULL) return 0;

 return get_height(current_node->left) - get_height(current_node->right);
}




//Function for formating the line
void formatLine(char* key_word, char synonums_array[10][31], char* line, int64_t* qtd_synonums){
char* stopLine;
char qtd_temp[150];

stopLine = strtok(line," ");
if(stopLine !=NULL){
strcpy(key_word, stopLine);
}

stopLine = strtok(NULL," ");
if(stopLine !=NULL){
strcpy(qtd_temp, stopLine);
*qtd_synonums = strtoull(stopLine, NULL, 10);
}

for(int i = 0; i < *qtd_synonums; i++){
stopLine = strtok(NULL," ");
if(stopLine !=NULL){
strcpy(synonums_array[i], stopLine);
}else{   break;  }
}
}




//Function for inicialize the tree
void inicializeTree(AVLtree* AvlTreeSeach){
AvlTreeSeach->root = NULL; 
}



// Tree rotations logic
//Function to rotate for right the subTree 
node* rotation_left(node* root){
node* axis = root->right;
root->right = axis->left;
axis->left = root;

root->height = fmax(get_height(root->left), get_height(root->right)) + 1;
    
axis->height = fmax(get_height(axis->left), get_height(axis->right)) + 1;

return axis;
}



//Function to rotate for right the subTree 
node* rotation_right(node* root){
node* axis = root->left;
root->left = axis->right;
axis->right = root;
 
root->height = fmax(get_height(root->left), get_height(root->right)) + 1;
    
axis->height = fmax(get_height(axis->left), get_height(axis->right)) + 1;

return axis;
}



//Function to create a node
node* createNewNode(char* word, char synonums_array[10][31], int qtd_synonyms) {
    node* newNode = (node*)malloc(sizeof(node));

    strcpy(newNode->key_word, word);

    for (int i = 0; i < 10; i++) {
        newNode->synonyms[i][0] = '\0';
    }
   
    for (int i = 0; i < qtd_synonyms && i < 10; i++) {
        strcpy(newNode->synonyms[i], synonums_array[i]);
    }

    newNode->height = 0;
    newNode->qtd_synonyms = qtd_synonyms;                  
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}



//Function to add a node created
node* addNode(node* current_node, char* word, char synonums_array[10][31], int qtd_synonums){

//Base case: Leaf of tree
if(current_node == NULL){
return createNewNode(word, synonums_array, qtd_synonums);
}
int cmp = strcmp(word, current_node->key_word);

if(cmp == 0){
    
}
if(cmp < 0){
    current_node->left = addNode(current_node->left, word, synonums_array, qtd_synonums);
}
if(cmp > 0){
    current_node->right = addNode(current_node->right, word, synonums_array, qtd_synonums);
} 


current_node->height = 1 + fmax(get_height(current_node->left), get_height(current_node->right));

int balance_temp = get_balanced(current_node);

    // case left-left
    if (balance_temp > 1 && strcmp(word, current_node->left->key_word) < 0) {
        return rotation_right(current_node);
    }
    // why -1
    // case right-right
    if (balance_temp < -1 && strcmp(word, current_node->right->key_word) > 0) {
        return rotation_left(current_node);
    }

    // case left-right
    if (balance_temp > 1 && strcmp(word, current_node->left->key_word) > 0) {
        current_node->left = rotation_left(current_node->left);
        return rotation_right(current_node);
    }
    //why -1
    // Case right-left
    if (balance_temp < -1 && strcmp(word, current_node->right->key_word) < 0) {
        current_node->right = rotation_right(current_node->right);
        return rotation_left(current_node);
    }

    return current_node;

//entender essa logica

}



//Function to search a node
node* searchNode(node* current_node, char* word, FILE* output){
if(current_node == NULL){
fprintf(output,"?");   
return NULL;
}
fprintf(output,"%s", current_node->key_word);
int cmp = strcmp(word, current_node->key_word);

if(cmp == 0){
    return current_node;
}else if(cmp < 0){
    fprintf(output,"->");
    return searchNode(current_node->left, word, output);
}else {
    fprintf(output,"->");
return searchNode(current_node->right, word, output);
}
}



//Function that frees the memory allocated
void freeTree(node* current_node) {
    if (current_node != NULL) {
        freeTree(current_node->left);
        freeTree(current_node->right);
        free(current_node);
    }
}


//Function that performs processing of code

void processing(FILE* input, FILE* output){

//inicialize the tree
AVLtree* Tree = (AVLtree*) malloc(sizeof(AVLtree));
inicializeTree(Tree);

node* searchedNode;

char line[400];  
char temp_word[31];
char search_word[31];
char temp_synonums_array[10][31];
int64_t qtd_key_words;
int64_t qtd_key_seach;
int64_t qtd_synonums;

//obtains the number of words and processes them
if(fgets(line, sizeof(line), input) == NULL) {
     printf("Erro na leitura da line\n");
    return;
}
if(sscanf(line, "%"SCNu64, &qtd_key_words) != 1){
    printf("Erro ao ler qtd palavras chaves\n");
    return;
}

//get line by line, process, and add to tree

for(int i = 0; i < qtd_key_words; i++){

if (fgets(line, sizeof(line), input) == NULL) {
     printf("Erro na leitura da line\n");
    return;
}

formatLine(temp_word, temp_synonums_array,line, &qtd_synonums);
Tree->root = addNode(Tree->root, temp_word, temp_synonums_array, qtd_synonums);
}

//get the inputs line by line

if(fgets(line, sizeof(line), input) == NULL) {
     printf("Erro na leitura da line\n");
    return;
}
if(sscanf(line, "%"SCNu64, &qtd_key_seach) != 1){
    printf("Erro ao ler qtd palavras busca\n");
    return;
}


// SEARCH NODE
for(int i = 0; i < qtd_key_seach; i++){

if (fgets(line, sizeof(line), input) == NULL) {
    printf("Erro na leitura da line\n");
    return;
}
if(sscanf(line, "%s", search_word) != 1){
    printf("Erro ao ler a palavra busca\n");
    return;
}

//IF THE WORD NOT THIS IN TREE
fprintf(output,"[");
searchedNode = searchNode(Tree->root, search_word, output);
fprintf(output,"]\n"); 

if(searchedNode == NULL){   
fprintf(output, "-\n"); 
//ELSE
}else{
for(int i = 0; i < searchedNode->qtd_synonyms; i++){
    fprintf(output, "%s", searchedNode->synonyms[i]);
    if(i < searchedNode->qtd_synonyms - 1) {
        fprintf(output, ",");
    }
}
}
}

freeTree(Tree->root);

}




int main(int argc, char* argv[]){

FILE* input = fopen(argv[1],"r");
FILE* output = fopen(argv[2],"w");

processing(input,output);

fclose(input);
fclose(output);
return 0;
}

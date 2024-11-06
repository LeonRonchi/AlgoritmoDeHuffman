#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct bits
{
    unsigned char b7:1;
    unsigned char b6:1;
    unsigned char b5:1;
    unsigned char b4:1;
    unsigned char b3:1;
    unsigned char b2:1;
    unsigned char b1:1;
    unsigned char b0:1;
};

union byte
{
    struct bits bi;
    unsigned char num;
};

struct no{
    int frequencia;
    int codigo;
    struct no *esq,*dir;
};
typedef struct no No;

struct floresta{
    struct floresta *prox;
    struct no *raiz;
};
typedef struct floresta Floresta;

struct registro{
    int codigo;
    char palavra[50];
    int frequencia;
    char huffman[20];
};
typedef struct registro Reg;

struct lista{
    int codigo;
    char palavra[50];
    int frequencia;
    char huffman[20];
    struct lista *prox;
};
typedef struct lista Lista;

No *criaNo(int codigo, int frequencia){
    No *nova = (No*)malloc(sizeof(No));
    nova->frequencia=frequencia;
    nova->codigo=codigo;
    nova->esq = nova->dir=NULL;
    return nova;
}

Floresta *criaFloresta(No *novo){
    Floresta *nova = (Floresta*)malloc(sizeof(Floresta));
    nova->prox=NULL;
    nova->raiz=novo;
    return nova;
}

Lista *criaLista(int cod, char palav[]){
    Lista *nova = (Lista*) malloc(sizeof (Lista));
    nova->codigo = cod;
    strcpy(nova->huffman,"");
    nova->frequencia=1;
    strcpy(nova->palavra, palav);
    nova->prox=NULL;
    return nova;
}

int buscaLista(char busca[], Lista **L){
    Lista *aux = (*L);
    int achou=0;
    while(aux!=NULL && !achou){
        if(stricmp(aux->palavra,busca)==0){
            achou=1;
            aux->frequencia+=1;
        }
        aux=aux->prox;
    }
    return achou;
}

void insereLista(Lista **L, char palavra[], int cod){
    Lista *aux=*L,*ant;
    if(*L==NULL)
        *L=criaLista(cod,palavra);
    else{
        while(aux!=NULL){
            ant=aux;
            aux=aux->prox;
        }
        ant->prox= criaLista(cod,palavra);
    }
}

void leArquivo(Lista **L, FILE *ptr){
    char palavra[50];
    int i=1; //0 fica reservado para os nos que vao surgir na floresta
    if(ptr!=NULL) {
        rewind(ptr);
        while (!feof(ptr)) {
            strcpy(palavra," ");
            if(!buscaLista(palavra,&*L)){
                insereLista(&*L,palavra,i++);
            }
            fscanf(ptr, "%s", palavra);
            //printf("%s\n",palavra);
            if(!buscaLista(palavra,&*L)){
                insereLista(&*L,palavra,i++);
            }
        }
    }
    else{
        printf("Crie um arquivo com o nome 'texto.txt' e coloque seu texto nele.\n");
    }
}

void exibeLista(Lista *L){
    printf("Lista Gerada:\n");
    while(L!=NULL){
        printf("Codigo: %d\tPalavra: %s\tFrequencia:%d\tHuffman:%s\n",L->codigo,L->palavra,L->frequencia,L->huffman);
        L=L->prox;
    }
}

void insereFloresta(Floresta **F, No *novoNo) {
    Floresta *novaFloresta = criaFloresta(novoNo);
    Floresta *atual = *F, *ant = NULL;

    if (*F == NULL) { // Floresta vazia
        *F = novaFloresta;
    } else { // Floresta não vazia
        while (atual != NULL && atual->raiz->frequencia < novoNo->frequencia) {
            ant = atual;
            atual = atual->prox;
        }
        if (ant == NULL) { // Inserir no início
            novaFloresta->prox = *F;
            *F = novaFloresta;
        } else { // Inserir no meio ou no final
            novaFloresta->prox = ant->prox;
            ant->prox = novaFloresta;
        }
    }
}

int tamanhoFloresta(Floresta *F){
    int i;
    for(i=0;F!=NULL;i++)
        F=F->prox;
    return i;
}

void reorganizaFloresta(Floresta **F){
    No *novoNo;
    Floresta *aux;
    while(tamanhoFloresta((*F))>1){ //enquanto não tiver só 1 elemento na floresta
        novoNo=criaNo(0,(*F)->raiz->frequencia + (*F)->prox->raiz->frequencia); //pega a frequencia dos dois primeiros itens e soma
        novoNo->esq = (*F)->raiz;
        novoNo->dir = (*F)->prox->raiz;
        insereFloresta(&(*F),novoNo);
        aux=(*F);
        (*F)=(*F)->prox;
        free(aux);
        aux=(*F);
        (*F)=(*F)->prox;
        free(aux);
    }
}

void exibeArvore(No *raiz){
    static int n= -1;
    if(raiz!=NULL){
        n++;
        exibeArvore(raiz->dir);
        for(int i=0; i< 5*n;i++)
            printf("  ");
        printf("(%d,%d)",raiz->codigo,raiz->frequencia);
        exibeArvore(raiz->esq);
    }
}

void pre_ordem_pra_huffman(Lista **L, No *raiz, char huff[]){
    if(raiz!=NULL) {
        char huffdir[50], huffesq[50];
        strcpy(huffdir,huff);
        strcpy(huffesq,huff);
        strcat(huffesq,"0");
        strcat(huffdir,"1");
        if(raiz->dir==NULL && raiz->esq==NULL){ //chegou em uma folha
            Lista *aux = *L;
            while(aux->codigo!=raiz->codigo) //busca na lista quem tem o mesmo
                aux=aux->prox;
            strcpy(aux->huffman,huff);
        }
        pre_ordem_pra_huffman(&(*L),raiz->esq, huffesq);
        pre_ordem_pra_huffman(&(*L),raiz->dir, huffdir);
    }
}

void gera_arquivos(Lista *L){
    FILE *ptr = fopen("texto2.txt","r");
    if(ptr!=NULL){
        FILE *registroLista = fopen("registros.dat","wb+");
        FILE *fraseHuff = fopen("Huffman.dat","wb+");
        Lista *aux = L, *busca;
        Reg registro;
        union byte Un;
        int i;
        char text[5000]="", palavra[50];
        while(aux!=NULL){
            registro.frequencia=aux->frequencia;
            registro.codigo=aux->codigo;
            strcpy(registro.palavra,aux->palavra);
            strcpy(registro.huffman,aux->huffman);
            fwrite(&registro,1,sizeof(Reg),registroLista);
            aux=aux->prox;
        }
        fclose(registroLista);
        while(!feof(ptr)){
            busca=L;
            fscanf(ptr, "%s", palavra);
            while(stricmp(busca->palavra,palavra)!=0)
                busca=busca->prox;
            strcat(text,busca->huffman);
            strcat(text,"1"); //colcoar espaço entre as palavras
        }
        i = strlen(text)%8;
        while(i>0){
            strcat(text,"1");
            i--;
        }
        while(i< strlen(text)){
            Un.bi.b0 = text[i++];
            Un.bi.b1 = text[i++];
            Un.bi.b2 = text[i++];
            Un.bi.b3 = text[i++];
            Un.bi.b4 = text[i++];
            Un.bi.b5 = text[i++];
            Un.bi.b6 = text[i++];
            Un.bi.b7 = text[i++];

            fwrite(&Un.num,sizeof(char),1,fraseHuff);
        }
        printf("%s",text);
        fclose(fraseHuff);
    }
    else
        printf("Por favor, crie um arquivo com o nome 'texto2.txt', utilizando as palavras do arquivo 'texto.txt' que você deseja codificar.");
    fclose(ptr);
}

int main() {
    FILE *ptr = fopen("texto.txt","r");
    Lista *L = NULL, *aux;
    Floresta *F = NULL;
    No *novo;
    char codigodeHuff[50] = "";

    leArquivo(&L,ptr);
    if (L != NULL) { // Para garantir que uma lista foi gerada
        //exibeLista(L);

        aux = L;
        while (aux != NULL) {
            novo = criaNo(aux->codigo, aux->frequencia);
            insereFloresta(&F, novo);
            aux = aux->prox;
        }
        //printf("Floresta inicial gerada com sucesso\n");

        reorganizaFloresta(&F);
        exibeArvore(F->raiz);
        //printf("Floresta reorganizada com sucesso");

        pre_ordem_pra_huffman(&L,F->raiz,codigodeHuff);
        exibeLista(L);

        rewind(ptr);
        gera_arquivos(L);
        printf("\nArquivos gerados com sucesso!\n");

    }

    fclose(ptr);
    return 0;
}

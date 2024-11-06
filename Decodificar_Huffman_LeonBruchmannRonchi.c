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

Lista *criaLista(int cod, char palav[], int frequencia, char huffman[]){
    Lista *nova = (Lista*) malloc(sizeof (Lista));
    nova->codigo = cod;
    strcpy(nova->huffman,huffman);
    nova->frequencia=frequencia;
    strcpy(nova->palavra, palav);
    nova->prox=NULL;
    return nova;
}

No *criaNo(int codigo, int frequencia){
    No *nova = (No*) malloc(sizeof(No));
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

void insereLista(Lista **L, char palavra[], int cod, int frequencia, char huffman[]){
    Lista *aux=*L,*ant;
    if(*L==NULL)
        *L=criaLista(cod,palavra,frequencia,huffman);
    else{
        while(aux!=NULL){
            ant=aux;
            aux=aux->prox;
        }
        ant->prox= criaLista(cod,palavra,frequencia,huffman);
    }
}

void leArquivoRegistros(Lista **L){
    FILE *ptr=fopen("registros.dat","rb");
    Reg aux;
    while(!feof(ptr)){
        fread(&aux,sizeof(Reg),1,ptr);
        insereLista(&(*L),aux.palavra,aux.codigo,aux.frequencia,aux.huffman);
    }
    fclose(ptr);
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

int ehfolha(No *raiz){
    return (raiz->dir && raiz->esq==NULL);
}

void decodifica(No *raiz,Lista *L){
    No *aux;
    Lista *listaaux;
    char texto[5000]="",textodecodificado[50000]="";
    int i=0;
    union byte Un;
    FILE *ptr = fopen("Huffman.dat","rb");
    if(ptr!=NULL){
        fread(&Un.num,sizeof(char),1,ptr);
        while(!feof(ptr)){
            if(Un.bi.b0==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b1==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b2==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b3==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b4==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b5==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b6==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            if(Un.bi.b7==0)
                strcat(texto,"0");
            else
                strcat(texto,"1");
            fread(&Un.num,sizeof(char),1,ptr);
        }
        printf("\n\nTexto codificado:\n%s\n",texto);

        while(i < strlen(texto)){
            aux=raiz;
            while(!ehfolha(raiz)){
                if(texto[i]=='0')
                    aux=aux->esq;
                else
                    aux=aux->dir;
                i++;
            }
            listaaux=L;
            while (listaaux->codigo!=aux->codigo){
                listaaux=listaaux->prox;
            }
            strcat(textodecodificado,listaaux->palavra);
            strcat(textodecodificado," ");
            i++;
        }
    }
    else
        printf("Arquivo da frase codificada não encontrado!\n");
    fclose(ptr);
}

int main(){
    Floresta *F;
    Lista *L,*aux;
    leArquivoRegistros(&L);
    exibeLista(L);
    No *novo;

    aux = L;
    while (aux != NULL) {
        novo = criaNo(aux->codigo, aux->frequencia);
        insereFloresta(&F, novo);
        aux = aux->prox;
    }
    //printf("Floresta inicial gerada com sucesso\n");

    reorganizaFloresta(&F);

    decodifica(F->raiz, L);
    return 0;
}
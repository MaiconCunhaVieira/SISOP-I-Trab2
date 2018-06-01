/*

Universidade Federal do Rio Grande do Sul
Instituto de Informatica
INF01142 - Sistemas Operacionais I N - 2018/1 - Turma B
Prof. Dr. Sergio Luis Cechin
Trabalho Pratico II

Integrantes:
Cassio Ramos - 193028
Giovani Tirello - 252741
Maicon Vieira - 242275

*/

#include "../include/apidisk.h"
#include "../include/bitmap2.h"
#include "../include/t2fs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// **************************************************************************
// Constantes e typedefs

#define NUMERO_MAX_ARQUIVOS_ABERTOS 10
typedef struct t2fs_superbloco SUPERBLOCO;
typedef struct t2fs_record ARQUIVO;
typedef struct t2fs_record DIRETORIO;

// **************************************************************************
// Structs

typedef struct arq_aberto {
    struct t2fs_record arquivo;
    struct t2fs_record diretorio_pai;
    int current_pointer;
    int aberto;

} ARQUIVO_ABERTO;

typedef struct dir_aberto {
    struct t2fs_record diretorio;
    //int entrada_atual;
    int aberto;
} DIRETORIO_ABERTO;

// **************************************************************************
// Variaveis globais

int first_time_running = 1; // indica se eh a primeira vez que o sistema esta rodando
char *caminho_atual; // indica qual o caminho que o sistema esta
SUPERBLOCO superbloco;
ARQUIVO_ABERTO arquivos_abertos[NUMERO_MAX_ARQUIVOS_ABERTOS]; // armazena os arquivos abertos (máximo de 10, como especificado)
DIRETORIO_ABERTO dretorios_abertos[NUMERO_MAX_ARQUIVOS_ABERTOS]; // armazena os diretorios abertos

// **************************************************************************
// Outras funcoes (auxiliares)

void init_file_system()
{
    //caminho_atual = malloc(sizeof(strlen("/")));
    //strcpy(caminho_atual, "/");

    //first_time_running = 0;
}

int nome_correto(char *nome)
{
    if(strlen(nome) > MAX_FILE_NAME_SIZE){
        return 0;
    }
    else {
        for(int i = 0; i < strlen(nome); i++){
            if(nome[i] < 0x21 || nome[i] > 0x7a){
                return 0;
            }
        }
        return 1;
    }
}

// **************************************************************************
// Funcoes obrigatorias (do arquivo t2fs.h)

int identify2 (char *name, int size)
{
    char names[] = "\nCassio Ramos - 193028\nGiovani Tirello - 252741\nMaicon Vieira - 242275\n";

    if(sizeof(names) > size) { // Se o tamanho da variavel 'names' for maior do que o informado em 'size'
        return -1;
    }
    else {
        strcpy(name, names);
        return 0;
    }
}

FILE2 create2 (char *filename)
{
    if(first_time_running)
        init_file_system();

    if(nome_correto(filename)){
        // verificar se não existe arquivo com mesmo nome

        return -1;
    }
    else {
        printf("ERRO: Nome incorreto.");
        return -1;
    }
}

int delete2 (char *filename)
{
    return -1;
}

FILE2 open2 (char *filename)
{
    return -1;
}

int close2 (FILE2 handle)
{
    return -1;
}

int read2 (FILE2 handle, char *buffer, int size)
{
    return -1;
}

int write2 (FILE2 handle, char *buffer, int size)
{
    return -1;
}

int truncate2 (FILE2 handle)
{
    return -1;
}

int seek2 (FILE2 handle, DWORD offset)
{
    return -1;
}

int mkdir2 (char *pathname)
{
    return -1;
}

int rmdir2 (char *pathname)
{
    return -1;
}

int chdir2 (char *pathname)
{
    return -1;
}

int getcwd2 (char *pathname, int size)
{
    return -1;
}

DIR2 opendir2 (char *pathname)
{
    return -1;
}

int readdir2 (DIR2 handle, DIRENT2 *dentry)
{
    return -1;
}

int closedir2 (DIR2 handle)
{
    return -1;
}

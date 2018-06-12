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
typedef struct t2fs_record RECORD;
typedef struct t2fs_inode INODE;
typedef struct t2fs_data DATA;
typedef struct arq_aberto ARQUIVO_ABERTO;
typedef struct dir_aberto DIRETORIO_ABERTO;

// **************************************************************************
// Structs

struct arq_aberto {
  DIRENT2 info;
	char * path;
	int cursor;
	int parentInode;
	int inodeNumber;
	INODE * in;
	DATA * dados;
	DIRETORIO_ENTRADAS* diretorio;
};

/*struct dir_aberto {
    struct t2fs_record diretorio;
    //int entrada_atual;
    int aberto;
};*/

struct t2fs_data {
	int size;
	int parentInode;
	BYTE* data;
};

typedef struct dir_aberto {
	int size;
	RECORD* entry;
} DIRETORIO_ENTRADAS;

// **************************************************************************
// Variaveis globais

int first_time_running = 1; // indica se eh a primeira vez que o sistema esta rodando
char *caminho_atual; // indica qual o caminho que o sistema esta
SUPERBLOCO* superbloco = NULL;
ARQUIVO_ABERTO* arquivos_abertos[NUMERO_MAX_ARQUIVOS_ABERTOS]; // armazena os arquivos abertos (m�ximo de 10, como especificado)
DIRETORIO_ABERTO* diretorios_abertos[NUMERO_MAX_ARQUIVOS_ABERTOS]; // armazena os diretorios abertos
INODE* rootInode = NULL;
//DIRECTORY_RECORD * rootDirectory = NULL;
int totalArqAbertos = 0;

FILE * disk;

// Variveis que armazenam a geometria do disco
int inodeSectorStart;	// Setor onde se iniciam os inodes
int dataSectorStart;	// Setor onde se iniciam os dados
int blockSize;			// Tamanho do bloco (em setores)
int blockSizeInBytes;	// Tamanho do bloco (em bytes)
unsigned long long maxFileSize ;
// **************************************************************************
// Outras funcoes (auxiliares)

// Declaracao das funcoes
INODE* readInode(int inode);
int init_file_system();
BYTE* read_bloco(int setor, int tamanho);
SUPERBLOCO* read_superbloco();
int nome_correto(char *nome);

INODE* readInode(int inode)
{
    int i = 0;
    int sectorNumber = 0;
    int offset = 0;
	BYTE* buffer;
	INODE* in;

    sectorNumber = inodeSectorStart + (inode / (SECTOR_SIZE/16));
	offset = 16 * (inode % (SECTOR_SIZE/16));
    buffer = read_bloco(sectorNumber, 1);

    if (buffer)
	{
		in = calloc(1, sizeof(INODE));

        memcpy(&in->blocksFileSize, buffer+i+offset, 4);
		i += 4;
        memcpy(&in->bytesFileSize, buffer+i+offset, 4);
		i += 4;
		memcpy(&in->dataPtr[0], buffer+i+offset, 4);
		i += 4;
		memcpy(&in->dataPtr[1], buffer+i+offset, 4);
		i += 4;
		memcpy(&in->singleIndPtr, buffer+i+offset, 4);
		i += 4;
		memcpy(&in->doubleIndPtr, buffer+i+offset, 4);
		i += 4;
        memcpy(&in->reservado[0], buffer+i+offset, 4);
		i += 4;
        memcpy(&in->reservado[1], buffer+i+offset, 4);
		i += 4;

		free(buffer);
		return in;
	}

	return NULL;
}

int init_file_system()
{
    int i;
    ARQUIVO_ABERTO* rootFile;
    if (superbloco != NULL)
        return 1;
    superbloco = read_superbloco();
    if (superbloco){
        inodeSectorStart = superbloco->superblockSize + superbloco->freeBlocksBitmapSize + superbloco->freeInodeBitmapSize;
        dataSectorStart = inodeSectorStart + superbloco->inodeAreaSize;
        blockSize = superbloco->blockSize;
        blockSizeInBytes = superbloco->blockSize * SECTOR_SIZE;
        maxFileSize = (blockSizeInBytes*2) + ((blockSizeInBytes/4)*blockSizeInBytes) + ((blockSizeInBytes/4)*(blockSizeInBytes/4)*blockSizeInBytes);
        rootInode = readInode(0);
        for (i = 0; i < NUMERO_MAX_ARQUIVOS_ABERTOS; i++)
            arquivos_abertos[i] = NULL;
        if (rootInode) {
  			rootFile = calloc(1, sizeof(ARQUIVO_ABERTO));
        rootFile->cursor = 0;
			  rootFile->path = calloc(2, sizeof(char));
			sprintf(rootFile->path, "/");
			rootFile->inodeNumber = 0;
			rootFile->parentInode = 0;
			rootFile->in = rootInode;
		//	rootFile->dados = getInodeData(rootInode); //implementar funcao getInodeData
		//	rootFile->diretorio = readDirectory(rootFile->dados); //implementar funcao readDirectory
      sprintf(rootFile->info.name, "/");
			rootFile->info.fileType = TYPEVAL_DIRETORIO;
			rootFile->info.fileSize = rootFile->dados->size;
			tabelaArquivosAbertos[0] = rootFile;
			return 1;
		}
		return 0;
	}

	return 0;
}
    //caminho_atual = malloc(sizeof(strlen("/")));
    //strcpy(caminho_atual, "/");

    //first_time_running = 0;


BYTE* read_bloco(int setor, int tamanho)
{
    BYTE* buffer = calloc(1, (SECTOR_SIZE * tamanho));
    BYTE* buffer_pointer = buffer;

    for(int i = 0; i < tamanho; i++){
        if(read_sector(setor + i, buffer_pointer) == 0){
            buffer_pointer += SECTOR_SIZE;
        }
        else {
            free(buffer);
            return NULL;
        }
    }

    return buffer;
}

SUPERBLOCO* read_superbloco()
{
    BYTE* buffer;
    SUPERBLOCO* superbloco;
    int i = 0;

    buffer = read_bloco(0, 1); // superbloco ocupa somente o bloco 0, portanto tem tamanho 1

    if(buffer) {
        superbloco = calloc(1, sizeof(SUPERBLOCO));

        memcpy(&superbloco->id, buffer + i, 4); // campo id ocupa 4 bytes
        i+=4;
        memcpy(&superbloco->version, buffer + i, 2); // campo version ocupa 2 bytes
        i+=2;
        memcpy(&superbloco->superblockSize, buffer + i, 2); // campo superblockSize ocupa 2 bytes
        i+=2;
        memcpy(&superbloco->freeBlocksBitmapSize, buffer + i, 2);
        i+=2;
        memcpy(&superbloco->freeInodeBitmapSize, buffer + i, 2);
        i+=2;
        memcpy(&superbloco->inodeAreaSize, buffer + i, 2);
        i+=2;
        memcpy(&superbloco->blockSize, buffer + i, 2);
        i+=2;
        memcpy(&superbloco->diskSize, buffer+i, 4);
        i+=4;

        free(buffer);
        return superbloco;
    }

    return NULL;
}

int nome_correto(char *nome)
{
    if(strlen(nome) > MAX_FILE_NAME_SIZE){ // se tamanho do nome for maior do que o permitido, erro
        return 0;
    }
    else {
        for(int i = 0; i < strlen(nome); i++){
            if(nome[i] < 0x21 || nome[i] > 0x7a){ // se algum caractere do nome n�o estiver entre os caracteres permitidos, erro
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
    if(!init_file_system)
      return -1;

    if(nome_correto(filename)){
        // verificar se n�o existe arquivo com mesmo nome

        return -1;
    }
    else {
        printf("ERRO: Nome incorreto.");
        return -1;
    }

    int i = 0;
    // Verificar se ha espaco entre arquivos abertos
    for (i = 0; i < NUMERO_MAX_ARQUIVOS_ABERTOS; i++)
        if (arquivos_abertos[i] == NULL)
		break;
	// Se nao ha espaco para novos arquivos abertos
	if (i == NUMERO_MAX_ARQUIVOS_ABERTOS)
		return -1;


}

int delete2 (char *filename)
{
    if(!init_file_system)
      return -1;
    return -1;
}

FILE2 open2 (char *filename)
{
    return -1;
}

int close2 (FILE2 handle)
{
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    return -1;
}

int read2 (FILE2 handle, char *buffer, int size)
{
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    return -1;
}

int write2 (FILE2 handle, char *buffer, int size)
{
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    return -1;
}

int truncate2 (FILE2 handle)
{
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    return -1;
}

int seek2 (FILE2 handle, DWORD offset)
{
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    if (offset == -1)
      arquivos_abertos[handle]->cursor = arquivos_abertos[handle]->info.fileSize + 1;
    else
      arquivos_abertos[handle]->cursor = offset;

    return 0;
}

int mkdir2 (char *pathname)
{
    if(superbloco==NULL)
      superbloco = read_superbloco();
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
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    return -1;
}

int closedir2 (DIR2 handle)
{
    if(!init_file_system)
      return -1;

    if (handle < 0 || handle >= NUMERO_MAX_ARQUIVOS_ABERTOS)
      return -2;

    if (arquivos_abertos[handle] == NULL)
      return -3;

    if (arquivos_abertos[handle]->info.fileType != TYPEVAL_REGULAR)
      return -4;

    return -1;
}

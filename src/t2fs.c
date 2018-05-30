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
// Outras funcoes (auxiliares)



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

}

int delete2 (char *filename)
{

}

FILE2 open2 (char *filename)
{

}

int close2 (FILE2 handle)
{

}

int read2 (FILE2 handle, char *buffer, int size)
{

}

int write2 (FILE2 handle, char *buffer, int size)
{

}

int truncate2 (FILE2 handle)
{

}

int seek2 (FILE2 handle, DWORD offset)
{

}

int mkdir2 (char *pathname)
{

}

int rmdir2 (char *pathname)
{

}

int chdir2 (char *pathname)
{

}

int getcwd2 (char *pathname, int size)
{

}

DIR2 opendir2 (char *pathname)
{

}

int readdir2 (DIR2 handle, DIRENT2 *dentry)
{

}

int closedir2 (DIR2 handle)
{

}

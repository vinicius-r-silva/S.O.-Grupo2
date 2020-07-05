#include "../include/interpreter.h"

//Le o proximo comando no arquivo de entrada (fs)
//Salva as informações do comando na variável "acao"
//retorna SUCESS caso consiga ler o comando do arquivo, caso contrario retorna FAILURE
bool read_comando(comando *acao, char* string){
    //verifica se a string esta inicializada
    if(string == nullptr)
        return FAILURE;

    int i = 0;
    int size = strlen(string);

    if(size < 4 || string[0] != 'P')
        return FAILURE;

    int pid;
    sscanf(string + 1, "%d", &pid);

    for(i = 2; i < size && string[i] == ' '; i++);
    if(i == size)
        return FAILURE;

    // i++;
    char action = string[i];

    i++;
    for(; i < size && string[i] != ' '; i++);
    if(i == size)
        return FAILURE;

    i++;
    if(string[i] <'0' || string[i] > '9')
        return FAILURE;

    int arg;
    sscanf(string + i, "%d", &arg);

    acao->pid = pid;
    acao->action = action;
    acao->arg = arg;

    return SUCESS;
}

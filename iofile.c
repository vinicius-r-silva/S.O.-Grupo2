//Para compilar, rode: gcc iofile.c -Wall -o iofile
//Para executar com o arquivo padrão, rode: ./iofile
//Para executar com um arquivo em especifico, rode: ./iofile path/to/file

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/fcntl.h>

#define B_SIZE 1048576  //Lê 1MB do arquivo 
#define fileName "README.md" //Nome do arquivo padrão

int main(int argc, char* argv[]){
    char *buffer = calloc(B_SIZE, sizeof(char));    //Buffer para ler o arquivo
    int file;                                       //Inteiro para receber o 'file descriptor'


    if(argc > 0){                                   //Caso haja argumento na execução, abre o arquivo perdido

        /*
        Syscall open, usada para abrir um arquivo, sendo o primeiro argumento o caminho até o arquivo,
        e o segundo é a flag usada para especificar que o arquivo será aberto apenas para leitura
        Retorna o 'file descriptor' do arquivo
        */
        file = open(argv[1], O_RDONLY);             

    }else{                                          //Caso não haja argumento na execução, abre o arquivo padrão
        file = open(fileName, O_RDONLY);
    }


    if(file < 0){               //Verificação caso ocorra um erro na abertura do arquivo
        perror("file");
        exit(1);
    }

    printf("Arquivo aberto com sucesso\n");

    /*
    Syscall read usada para leitura do arquivo, tendo como argumentos o 'file descriptor' do arquivo,
    o buffer que receberá os caracteres do arquivo, e quantos caracteres devem ser lidos
    Retorna o número de caracteres lidos ou -1 caso tenha ocorrido algum erro.
    */
    int check = read(file, buffer, B_SIZE);

    if(check < 0){          //Verificação de erro da leitura do arquivo
        
        perror("file");
        exit(1);

    }        
    
    /*
    Syscall write usada para escrita em um arquivo, tendo como argumentos o 'file descriptor' do arquivo, no caso é o STDOUT(1),
    o buffer de onde serão tirados os caracteres e quantos caracteres devem ser lidos do buffer
    Retorna o número de caracteres escritos, ou -1 caso ocorra algum erro
    */
    if(write(1, buffer, B_SIZE) < 0){   //Verificação de erro da escrita ao STDOUT
       
        perror("file");
        exit(1);

    }

    /*
    Syscall close utilizada para fechar o arquivo, tem como parametros o 'file descriptor' do arquivo
    Retorna 0 caso o arquivo seja fechado com sucesso, ou -1 caso tenha ocorrido algum erro
    */
    if(close(file) < 0){        //Verificação de erro do arquivo
        
        perror("file");
        exit(1);

    }else{

        printf("\n\nArquivo fechado com sucesso\n");

    }
    
    return 0;
}
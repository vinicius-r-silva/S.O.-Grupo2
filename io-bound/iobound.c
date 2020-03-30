#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define B_SIZE 1048576             //Lê 1MB do arquivo 
#define fileName "../README.md"    //Nome do arquivo padrão
#define outputName "output.txt"        //Nome do arquivo de saída


int main(int argc, char* argv[]){
    char* buffer = (char*) calloc(B_SIZE, sizeof(char));
    int file;
    int output;
    int times = 100000;
    int check;


    if(argc > 1){                                   
        file = open(argv[argc - 1], O_RDONLY);             
    }else{                                          //Caso não haja argumento na execução, abre o arquivo padrão
        file = open(fileName, O_RDONLY);
    }

    //Verificação caso ocorra um erro na abertura do arquivo
    if(file < 0){               
        perror("file");
        exit(EXIT_FAILURE);
    }

    printf("Arquivo que será copiado foi aberto com sucesso\n");

    output = open(outputName, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);

    if(output < 0){
        perror("output");
        exit(EXIT_FAILURE);
    }

    printf("Arquivo que receberá a copia foi aberto com sucesso\n");

    //Loop para as mil execuções

    while(times > 0){
        //Lendo o arquivo que será copiado
        check = read(file, buffer, B_SIZE);

        //Verificação de erro da leitura do arquivo
        if(check < 0){          
            
            perror("file");
            exit(EXIT_FAILURE);

        }       

        //Escrevendo no arquivo que receberá a cópia
        if(write(output, buffer, check) < 0){   //Verificação de erro da escrita ao output
            
            perror("output: ");
            exit(EXIT_FAILURE);

        }

        times--;
    }

    close(file);
    close(output);

    return 0;
}
#include "../include/memory.h"


Memory::Memory(int8_t memType, int32_t size, int32_t pageSize){
    if(size % pageSize != 0){
        Memory();
        return;
    }

    this->memType = memType;
    this->size = size;
    this->pageSize = pageSize;
    this->qtdPages = size / pageSize;

    this->pages = (page*)malloc(qtdPages * sizeof(page));
    for(int i = 0; i < qtdPages; i++){
        this->pages[i].pid = -1;
        this->pages[i].page_id = -1;
    }
    for(int i = 0; i < qtdPages/2; i++){
        this->pages[i].pid = i;
        this->pages[i].page_id = i;
    }
}

Memory::Memory(){
    memType = -1;
    size = -1;
    qtdPages = -1;
    pageSize = -1;
    pages = nullptr;
}

Memory::~Memory(){
    //free(pages);
}

int8_t Memory::get_type(){return memType;}

int8_t Memory::get_size(){return size;}

//convert's a integer to binary in char array
void Memory::to_binary(int a, char *bin, int size){
    int i = 0;
    for(i = 0; i < size; i++){
        bin[size - i - 1] = (a >> i & 1) + '0';
    }
}

//print the memory pages
char* Memory::print(){
    int i = 0; 
    char bin[6]; 
    int pagesPerLine = 3;

    bin[5] = '\0';

    //--------------printing Header--------------//
    printf("Offset         ");
    for(i = 0; i < pagesPerLine; i++){
        to_binary(i, bin, 5);
        printf("%s          ", bin);
    }
    printf("\n");
    
    //Line is the "---..." separation between values
    //its size depends on the quantity of values per line
    char Line[pagesPerLine * 15 + 3];
    memset(Line, '-', pagesPerLine * 15 + 1);
    Line[pagesPerLine * 15 + 1] = '\r';
    Line[pagesPerLine * 15 + 2] = '\0';

    //printing the values
    i = 0;
    while(i < qtdPages){
        //prints the offset
        to_binary(i, bin, 5);
        printf("          %s\n", Line); 
        printf("%s     |", bin);

        //print the line values
        do{
            if(pages[i].pid == -1)  //if this memory address is unoccupied, print a blank space
                printf("              |");
            else                    //Otherwise, print the memory address data
                printf("  P%d page %02d  |", pages[i].pid, pages[i].page_id);

            i++;
        }while(i % pagesPerLine != 0 && i < qtdPages);
        
        if(i < qtdPages)
            printf("\n");
    }

    //If the grid isn't finished, show that the current address don't exists
    while(i % pagesPerLine != 0){
        printf("  Inexistente |");
        i++;
    }
    printf("\n          %s\n", Line);
}
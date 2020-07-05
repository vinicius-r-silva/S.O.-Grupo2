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

    this->pages = (page**)malloc(qtdPages * sizeof(page*));
    for(int i = 0; i < qtdPages; i++){
        this->pages[i] = nullptr;
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

page **Memory::get_pages(){
    return pages;
}

//print the memory pages
void Memory::print(char* str){
    int i = 0; 
    char bin[6]; 
    int pagesPerLine = 3;

    bin[5] = '\0';

    //--------------printing Header--------------//
    sprintf(str, "Offset         ");
    for(i = 0; i < pagesPerLine; i++){
        to_binary(i, bin, 5);
        sprintf(str, "%s%s          ", str, bin);
    }
    sprintf(str, "%s\n", str);
    
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
        sprintf(str, "%s          %s\n", str, Line); 
        sprintf(str, "%s%s     |", str, bin);

        //print the line values
        do{
            if(pages[i] == nullptr || pages[i]->pid == -1)  //if this memory address is unoccupied, print a blank space
                sprintf(str, "%s              |", str);
            else                    //Otherwise, print the memory address data
                sprintf(str, "%s  P%d page %02d  |", str, pages[i]->pid, pages[i]->page_id);

            i++;
        }while(i % pagesPerLine != 0 && i < qtdPages);
        
        if(i < qtdPages)
            sprintf(str, "%s\n", str);
    }

    //If the grid isn't finished, show that the current address don't exists
    while(i % pagesPerLine != 0){
        sprintf(str, "%s  Inexistente |", str);
        i++;
    }
    sprintf(str, "%s\n          %s\n", str, Line);
}
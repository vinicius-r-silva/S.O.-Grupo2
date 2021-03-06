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

int32_t Memory::get_size(){return size;}

page **Memory::get_pages(){
    return pages;
}

void Memory::nullify(){
    for(int i = 0; i < qtdPages; i++){
        this->pages[i] = nullptr;
    }
}

void Memory::clear_all(){
    for(int i = 0; i < qtdPages; i++){
        if(pages[i] == nullptr)
            continue;
            
        free(pages[i]);
        pages[i] = nullptr;
    }
}

//print the memory pages
std::string Memory::print(){
    std::string str;

    int pagesPerLine = 3;
    int i = 0;

    char bin[6];

    bin[5] = '\0';

    str.append("Offset         ");

    //--------------printing Header--------------//

    for(i = 0; i < pagesPerLine; i++){
        to_binary(i, bin, 5);
        str.append(bin);
        str.append("          ");
    }
    str.append("\n");
    
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
        str.append("          ");
        str.append(Line);
        str.append("\n");
        str.append(bin);
        str.append("     |");

        //print the line values
        do{
            if(pages[i] == nullptr || pages[i]->pid == -1)  //if this memory address is unoccupied, print a blank space
                str.append("              |");

            else{                    //Otherwise, print the memory address data
                char buff[20];
                sprintf(buff, "  P%d page %02d  |", pages[i]->pid, pages[i]->page_id);
                str.append(buff);
            }

            i++;
        }while(i % pagesPerLine != 0 && i < qtdPages);
        
        if(i < qtdPages)
            str.append("\n");
    }

    //If the grid isn't finished, show that the current address don't exists
    while(i % pagesPerLine != 0){
        str.append("  ----------  |");
        i++;
    }
    str.append("\n          ");
    str.append(Line);
    str.append("\n");

    return str;
}


int32_t Memory::get_qtdPages(){
    return qtdPages;
}

void Memory::updateSizes(int32_t size, int32_t pageSize){
    clear_all();

    this->size = size;
    this->pageSize = pageSize;
    this->qtdPages = size / pageSize;

    free(this->pages);

    this->pages = (page**)malloc(qtdPages * sizeof(page*));
    for(int i = 0; i < qtdPages; i++){
        this->pages[i] = nullptr;
    }

}

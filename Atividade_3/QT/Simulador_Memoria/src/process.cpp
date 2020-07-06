#include "../include/process.h"

Process::Process(int id, int size, int pageSize){
    int i = 0;
    this->id = id;
    this->size = size;
    this->qtdPages = (size + pageSize - 1) / pageSize;

    map = (page_map*)malloc(this->qtdPages * sizeof(page_map));
    for(i = 0; i < this->qtdPages; i++){
        map[i].logical = i;
        map[i].physical = -1;
        map[i].ref = nullptr;
    }

    this->pagesAtRam = 0;
    this->pagesAtDisk = 0;
}

void Process::set_pages(page *pages){
    int i = 0;
    page *curr = pages;
    page *prev = nullptr;

    for(i = 0; i < qtdPages; i++){
        if(curr != nullptr){
            map[i].logical = i;
            map[i].physical = curr->page_id;

            prev = curr;
            curr = curr->next_lru;
            free(prev);
        }
        else {
            map[i].logical = i;
            map[i].physical = -1;
        }
    }
}

void Process::update_map_entry(int logical, int physical){
    map[logical].physical = physical;
}

void Process::update_map_entry(int logical, int physical, page* ref){
    map[logical].physical = physical;
    map[logical].ref = ref;
}

int Process::get_logical(int physical){
    int i = 0;
    for(i = 0; i < qtdPages && map[i].physical != physical; i++);
    if(i == qtdPages)
        return -1;

    return i;
}

void Process::replace_physical(int src, int dst){
    int logical = get_logical(src);
    map[logical].physical = dst;
}

std::string Process::print(int replacement){
    std::string str;
    int pagesPerLine = 3;
    int i;

    //----------------------- printing Header ---------------------/

    str.append("###########################################################\n");
    str.append("                        PROCESSO ");
    str.append(std::to_string(id));
    str.append("\n\n");
    str.append("Offset");

    for(i = 0; i < pagesPerLine; i++){
        char buff[20];
        sprintf(buff, "        %02d       ", i);
        str.append(buff);
    }
    str.append("\n");

    //Line is the "---..." separation between values
    //its size depends on the quantity of values per line
    char Line[pagesPerLine * 17 + 3];
    memset(Line, '-', pagesPerLine * 17 + 1);
    Line[pagesPerLine * 17 + 1] = '\r';
    Line[pagesPerLine * 17 + 2] = '\0';

    //printing the values
    i = 0;
    while(i < qtdPages){
        //prints the offset
        char buff[100];
        sprintf(buff, "      %s\n  %02d  |", Line, i);
        str.append(buff);

        //print the line values
        do{
            if(map[i].physical < 0){
                str.append("     DISCO      |");
            }else{
                int repPag = (replacement == LRU)? map[i].ref->lru_order : map[i].ref->references;
                char buff2[20];
                //sprintf(buff2, " P:%02d L:%02d R:%02d |", map[i].physical, );
                sprintf(buff2, "    %02d   (%02d)   |", map[i].physical, repPag);
                str.append(buff2);
            }

            i++;
        }while(i % pagesPerLine != 0 && i < qtdPages);

        if(i < qtdPages)
            str.append("\n");
    }

    //If the grid isn't finished, show that the current address don't exists
    while(i % pagesPerLine != 0){
        str.append("  ------------  |");
        i++;
    }

    str.append("\n      ");
    str.append(Line);
    str.append("\n\n");

    return str;
}

// Process::Process(){
//     id = -1;
//     map = nullptr;
// }

Process::~Process(){
    //free(map);
}


page_map* Process::get_map(){
    return map;
}

int Process::get_qtdPages(){
    return qtdPages;
}

int Process::get_Id(){
    return id;
}


int Process::get_size(){
    return size;
}

int Process::get_pagesAtRam(){
    return pagesAtRam;
}

void Process::set_pagesAtRam(int pagesAtRam){
    this->pagesAtRam = pagesAtRam;
}

int Process::get_pagesAtDisk(){
    return pagesAtDisk;
}

void Process::set_pagesAtDisk(int pagesAtDisk){
    this->pagesAtDisk = pagesAtDisk;
}

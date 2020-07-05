#include "../include/process.h"

Process::Process(int id, int size, int pageSize){
    int i = 0;
    this->id = id;
    this->qtdPages = (size + pageSize - 1) / pageSize;

    map = (page_map*)malloc(this->qtdPages * sizeof(page_map));
    for(i = 0; i < this->qtdPages; i++){
        map[i].logical = -1;
        map[i].physical = -1;
    }
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
    page_map *curr = map;
    map[logical].physical = physical;
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

char* Process::print(){
    int i = 0;
    printf("\npid: %d\n", id);
    for(i = 0; i < qtdPages; i++){
        printf("logical %d, physical: %d\n", map[i].logical, map[i].physical);
    }
    printf("\n");
}

Process::Process(){
    id = -1;
    map = nullptr;
}

Process::~Process(){
    //free(map);
}


page_map* Process::get_map(){
    return map;
}
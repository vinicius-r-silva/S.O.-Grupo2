#include "../include/process.h"

Process::Process(int id, int size, int pageSize){
    int i = 0;
    this->id = id;
    int qtdPages = (size + pageSize - 1) / pageSize;

    page *currPage;
    page *prevPage = nullptr;
    for(i = 0; i < qtdPages; i++){
        currPage = (page*)malloc(sizeof(page));
        currPage->pid = id;
        currPage->page_id = i;

        currPage->prev = prevPage;
        currPage->next = nullptr;
        prevPage->next = currPage;

        prevPage = currPage;
    }
}


Process::Process(){
    id = -1;
    pages = nullptr;
}

Process::~Process(){
    page *currPage = pages;
    page *prevPage = nullptr;
    while(currPage != nullptr){
        prevPage = currPage;
        currPage = currPage->next;

        //delete(prevPage);
    }
}


page* Process::get_pages(){
    return pages;
}
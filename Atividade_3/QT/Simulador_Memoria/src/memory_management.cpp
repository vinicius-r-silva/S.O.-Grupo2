#include "../include/memory_management.h"

MemoryManagement::MemoryManagement(int ramSize, int diskSize, int pageSize){
    ram = new Memory(RAM, ramSize, pageSize);
    disk = new Memory(DISK, diskSize, pageSize);

    this->ramSize = ramSize;
    this->diskSize = diskSize;
    this->pageSize = pageSize;

    this->ramPagesCount = ramSize / pageSize;
    this->diskPagesCount = diskSize / pageSize;

    this->ramAvailable = ramSize;
    this->diskAvailable = diskSize;

    this->processesAtRam = 0;
    this->processesAtDisk = 0;
 
    processes = nullptr;
    lruBegin = nullptr;
    waiting_processes = nullptr;

    warning = (char*)calloc(1000, sizeof(char));
    sprintf(warning, "no action executed");
}

MemoryManagement::~MemoryManagement(){
    //free(warning);
}

void MemoryManagement::add_wating_process(int pid, int size){
    waiting_process *novo = (waiting_process*)malloc(sizeof(waiting_process));
    novo->id = pid;
    novo->next = nullptr;
    novo->prev = nullptr;
    novo->size = size;

    waiting_process *curr = waiting_processes;
    waiting_process *prev = nullptr;

    while (curr != nullptr){
        prev = curr;
        curr = curr->next;
    }

    if(prev == nullptr)
        waiting_processes = novo;
    else{
        prev->next = novo;
        novo->prev = prev;
    }
}

void MemoryManagement::create_process(int id, int size){
    int i = 0;
    int totalMemory = ramSize + diskSize;
    int totalFree = ramAvailable + diskAvailable;

    if(size > totalMemory){
        sprintf(warning, "Nao ha memoria suficiente para criar o processo");
        return;
    }

    if(size > totalFree){
        sprintf(warning, "Nao ha memoria livre suficiente para criar o processo, processo em espera");
        add_wating_process(id, size);
        return;
    }

    Process *process = new Process(id, size, pageSize);
    process_list *novo = (process_list*)malloc(sizeof(process_list));
    novo->process = process;
    novo->next = nullptr;
    novo->prev = nullptr;

    if(processes == nullptr)
        processes = novo;
    else{
        novo->next = processes;
        processes->prev = novo;

        processes = novo;    
    }

    int result = 0;
    int qtdPages = process->get_qtdPages();
    for(i = 0; i < qtdPages; i++){
        page *new_page = (page*)malloc(sizeof(page));
        new_page->pid = id;
        new_page->page_id = i;
        new_page->references = 0;
        new_page->next_lru = nullptr;
        new_page->prev_lru = nullptr;

        result = add_page_ram(new_page);
        process->update_map_entry(i, result);

        if(result == -1){
            //TODO: add at disk
        }
    }
    sprintf(warning, "Processo %d, de tamanho %d, foi criado com sucesso", id, size);
    
}

int MemoryManagement::add_page_ram(page *new_page){
    int i = 0;
    page novo;
    page **ramPages = ram->get_pages(); 
    if(ramAvailable > 0){
        for(i = 0; i < ramPagesCount; i++){
            if(ramPages[i] == nullptr){
                ramPages[i] = new_page;

                //TODO: insert on lru
                break;
            }
        }
    }
    else{
        for(i = 0; i < ramPagesCount; i++){
            if(ramPages[i]->references == 0){
                //TODO: remove from lru
                //TODO: move do disk

                    free(ramPages[i]);
                ramPages[i] = new_page;

                //TODO: insert on lru
                break;
            }
        }
    }
    if(i == ramPagesCount) return -1;
    return i;
}

void insert_page_disk(page* new_page){
    
}

void MemoryManagement::get_ram(char *str){
    ram->print(str);
}

void MemoryManagement::get_disk(char *str){
    disk->print(str);
}

char* MemoryManagement::get_warning(){
    return warning;
}

void MemoryManagement::clean_all(){
    process_list *currPL = processes;
    process_list *prevPL = nullptr;

    ram->clear_all();
    disk->clear_all();
    while(currPL != nullptr){
        delete(currPL->process);
        prevPL = currPL;
        currPL = currPL->next;
        free(prevPL);
    }

    waiting_process *currWP = waiting_processes;
    waiting_process *prevWP = nullptr;
    while(currWP != nullptr){
        prevWP = currWP;
        currWP = currWP->next;
        free(prevWP);
    }

    processes = nullptr;
    lruBegin = nullptr;
    waiting_processes = nullptr;

    sprintf(warning, " ");
}
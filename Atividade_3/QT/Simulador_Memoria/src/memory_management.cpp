#include "../include/memory_management.h"

MemoryManagement::MemoryManagement(int ramSize, int diskSize, int pageSize){
    ram = new Memory(RAM, ramSize, pageSize);
    disk = new Memory(DISK, diskSize, pageSize);

    this->ramSize = ramSize;
    this->diskSize = diskSize;

    this->ramPagesCount = ramSize / pageSize;
    this->diskPagesCount = diskSize / pageSize;

    this->ramAvailable = ramSize;
    this->diskAvailable = diskSize;

    this->processesAtRam = 0;
    this->processesAtDisk = 0;
 
    processes = nullptr;
    lruBegin = nullptr;

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
}

int MemoryManagement::addPage(int pid, int page_id){
    int i = 0;
    page novo;
    page **ramPages = ram->get_pages(); 
    if(ramAvailable > 0){
        for(i = 0; i < ramPagesCount; i++){
            if(ramPages[i]->pid == -1){
                ramPages[i]->pid = -1;
                ramPages[i]->page_id = page_id;

                //insert on lru
                break;
            }
        }
    }
    else{
        for(i = 0; i < ramPagesCount; i++){
            if(ramPages[i]->references == 0){
                //remove from lru
                //move do disk

                ramPages[i]->pid = -1;
                ramPages[i]->page_id = page_id;

                //insert on lru
                break;
            }
        }
    }
    if(i == ramPagesCount) return -1;
    return i;
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

#include "../include/memory_management.h"

MemoryManagement::MemoryManagement(int ramSize, int diskSize, int pageSize){
    ram = new Memory(RAM, ramSize, pageSize);
    disk = new Memory(DISK, diskSize, pageSize);

    this->ramSize = ramSize;
    this->diskSize = diskSize;

    this->ramAvailable = ramSize;
    this->diskAvailable = diskSize;

    this->processesAtRam = 0;
    this->processesAtDisk = 0;
 
    processes = nullptr;

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

char* MemoryManagement::get_ram(){
    return ram->print();
}

char* MemoryManagement::get_disk(){
    return disk->print();
}

char* MemoryManagement::get_warning(){
    return warning;
}

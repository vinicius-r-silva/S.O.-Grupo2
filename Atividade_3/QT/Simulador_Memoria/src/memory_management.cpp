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
    lruEnd = nullptr;
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
        process_list *ptr = processes;

        while(ptr->next != nullptr && novo->process->get_Id() < ptr->process->get_Id())
            ptr = ptr->next;

        if(ptr->next == nullptr && novo->process->get_Id() < ptr->process->get_Id()){
            ptr->next = novo;
            novo->prev = ptr;
            novo->next = nullptr;
        }else if(ptr->next == nullptr){
            novo->next = ptr;
            novo->prev = ptr->prev;
            ptr->prev = novo;
            ptr->next = nullptr;
        }else{
            novo->prev = ptr;
            novo->next = ptr->next;
            novo->next->prev = novo;
            ptr->next = novo;
        }
    }

    int result = 0;
    int qtdPages = process->get_qtdPages();
    for(i = 0; i < qtdPages; i++){
        page *new_page = (page*)malloc(sizeof(page));
        new_page->pid = id;
        new_page->page_id = i;
        new_page->references = 0;
        new_page->page_physical = -1;
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
                ramAvailable-= pageSize;
                new_page->page_physical = i;

                if(lruBegin == nullptr){
                    lruBegin = new_page;
                    lruEnd = new_page;
                }else{
                    new_page->next_lru = lruBegin;
                    lruBegin->prev_lru = new_page;
                    lruBegin = new_page;
                }

                break;
            }
        }
    }
    else{
        for(i = 0; i < ramPagesCount; i++){
        
            page* page2remove = lruEnd;
            lruEnd->prev_lru->next_lru = nullptr;
            lruEnd = page2remove->prev_lru;

            page2remove->next_lru = nullptr;
            page2remove->prev_lru = nullptr;

            new_page->next_lru = lruBegin;
            lruBegin->prev_lru = new_page;
            lruBegin = new_page;

            if(insert_page_disk(page2remove) == FAILURE)
                return -1;
            
            diskAvailable-=pageSize;

            ramPages[page2remove->page_physical] = new_page;
            new_page->page_physical = page2remove->page_physical;
            page2remove->page_physical = -1;

            break;
        }
    }
    if(i == ramPagesCount) return -1;
    return i;
}

int MemoryManagement::insert_page_disk(page* new_page){
    int i = 0;
    page novo;
    page **diskPages = disk->get_pages(); 
    if(diskAvailable > 0){
        for(i = 0; i < diskPagesCount; i++){
            if(diskPages[i] == nullptr){
                diskPages[i] = new_page;
                break;
            }
        }
    }
    else
        return FAILURE;

    if(i == diskPagesCount)
        return FAILURE;

    return SUCESS;    
}

page* MemoryManagement::remove_page_disk(int pid, int page_id){
    
}

std::string MemoryManagement::get_ram(){
    return ram->print();
}

std::string MemoryManagement::get_disk(){
    return disk->print();
}

std::string MemoryManagement::get_proTable(){
    std::string text;

    for(process_list* ptr = processes; ptr->next != nullptr; ptr = ptr->next){
        text.append(ptr->process->print());

    }

    return text;
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
    lruEnd = nullptr;
    waiting_processes = nullptr;

    ramAvailable = ramSize;
    diskAvailable = diskSize;

    sprintf(warning, " ");
}

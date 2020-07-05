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

waiting_process* MemoryManagement::search_waiting_process(int id){
    waiting_process* curr = waiting_processes;

    while(curr != nullptr && id != curr->id)
        curr = curr->next;

    return curr;
}

process_list* MemoryManagement::search_active_process(int id){
    process_list* curr = processes;

    while(curr != nullptr && id != curr->process->get_Id())
        curr = curr->next;

    return curr;
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

   if(search_active_process(id) != nullptr){
       sprintf(warning, "Processo com ID %d ja existe", id);
       return;
   }

   if(search_waiting_process(id) != nullptr){
       sprintf(warning, "Processo com ID %d ja existe, e esta na lista de espera", id);
       return;
   }

    Process *process = new Process(id, size, pageSize);
    process_list *novo = (process_list*)malloc(sizeof(process_list));
    novo->process = process;
    novo->next = nullptr;
    novo->prev = nullptr;

    process_list *ptr = processes;
    process_list *prev = nullptr;

    while(ptr != nullptr && novo->process->get_Id() > ptr->process->get_Id()){
        prev = ptr;
        ptr = ptr->next;
    }

    if(prev == nullptr){
        novo->next = processes;
        processes = novo;
    }else{
        novo->next = ptr;
        novo->prev = prev;
        prev->next = novo;

        if(ptr != nullptr){
            ptr->prev = novo;
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

void MemoryManagement::kill_process(int id){
    id++;
}

int MemoryManagement::get_ordem_lru(int pid, int page_id){
    int i = 0;
    page *curr = lruBegin;
    while(curr != nullptr && (curr->pid != pid || curr->page_id != page_id)){
        curr = curr->next_lru;
        i++;
    }
    return i;
}

int MemoryManagement::add_page_ram(page *new_page){
    int i = 0;
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
    pid = pid + page_id;
    return nullptr;
}

std::string MemoryManagement::get_ram(){
    return ram->print();
}

std::string MemoryManagement::get_disk(){
    return disk->print();
}

std::string MemoryManagement::get_proTable(){
    std::string text;

    if(processes == nullptr)
        return text;

    process_list* ptr = processes;

    while(ptr != nullptr){
        text.append(ptr->process->print());
        ptr = ptr->next;
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

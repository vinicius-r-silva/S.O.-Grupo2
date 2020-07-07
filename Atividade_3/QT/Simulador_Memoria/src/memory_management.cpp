#include "../include/memory_management.h"

MemoryManagement::MemoryManagement(int ramSize, int diskSize, int pageSize, int replacement){
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

    this->replacement = replacement;
    this->qtdPageFaults = 0;
 
    processes = nullptr;
    lruBegin = nullptr;
    lruEnd = nullptr;
    waiting_processes = nullptr;

    relPos = 0;

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
        new_page->references = 1;
        new_page->page_physical = -1;
        new_page->next_lru = nullptr;
        new_page->prev_lru = nullptr;

        result = add_page_ram(new_page);
        process->update_map_entry(i, result, new_page);

        if(result == -1){
            sprintf(warning, "Erro ao criar processo %d, de tamanho %d", id, size);
            kill_process(id);
            return;
        }
    }

    int qtdPagesRam = ram->get_qtdPages();
    if(qtdPages > qtdPagesRam){
        process->set_pagesAtRam(qtdPagesRam);
        process->set_pagesAtDisk(qtdPages - qtdPagesRam);
        processesAtDisk++;
    }
    else{
        process->set_pagesAtRam(qtdPages);
        process->set_pagesAtDisk(0);
    }
    processesAtRam++;

    sprintf(warning, "Processo %d, de tamanho %d, foi criado com sucesso", id, size);    
}

void MemoryManagement::activate_waiting_processes(){
    waiting_process *curr = waiting_processes;
    while(curr != nullptr){
        if(curr->size <= (ramAvailable + diskAvailable)){
            create_process(curr->id, curr->size);
            sprintf(warning, "%s\nProcesso %d de tamanho: %d, foi removido da lista de espera e ativado na memoria", warning, curr->id, curr->size);
        }

        curr = curr->next;
    }
}

void MemoryManagement::kill_process(int id){
    process_list *pl = nullptr;
    pl = search_active_process(id);
    if(pl != nullptr){
        if(pl->prev == nullptr){
            processes = pl->next;
        } else {
            pl->prev->next = pl->next;
        }

        if(pl->process->get_pagesAtRam() > 0)
            processesAtRam--;
        if(pl->process->get_pagesAtDisk() > 0)
            processesAtDisk--;

        int i = 0;
        page *pageRemoved = nullptr;
        page_map *map = pl->process->get_map();
        int qtdPages = pl->process->get_qtdPages();
        for(i = 0; i < qtdPages; i++){
            if(map[i].physical >= 0)
                pageRemoved = remove_page_ram(map[i].ref);
            else
                pageRemoved = remove_page_disk(id, i);

            free(pageRemoved);
        }
        sprintf(warning, "Processo com ID %d foi removido", id);
        pl->process->~Process();

        activate_waiting_processes();
        return;
    }

    waiting_process *wp = nullptr;
    wp = search_waiting_process(id);
    if(wp != nullptr){
        if(wp->prev == nullptr)
            waiting_processes = wp->next;
        else
            wp->prev->next = wp->next;

        free(wp);

        sprintf(warning, "Processo com ID %d, que estava na lista de espera, foi removido", id);
        return;
    }

    sprintf(warning, "Processo com ID %d nao existe", id);
    return;
}

void MemoryManagement::update_lru_order(){
    int i = 0;
    page *curr = lruBegin;
    while(curr != nullptr){
        curr->lru_order = i;
        curr = curr->next_lru;
        i++;
    }
}

void MemoryManagement::debug(){
    page* curr = lruBegin;
    printf("\n");
    while (curr != nullptr){
        printf(" (%d, %d) -> ", curr->pid, curr->page_id);
        curr = curr->next_lru;
    }
    printf("\n");
    
}

int MemoryManagement::add_page_ram(page *new_page){
    int i = 0;
    int physical_address = 0;
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
        physical_address = i;
    }
    else{
        page* page2remove = remove_page_ram(get_page2remove());
        process_list *pl = search_active_process(page2remove->pid);
        pl->process->update_map_entry(page2remove->page_id, -1);

        physical_address = page2remove->page_physical;

        new_page->next_lru = lruBegin;
        lruBegin->prev_lru = new_page;
        lruBegin = new_page;

        if(insert_page_disk(page2remove) == FAILURE)
            return -1;
        
        diskAvailable-=pageSize;

        ramPages[page2remove->page_physical] = new_page;
        new_page->page_physical = page2remove->page_physical;
        page2remove->page_physical = -1;
        page2remove->references = 0;
        ramAvailable -= pageSize;
    }

    debug();
    if(i == ramPagesCount) return -1;
    return physical_address;
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
    page **diskPages = disk->get_pages();
    int i = 0;
    page *curr = nullptr;
    int qtdPages = disk->get_qtdPages();
    for(i = 0; i < qtdPages; i++){
        curr = diskPages[i];
        if(curr != nullptr && (curr->pid == pid && curr->page_id == page_id))
            break;
    }

    if(curr == nullptr)
        return nullptr;

    diskPages[i] = nullptr;
    diskAvailable += pageSize;
    return curr;
}

page* MemoryManagement::get_page2remove(){
    if(replacement == LRU)
        return lruEnd;
    
    page **ramPages = ram->get_pages();
    while(true){
        if(ramPages[relPos]->references == 0)
            return ramPages[relPos];
        
        ramPages[relPos]->references = 0;
        relPos++;
        if(relPos >= ramPagesCount)
            relPos = 0;
    }
}

page* MemoryManagement::remove_page_ram(page* page2remove){
    if(page2remove->page_physical < 0)
        return nullptr;

    if(page2remove == lruBegin){
        lruBegin = page2remove->next_lru;
    }
    if(page2remove == lruEnd){
        lruEnd->next_lru = nullptr;
        lruEnd = page2remove->prev_lru;
    }
    if(page2remove->prev_lru != nullptr)
        page2remove->prev_lru->next_lru = page2remove->next_lru;
    
    page2remove->next_lru = nullptr;
    page2remove->prev_lru = nullptr;

    page **ramPages = ram->get_pages();
    ramPages[page2remove->page_physical] = nullptr;

    ramAvailable += pageSize;
    return page2remove;
}

page* MemoryManagement::remove_page_ram(int physical){
    page **ramPages = ram->get_pages(); 
    page *page2remove = ramPages[physical];
    return remove_page_ram(page2remove);
}

std::string MemoryManagement::get_ram(){
    return ram->print();
}

std::string MemoryManagement::get_disk(){
    return disk->print();
}

std::string MemoryManagement::get_proTable(){
    std::string text;
    update_lru_order();

    if(processes == nullptr)
        return text;

    process_list* ptr = processes;

    while(ptr != nullptr){
        text.append(ptr->process->print(replacement));
        ptr = ptr->next;
    }

    return text;
}

void MemoryManagement::move_to_begin_lru(int page_id, int pid){    
    page *curr = lruBegin;
    while (curr != nullptr && (curr->page_id != page_id || curr->pid != pid)){
        curr = curr->next_lru;
    }
    if(curr == nullptr || curr->prev_lru == nullptr)
        return;

    curr->prev_lru->next_lru = curr->next_lru;
    if(curr->next_lru != nullptr)
        curr->next_lru->prev_lru = curr->prev_lru;

    curr->prev_lru = nullptr;
    curr->next_lru = lruBegin;
    lruBegin->prev_lru = curr;
    lruBegin = curr;
}

void MemoryManagement::acesso_memoria(int pid, int byte,const char* acao){
    int Logical = byte / pageSize;
    process_list *pl = search_active_process(pid);
    if(pl != nullptr){
        if(byte > pl->process->get_size()){
            sprintf(warning, "erro na %s, o processo %d tem tamanho %d, e o endereco acessado e %d", acao, pid, pl->process->get_size(), byte);
            return;
        }

        page_map *map = pl->process->get_map();
        if(map[Logical].physical >= 0){
            sprintf(warning, "%s feita Sucesso.\nO endereço lógico %d do processo %d se encontra na página lógica %d, que por sua vez se encontra na página física %d", acao, byte, pid, Logical, map[Logical].physical);
            map[Logical].ref->references = 1;
            move_to_begin_lru(Logical, pid);
            return;
        }

        page* wantedPage = remove_page_disk(pid, Logical);
        if(wantedPage == nullptr){
            sprintf(warning, "Page nor found");
            return;
        }

        wantedPage->references = 1;
        int physical = add_page_ram(wantedPage);
        pl->process->update_map_entry(Logical, physical);
        sprintf(warning, "Page fault\nO endereço lógico %d do processo %d se encontra na página lógica %d\nA página se encontrava no disco e foi movida para a página lógica %d da ram\n", byte, pid, Logical, physical);
        qtdPageFaults++;    
        return;
    }
    
    waiting_process *wp = search_waiting_process(pid);
    if(wp != nullptr)
        sprintf(warning, "Processo %d na lista de espera, sem memoria livre suficiente para ativa-lo", pid);
    else
        sprintf(warning, "Processo %d nao encontrado", pid);
}


void MemoryManagement::operacao(int pid,const char* acao){
    process_list *pl = search_active_process(pid);
    if(pl != nullptr){
        sprintf(warning, "operacao de %s pelo processo %d feita com sucesso\n", acao, pid);
        return;
    }

    waiting_process *wp = search_waiting_process(pid);
    if(wp != nullptr)
        sprintf(warning, "Processo %d na lista de espera, sem memoria livre suficiente para ativa-lo", pid);
    else
        sprintf(warning, "Processo %d nao encontrado", pid);
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
    qtdPageFaults = 0;

    sprintf(warning, " ");
}

int MemoryManagement::get_processes_at_ram(){
    return processesAtRam;
}

int MemoryManagement::get_processes_at_disk(){
    return processesAtDisk;
}

int MemoryManagement::getRamAv(){
    return ramAvailable;
}
int MemoryManagement::getDiskAv(){
    return diskAvailable;
}

void MemoryManagement::set_page_replacement(int replacement){
    this->replacement = replacement;
}

int MemoryManagement::get_page_replacement(){
    return replacement;
}

void MemoryManagement::updateSizes(int ramSize, int diskSize, int pageSize){
    this->ramSize = ramSize;
    this->diskSize = diskSize;
    this->pageSize = pageSize;

    ramPagesCount = ramSize / pageSize;
    diskPagesCount = diskSize / pageSize;

    ramAvailable = ramSize;
    diskAvailable = diskSize;

    ram->updateSizes(ramSize, pageSize);
    disk->updateSizes(diskSize, pageSize);
}

int MemoryManagement::getQtdPageFaults(){
    return qtdPageFaults;
}
#ifndef MANAGER_H
#define MANAGER_H

#include "memory.h"
#include "defines.h"
#include "process.h"

struct process_list{
    Process *process;
    process_list *next;
    process_list *prev;
};

struct waiting_process{
    int id;
    int size;

    waiting_process *next;
    waiting_process *prev;
};

class MemoryManagement {
private:
    char* warning;
    int ramSize;
    int diskSize;
    int pageSize;

    int ramPagesCount;
    int diskPagesCount;

    int ramAvailable;
    int diskAvailable;

    int processesAtRam;
    int processesAtDisk;
    int processesAtBoth;

    Memory *ram;
    Memory *disk;

    page *lruBegin;
    page *lruEnd;

    process_list *processes;
    waiting_process *waiting_processes;


    int add_page_ram(page *new_page);
    page* remove_page_ram(int physical);
    page* remove_page_ram(page* page2remove);

    int insert_page_disk(page* new_page);
    page* remove_page_disk(int pid, int page_id);

    void add_wating_process(int pid, int size);
    void activate_waiting_processes();
    
    void update_lru_order();

    process_list* search_active_process(int id);
    waiting_process* search_waiting_process(int id);

    void move_to_begin_lru(int page_id, int pid);

public:
    MemoryManagement(int ramSize, int diskSize, int pageSize);
    ~MemoryManagement();

    std::string get_ram();
    std::string get_disk();
    std::string get_proTable();
    char* get_warning();

    void clean_all();

    void create_process(int id, int size);
    void kill_process(int id);

    void acesso_memoria(int pid, int byte, const char* acao);
    void operacao(int pid,const char* acao);

    int get_processes_at_ram();
    int get_processes_at_disk();
};


#endif //MANAGER_H

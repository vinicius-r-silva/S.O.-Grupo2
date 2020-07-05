#ifndef MANAGER_H
#define MANAGER_H

#include "memory.h"
#include "defines.h"
#include "process.h"

struct process_list{
    Process process;
    process_list *next;
    process_list *prev;
};

struct waiting_process{
    int id;
    int size;

    waiting_process *next;
    waiting_process *prev;
};

class MemoryManagement
{
private:
    char* warning;
    int ramSize;
    int diskSize;

    int ramAvailable;
    int diskAvailable;

    int processesAtRam;
    int processesAtDisk;

    Memory *ram;
    Memory *disk;

    process_list *processes;
    waiting_process *waiting_processes;


    void add_wating_process(int pid, int size);

public:
    MemoryManagement(int ramSize, int diskSize, int pageSize);
    ~MemoryManagement();

    char* get_ram();
    char* get_disk();
    char* get_warning();

    void create_process(int id, int size);
};


#endif //MANAGER_H
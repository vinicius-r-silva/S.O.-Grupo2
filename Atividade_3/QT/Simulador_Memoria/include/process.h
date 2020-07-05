#ifndef PROCESS_H
#define PROCESS_H

#include "stdlib.h"
#include "stdio.h"
#include "defines.h"

class Process {
private:
    int id;
    int qtdPages;
    page_map *map;

public:
    Process(int id, int size, int pageSize);
    Process();
    ~Process();

    page_map *get_map();
    void set_pages(page *pages);
    void update_map_entry(int logical, int physical);

    int get_logical(int physical);
    void replace_physical(int src, int dst);

    char* print();
};

#endif //PROCESS_H

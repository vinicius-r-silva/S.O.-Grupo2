#ifndef PROCESS_H
#define PROCESS_H

#include "stdlib.h"
#include "stdio.h"
#include "defines.h"
#include "interpreter.h"
#include <iostream>

class Process {
private:
    int id;
    int qtdPages;
    int size;
    page_map *map;

    int pagesAtRam;
    int pagesAtDisk;

public:
    Process(int id, int size, int pageSize);
    // Process();
    ~Process();

    page_map *get_map();
    void set_pages(page *pages);
    void update_map_entry(int logical, int physical);

    int get_logical(int physical);
    void replace_physical(int src, int dst);

    int get_qtdPages();

    int get_Id();
    int get_size();

    int get_pagesAtRam();
    void set_pagesAtRam(int pagesAtRam);
    int get_pagesAtDisk();
    void set_pagesAtDisk(int pagesAtDisk);

    std::string print();
};

#endif //PROCESS_H

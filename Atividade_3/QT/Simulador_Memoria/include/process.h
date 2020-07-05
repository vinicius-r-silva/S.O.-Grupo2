#ifndef PROCESS_H
#define PROCESS_H

#include "stdlib.h"
#include "defines.h"

class Process
{
private:
    int id;
    page *pages;

public:
    Process(int id, int size, int pageSize);
    Process();
    ~Process();

    page *get_pages();
};

#endif //PROCESS_H
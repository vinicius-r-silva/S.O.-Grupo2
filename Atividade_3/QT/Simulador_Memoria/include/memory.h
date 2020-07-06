#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <iostream>

#include "defines.h"
#include "interpreter.h"

class Memory{
private:

    page **pages;

    int8_t memType;
    int32_t size;

    int32_t pageSize;
    int32_t qtdPages;

public:
    Memory(int8_t memType, int32_t size, int32_t pageSize);
    Memory();
    ~Memory();

    int8_t get_type();
    int32_t get_size();
    int32_t get_qtdPages();

    page **get_pages();

    std::string print();

    void nullify();
    void clear_all();

    void updateSizes(int32_t size, int32_t pageSize);

};

#endif //MEMORY_H

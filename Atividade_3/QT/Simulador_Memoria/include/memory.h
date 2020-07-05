#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

#include "defines.h"

class Memory{
private:

    page **pages;

    int8_t memType;
    int32_t size;

    int32_t pageSize;
    int32_t qtdPages;
    void to_binary(int a, char *bin, int size);

public:
    Memory(int8_t memType, int32_t size, int32_t pageSize);
    Memory();
    ~Memory();

    int8_t get_type();
    int8_t get_size();

    char* print();
};

#endif //MEMORY_H
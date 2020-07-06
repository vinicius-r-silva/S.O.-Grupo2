#ifndef DEFINES_H
#define DEFINES_H

#define FAILURE 0
#define SUCESS 1

#define RAM 0
#define DISK 1

#define LRU 0
#define RELOGIO 1

struct comando{
    int pid;
    char action;
    int arg;
};

struct page{
    int pid;
    int page_id;
    int page_physical;

    int references;
    int lru_order;

    page *next_lru;
    page *prev_lru;
};

struct page_map{
    int logical;
    int physical;
    page *ref;
};

#endif //DEFINES_H
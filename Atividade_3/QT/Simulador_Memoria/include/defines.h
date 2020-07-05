#ifndef DEFINES_H
#define DEFINES_H

#define FAILURE 0
#define SUCESS 1

#define RAM 0
#define DISK 1

struct page{
    int pid;
    int page_id;

    page *next;
    page *prev;
};

#endif //DEFINES_H
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "defines.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

bool read_comando(comando *acao, char* string);
void to_binary(int a, char *bin, int size);

#endif //INTERPRETER_H

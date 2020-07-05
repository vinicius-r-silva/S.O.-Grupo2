#include "../include/process.h"

Process::Process(int id, int size, int pageSize){
    int i = 0;
    this->id = id;
    this->qtdPages = (size + pageSize - 1) / pageSize;

    map = (page_map*)malloc(this->qtdPages * sizeof(page_map));
    for(i = 0; i < this->qtdPages; i++){
        map[i].logical = -1;
        map[i].physical = -1;
    }
}

void Process::set_pages(page *pages){
    int i = 0;
    page *curr = pages;
    page *prev = nullptr;

    for(i = 0; i < qtdPages; i++){
        if(curr != nullptr){
            map[i].logical = i;
            map[i].physical = curr->page_id;

            prev = curr;
            curr = curr->next_lru;
            free(prev);
        }
        else {
            map[i].logical = i;
            map[i].physical = -1;
        }
    }
}

void Process::update_map_entry(int logical, int physical){
    map[logical].physical = physical;
}

int Process::get_logical(int physical){
    int i = 0;
    for(i = 0; i < qtdPages && map[i].physical != physical; i++);
    if(i == qtdPages)
        return -1;

    return i;
}

void Process::replace_physical(int src, int dst){
    int logical = get_logical(src);
    map[logical].physical = dst;
}

std::string Process::print(){
    std::string str;
    int pagesPerLine = 3;
    char bin[6];
    int i;

    bin[5] = '\0';

    //----------------------- printing Header ---------------------/

    str.append("############################################################\n");
    str.append("                        PROCESSO ");
    str.append(std::to_string(id));
    str.append("\n");
    str.append("Offset         ");

    for(i = 0; i < pagesPerLine; i++){
        to_binary(i, bin, 5);
        str.append(bin);
        str.append("          ");
    }
    str.append("\n");

    //Line is the "---..." separation between values
    //its size depends on the quantity of values per line
    char Line[pagesPerLine * 15 + 3];
    memset(Line, '-', pagesPerLine * 15 + 1);
    Line[pagesPerLine * 15 + 1] = '\r';
    Line[pagesPerLine * 15 + 2] = '\0';

    //printing the values
    i = 0;
    while(i < qtdPages){
        //prints the offset
        to_binary(i, bin, 5);
        str.append("         ");
        str.append(Line);
        str.append("\n");
        str.append(bin);
        str.append("     |");

        //print the line values
        do{
            if(map[i].physical < 0){
                str.append("     DISCO    |");
            }else{
                to_binary(i, bin, 5);
                char buff[20];
                sprintf(buff, "     %05d    |", map[i].physical);
                str.append(buff);
            }

            i++;
        }while(i % pagesPerLine != 0 && i < qtdPages);

        if(i < qtdPages)
            str.append("\n");
        str.append("\n");
    }

    //If the grid isn't finished, show that the current address don't exists
    while(i % pagesPerLine != 0){
        str.append("  ----------  |");
        i++;
    }

    str.append("\n          ");
    str.append(Line);
    str.append("\n");

    return str;
}

// Process::Process(){
//     id = -1;
//     map = nullptr;
// }

Process::~Process(){
    //free(map);
}


page_map* Process::get_map(){
    return map;
}

int Process::get_qtdPages(){
    return qtdPages;
}

int Process::get_Id(){
    return id;
}

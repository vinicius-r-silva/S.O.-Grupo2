#include "../include/simulador_memoria.h"
#include "../ui/ui_simulador_memoria.h"
#include "../include/memory.h"

Simulador_Memoria::Simulador_Memoria(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Simulador_Memoria)
{
    ui->setupUi(this);
    Memory mem(0, 32, 4);
    mem.print();
}

Simulador_Memoria::~Simulador_Memoria(){
    delete ui;
}


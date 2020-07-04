#include "simulador_memoria.h"
#include "./ui_simulador_memoria.h"

Simulador_Memoria::Simulador_Memoria(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Simulador_Memoria)
{
    ui->setupUi(this);
}

Simulador_Memoria::~Simulador_Memoria()
{
    delete ui;
}


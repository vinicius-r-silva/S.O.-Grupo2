#include "../include/simulador_memoria.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Simulador_Memoria w;
    w.show();
    return a.exec();
}

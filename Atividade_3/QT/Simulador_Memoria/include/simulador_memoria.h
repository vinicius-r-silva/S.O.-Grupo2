#ifndef SIMULADOR_MEMORIA_H
#define SIMULADOR_MEMORIA_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Simulador_Memoria; }
QT_END_NAMESPACE

class Simulador_Memoria : public QDialog
{
    Q_OBJECT

public:
    Simulador_Memoria(QWidget *parent = nullptr);
    ~Simulador_Memoria();

private:
    Ui::Simulador_Memoria *ui;
};
#endif // SIMULADOR_MEMORIA_H

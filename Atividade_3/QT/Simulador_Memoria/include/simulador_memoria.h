#ifndef SIMULADOR_MEMORIA_H
#define SIMULADOR_MEMORIA_H

#include <QDialog>
#include "memory_management.h"
#include "animation.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Simulador_Memoria; }
QT_END_NAMESPACE

class Simulador_Memoria : public QDialog {
    Q_OBJECT

public:
    Simulador_Memoria(QWidget *parent = nullptr);
    ~Simulador_Memoria();

private slots:
    void on_b_refreshPar_pressed();

    void on_le_pagSize_textChanged(const QString &arg1);
    void on_le_ramSize_textChanged(const QString &arg1);
    void on_le_diskSize_textChanged(const QString &arg1);

    void on_b_play_clicked();
    void on_b_next_clicked();
    void on_b_stop_clicked();

    void on_sl_delay_valueChanged(int value);

    void animationFinished();
    void receiveCommand(int line, QString command);

    void on_te_commands_textChanged();

    void on_rb_lru_clicked();

    void on_rb_clock_clicked();

private:
    Ui::Simulador_Memoria *ui;
    Animation *animation;
    MemoryManagement *mmu;

    void clean();

    QThreadPool *tp;

    int _pagSize;
    int _ramSize;
    int _diskSize;

    QStringList _commands;
    QString _commandsText;
    QString _opHtml;
    QString _opText;
    int _opNumb;

    void readFile();
    void changeColor(int line);
};
#endif // SIMULADOR_MEMORIA_H

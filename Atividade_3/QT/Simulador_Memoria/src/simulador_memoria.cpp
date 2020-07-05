#include "../include/simulador_memoria.h"
#include "../ui/ui_simulador_memoria.h"
#include <unistd.h>
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "../include/defines.h"
#include "../include/interpreter.h"

Simulador_Memoria::Simulador_Memoria(QWidget *parent) : QDialog(parent)
    , ui(new Ui::Simulador_Memoria) {
    /////////////////////////////////
    //SETUP UI AND INTERFACE'S ICONS
    ui->setupUi(this);

    ui->b_play->setIcon(QIcon(":/icons/play.png"));
    ui->b_next->setIcon(QIcon(":/icons/next.png"));
    ui->b_stop->setIcon(QIcon(":/icons/stop.png"));

    readFile();

    animation = new Animation(&_commands);

    /////////////////////////
    //SET INITIAL PARAMETERS
    ui->le_ramSize->setText(QString::number(32));
    ui->le_diskSize->setText(QString::number(32));
    ui->le_pagSize->setText(QString::number(4));
    ui->le_adsSize->setText(QString::number(8));

    ui->sl_delay->setValue(50);

    tp = new QThreadPool();
    tp->setExpiryTimeout(-1);

    QObject::connect(animation, SIGNAL(finished()), this, SLOT(animationFinished()));
    QObject::connect(animation, SIGNAL(sendCommand(int,QString)), this, SLOT(receiveCommand(int,QString)));

    mmu = new MemoryManagement(32, 32, 4);
}

Simulador_Memoria::~Simulador_Memoria(){
    delete ui;
}

void Simulador_Memoria::readFile(){
    QFile file(":/operations.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        if(!file.exists())
            QMessageBox::warning(this, tr("ERROR BOX"), tr("Arquivo não EXISTE."));
        QMessageBox::warning(this, tr("ERROR BOX"), tr("Arquivo não pode ser aberto."));
        return;
    }

    QTextStream in(&file);
    QString text;

    while (!in.atEnd()) {
        QString line = in.readLine();

        if(line.isEmpty())
            continue;

        text = QString(text + line + '\n');
    }

    ui->te_commands->setText(text);

    file.close();
}

void Simulador_Memoria::on_b_refreshPar_pressed(){
    bool wrong = false;

    if(ui->le_pagSize->text().isEmpty() || std::stoi(ui->le_pagSize->text().toStdString()) == 0){
        ui->le_pagSize->setText(QString::number(_pagSize));
        wrong = true;
    }

    if(ui->le_adsSize->text().isEmpty() || std::stoi(ui->le_adsSize->text().toStdString()) == 0){
        ui->le_adsSize->setText(QString::number(_adsSize));
        wrong = true;
    }

    if(ui->le_ramSize->text().isEmpty() || std::stoi(ui->le_ramSize->text().toStdString()) == 0){
        ui->le_ramSize->setText(QString::number(_ramSize));
        wrong = true;
    }

    if(ui->le_diskSize->text().isEmpty() || std::stoi(ui->le_diskSize->text().toStdString()) == 0){
        ui->le_diskSize->setText(QString::number(_diskSize));
        wrong = true;
    }

    if(wrong){
        QMessageBox::warning(this, tr("ERROR BOX"), tr("Há parâmetro(s) vazio(s) e/ou nulo(s)"));
        return;
    }

    std::cout << "Tamanho da Página: " << _pagSize << std::endl;
    std::cout << "Tamanho do endereço: " << _adsSize << std::endl;
    std::cout << "Tamanho da RAM: " << _ramSize << std::endl;
    std::cout << "Tamanho do Disco: " << _diskSize << std::endl;
}

void Simulador_Memoria::on_le_pagSize_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();
    int pagSize;

    try{
        pagSize = std::stoi(edit);

        if((_ramSize != 0 && _ramSize%pagSize != 0) || (_diskSize != 0 && _diskSize%pagSize != 0)){
            QMessageBox::warning(this, tr("ERROR BOX"), tr("Escolha um tamanho de página que seja divisível pelo tamanho do disco e pelo tamanho da RAM"));
            ui->le_pagSize->setText(QString::number(_pagSize));
            return;
        }

        if(pagSize != _pagSize && pagSize != 0)
            _pagSize = pagSize;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void Simulador_Memoria::on_le_adsSize_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();
    int adsSize;

    try{
        adsSize = std::stoi(edit);

        if(adsSize != _adsSize && adsSize != 0)
            _adsSize = adsSize;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void Simulador_Memoria::on_le_ramSize_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();
    int ramSize;

    try{
        ramSize = std::stoi(edit);

        if(ramSize != _ramSize && ramSize != 0)
            _ramSize = ramSize;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void Simulador_Memoria::on_le_diskSize_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();

    int diskSize;

    try{
        diskSize = std::stoi(edit);

        if(diskSize != _diskSize && diskSize != 0)
            _diskSize = diskSize;

    }catch(std::invalid_argument e){
    }catch(std::out_of_range e2){
    }
}

void Simulador_Memoria::on_b_play_clicked(){
    ui->b_play->setEnabled(false);
    animation->setPausing(false);

    if(tp->activeThreadCount() == 0){
        _opHtml = ui->te_commands->toHtml();
        animation->setRunning(true);
        tp->tryStart(animation);
    }
}

void Simulador_Memoria::on_b_next_clicked(){    
    ui->b_play->setEnabled(true);
    animation->setPausing(true);
    animation->addStep();

    if(tp->activeThreadCount() == 0){
        _opHtml = ui->te_commands->toHtml();
        animation->setRunning(true);
        tp->tryStart(animation);
    }
}

void Simulador_Memoria::on_b_stop_clicked(){
    animation->setRunning(false);
    animation->setPausing(false);
}

void Simulador_Memoria::animationFinished(){
    ui->b_play->setEnabled(true);
    animation->setLine(0);
    ui->te_commands->setHtml(_opHtml);
}

void Simulador_Memoria::receiveCommand(int line, QString commandStr){
    changeColor(line);
    // qDebug() << commandStr;

    comando cmd;
    std::string stdStr = commandStr.toStdString();
    char str[stdStr.size() + 1];
	strcpy(str, stdStr.c_str());
    if(read_comando(&cmd, str) == FAILURE){
        ui->te_warning->setText("Comando incorreto");
    }
    else{
        qDebug() << "pid: " << cmd.pid << ", action: " << cmd.action << ", arg: " << cmd.arg << "\n";
        if(cmd.action == 'C' && cmd.pid == 1){
            mmu->create_process(1, 4);
            qDebug() << "processo criado\n";
        }

        ui->te_RAM->setText(QString::fromStdString(mmu->get_ram()));

        ui->te_disk->setText(QString::fromStdString(mmu->get_disk()));

        ui->te_warning->setText(mmu->get_warning());

    }
}

void Simulador_Memoria::changeColor(int line){
    QString newText, color;

    QString colRed = QString("<font color=\"red\">");
    QString colBlack = QString("<font color=\"black\">");

    QString endFont = "</font>";
    QString ParagraphEnd = "</p>";

    auto ref = _opHtml.split(ParagraphEnd);

    for(int i = 0; i < ref.size();i++){
        QString s = ref.at(i);
        int insert = s.lastIndexOf(">");

        color = (i <= line)? colRed : colBlack;

        s.insert(insert+1, color);

        s.append(endFont+ParagraphEnd);
        newText.append(s);
    }

    ui->te_commands->setHtml(newText);

    QTextCursor cursor(ui->te_commands->document());
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line);
    ui->te_commands->setTextCursor(cursor);
}

void Simulador_Memoria::on_sl_delay_valueChanged(int value){
    animation->setDelay(value);
}

void Simulador_Memoria::on_te_commands_textChanged(){
    QString newHtml = ui->te_commands->toHtml();
    newHtml.replace("<span style=\" color:#ff0000;\">", "");
    newHtml.replace("<span style=\" color:#000000;\">", "");
    newHtml.replace("</span>", "");
    _opHtml = newHtml;

    _commandsText = ui->te_commands->toPlainText();
    _commands = _commandsText.split(QRegExp("[\n]"),QString::SkipEmptyParts);
}

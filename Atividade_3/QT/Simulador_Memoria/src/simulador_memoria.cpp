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
    ui->le_ramSize->setText(QString::number(32768));
    ui->le_diskSize->setText(QString::number(32768));
    ui->le_pagSize->setText(QString::number(4096));

    _prevRS = _ramSize;
    _prevDS = _diskSize;
    _prevPS = _pagSize;

    ui->sl_delay->setValue(50);

    ui->rb_lru->setChecked(true);

    tp = new QThreadPool();
    tp->setExpiryTimeout(-1);

    QObject::connect(animation, SIGNAL(finished()), this, SLOT(animationFinished()));
    QObject::connect(animation, SIGNAL(sendCommand(int,QString)), this, SLOT(receiveCommand(int,QString)));

    mmu = new MemoryManagement(_ramSize, _diskSize, _pagSize, LRU);
}

Simulador_Memoria::~Simulador_Memoria(){
    delete ui;
}

void Simulador_Memoria::readFile(){
    QFile file(":/operations.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
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

void Simulador_Memoria::on_b_refreshPar_clicked(){
    bool wrong = false;

    if(ui->le_pagSize->text().isEmpty() || std::stoi(ui->le_pagSize->text().toStdString()) == 0){
        _pagSize = _prevPS;
        ui->le_pagSize->setText(QString::number(_pagSize));
        wrong = true;
    }

    if(ui->le_ramSize->text().isEmpty() || std::stoi(ui->le_ramSize->text().toStdString()) == 0){
        _ramSize = _prevRS;
        ui->le_ramSize->setText(QString::number(_ramSize));
        wrong = true;
    }

    if(ui->le_diskSize->text().isEmpty() || std::stoi(ui->le_diskSize->text().toStdString()) == 0){
        _diskSize = _prevDS;
        ui->le_diskSize->setText(QString::number(_diskSize));
        wrong = true;
    }

    if(wrong){
        QMessageBox::warning(this, tr("ERROR BOX"), tr("Há parâmetro(s) vazio(s) e/ou nulo(s)"));
        return;
    }else if((_ramSize != 0 && _ramSize%_pagSize != 0) || (_diskSize != 0 && _diskSize%_pagSize != 0)){
            QMessageBox::warning(this, tr("ERROR BOX"), tr("Escolha um tamanho de página que seja divisível pelo tamanho do disco e pelo tamanho da RAM"));
            _pagSize = _prevPS;
            ui->le_pagSize->setText(QString::number(_pagSize));
            _ramSize = _prevRS;
            ui->le_ramSize->setText(QString::number(_ramSize));
            _diskSize = _prevDS;
            ui->le_diskSize->setText(QString::number(_diskSize));
            return;
    }

    on_b_stop_clicked();

    _prevPS = _pagSize;
    _prevRS = _ramSize;
    _prevDS = _diskSize;

    mmu->updateSizes(_ramSize, _diskSize, _pagSize);
}

void Simulador_Memoria::on_le_pagSize_textChanged(const QString &arg1){
    std::string edit = arg1.toStdString();
    int pagSize;

    try{
        pagSize = std::stoi(edit);

        if(pagSize != _pagSize)
            _pagSize = pagSize;

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

    ui->te_RAM->setText(QString::fromStdString(mmu->get_ram()));
    ui->te_disk->setText(QString::fromStdString(mmu->get_disk()));
    ui->te_warning->setText(mmu->get_warning());

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

    ui->te_RAM->setText(QString::fromStdString(mmu->get_ram()));
    ui->te_disk->setText(QString::fromStdString(mmu->get_disk()));
    ui->te_warning->setText(mmu->get_warning());

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
    mmu->clean_all();
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
        //qDebug() << "pid: " << cmd.pid << ", action: " << cmd.action << ", arg: " << cmd.arg << "\n";
        switch (cmd.action){
        case 'C':
            mmu->create_process(cmd.pid, cmd.arg);
            break;

        case 'K':
            mmu->kill_process(cmd.pid);
            break;

        case 'R':
            mmu->acesso_memoria(cmd.pid, cmd.arg, "leitura");
            break;

        case 'W':
            mmu->acesso_memoria(cmd.pid, cmd.arg, "escrita");
            break;

        case 'P':
            mmu->operacao(cmd.pid, "CPU");
            break;

        case 'I':
            mmu->operacao(cmd.pid, "I/O");
            break;
        
        default:
            break;
        }

        ui->te_RAM->setText(QString::fromStdString(mmu->get_ram()));
        ui->te_disk->setText(QString::fromStdString(mmu->get_disk()));
        ui->te_pagTable->setText(QString::fromStdString(mmu->get_proTable()));
        ui->te_warning->setText(mmu->get_warning());
        updateMonitor();

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

void Simulador_Memoria::on_rb_lru_clicked(){
    mmu->set_page_replacement(LRU);
    on_b_stop_clicked();
}

void Simulador_Memoria::on_rb_clock_clicked(){
    mmu->set_page_replacement(RELOGIO);
    on_b_stop_clicked();
}

void Simulador_Memoria::updateMonitor(){
    int ramAv = mmu->getRamAv();
    int diskAv = mmu->getDiskAv();
    int ramProc = mmu->get_processes_at_ram();
    int diskProc = mmu->get_processes_at_disk();
    int pagFaults = mmu->getQtdPageFaults();

    ui->l_pagFaults->setText(QString::number(pagFaults));
    ui->l_ramProc->setText(QString::number(ramProc));
    ui->l_diskProc->setText(QString::number(diskProc));
    ui->l_ramPag->setText(QString::number((_ramSize - ramAv)/_pagSize));
    ui->l_diskPag->setText(QString::number((_diskSize - diskAv)/_pagSize));
    ui->l_ramFree->setText(QString::number(ramAv));
    ui->l_memFree->setText(QString::number(diskAv + ramAv));
}

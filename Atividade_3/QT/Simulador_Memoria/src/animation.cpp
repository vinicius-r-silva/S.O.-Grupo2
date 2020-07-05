#include "../include/animation.h"
#include <iostream>
#include <unistd.h>

#define MAXDELAY 15000

Animation::Animation(QStringList *commands){
    _commands = commands;
    _line = 0;
    _step = 0;
    _pausing = false;
    _running = false;

    QRunnable::setAutoDelete(false);
}

void Animation::run(){
    usleep(_delay * MAXDELAY);

    for(_line; _line < _commands->size() && _running; _line++){
        while(_pausing){
            if(_step > 0){
                _step--;
                break;
            }
        }

        QString command = _commands->at(_line);
        if(_running)
            emit(sendCommand(_line, command));

        if(!_pausing)
            usleep(_delay * MAXDELAY);
    }

    _running = false;

    emit(finished());
}

void Animation::setPausing(bool pausing){
    _pausing = pausing;
}

void Animation::setRunning(bool running){
    _running = running;
}

void Animation::addStep(){
    _step++;
}

void Animation::setLine(int line){
    _line = line;
}

void Animation::setDelay(int delay){
    _delay = delay;
}

int Animation::getLine(){
    return _line;
}

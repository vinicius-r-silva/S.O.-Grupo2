#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>
#include <QThreadPool>
#include <QThread>
#include <QTextEdit>

class Animation: public QObject, public QRunnable {
    Q_OBJECT

public:
    Animation(QStringList *commands);

    void setLine(int line);
    void setRunning(bool running);
    void setPausing(bool pausing);
    void setDelay(int delay);
    void addStep();

    int getLine();

signals:
    void sendCommand(int line, QString command);
    void finished();

private slots:
    void run();

private:
    QStringList *_commands;
    int _line;
    int _delay;
    int _step;
    bool _running;
    bool _pausing;

};

#endif // ANIMATION_H



#include <QThread>
#include "main.h"
#include "timesync.h"
#include "scriptobjectthread.h"

ScriptObjectThread::ScriptObjectThread() : QObject()
{
    QThread *scriptObjectThread=new QThread;
    connect(this,SIGNAL(finishThread()),scriptObjectThread,SLOT(quit()));
    this->moveToThread(scriptObjectThread);
    scriptObjectThread->start();
}

ScriptObjectThread::~ScriptObjectThread()
{
    emit finishThread();
}

void ScriptObjectThread::performFileWrite(QString path, QByteArray data)
{
    QFile dataFile(path);
    if(!dataFile.open(QIODevice::WriteOnly | QIODevice::Text))return;

    data.append('\n');
    dataFile.write(data);

    dataFile.close();
}

void ScriptObjectThread::performFileAppend(QString path, QByteArray data)
{
    QFile dataFile(path);
    if(!dataFile.open(QIODevice::Append | QIODevice::Text))return;

    data.append('\n');
    dataFile.write(data);

    dataFile.close();
}

void ScriptObjectThread::performFileReadLine(QString path, qint64 seek, quint32 fileOperationNumber)
{
    if(seek==-1 && positions.contains(path)){
        if(positions[path]==-1){emit fileReadResult("",fileOperationNumber);return;}
        seek=positions[path];
    }

    QFile dataFile(path);
    if(!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)){emit fileReadResult("",fileOperationNumber);return;}
    if(seek>0 && seek<dataFile.size())dataFile.seek(seek);

    QByteArray data=dataFile.readLine();
    if(data.at(data.length()-1)=='\n')data.chop(1);
    emit fileReadResult(data,fileOperationNumber);

    if(dataFile.atEnd())
        positions[path]=-1;
    else
        positions[path]=dataFile.pos();

    dataFile.close();
}

void ScriptObjectThread::performFileReadLineSimple(QString path, quint32 fileOperationNumber)
{
    qint64 seek=0;
    if(positions.contains(path)){
        if(positions[path]==-1){emit fileReadResult("",fileOperationNumber);return;}
        seek=positions[path];
    }

    QFile dataFile(path);
    if(!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)){emit fileReadResult("",fileOperationNumber);return;}
    if(seek>0 && seek<dataFile.size())dataFile.seek(seek);

    QByteArray data=dataFile.readLine();
    if(data.at(data.length()-1)=='\n')data.chop(1);
    emit fileReadResult(data,fileOperationNumber);

    if(dataFile.atEnd())
        positions[path]=-1;
    else
        positions[path]=dataFile.pos();

    dataFile.close();
}

void ScriptObjectThread::performFileRead(QString path, qint64 size, quint32 fileOperationNumber)
{
    QFile dataFile(path);
    if(!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)){emit fileReadResult("",fileOperationNumber);return;}

    QByteArray data=dataFile.read(size);
    emit fileReadResult(data,fileOperationNumber);

    dataFile.close();
}
void ScriptObjectThread::performFileReadAll(QString path, quint32 fileOperationNumber)
{
    QFile dataFile(path);
    if(!dataFile.open(QIODevice::ReadOnly | QIODevice::Text)){emit fileReadResult("",fileOperationNumber);return;}

    QByteArray data=dataFile.readAll();
    emit fileReadResult(data,fileOperationNumber);

    dataFile.close();
}

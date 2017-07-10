

#ifndef SCRIPTOBJECTTHREAD_H
#define SCRIPTOBJECTTHREAD_H

#include <QObject>
#include <QHash>

class ScriptObjectThread : public QObject
{
    Q_OBJECT
public:
    ScriptObjectThread();
    ~ScriptObjectThread();

private:
    QHash<QString,qint64> positions;

public slots:
    void performFileWrite(QString,QByteArray);
    void performFileAppend(QString,QByteArray);
    void performFileReadLine(QString,qint64,quint32);
    void performFileReadLineSimple(QString,quint32);
    void performFileRead(QString,qint64,quint32);
    void performFileReadAll(QString,quint32);

signals:
    void finishThread();
    void fileReadResult(QByteArray,quint32);

public slots:
};

#endif // SCRIPTOBJECTTHREAD_H

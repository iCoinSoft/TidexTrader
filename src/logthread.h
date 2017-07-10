

#ifndef LOGTHREAD_H
#define LOGTHREAD_H

#include <QThread>

class LogThread : public QThread
{
	Q_OBJECT

public:
	void writeLog(QByteArray,int debugLevel=0);
    void writeLogB(QString mess,int dLevel=0){writeLog(mess.toLatin1(),dLevel);}
	LogThread(bool writeFile=true);
	~LogThread();

private:
	bool writeFile;
	void run();
signals:
	void writeLogSignal(QByteArray,int);
	void sendLogSignal(QByteArray);
public slots:
	void writeLogSlot(QByteArray,int);
};

#endif // LOGTHREAD_H

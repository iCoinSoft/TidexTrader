

#ifndef JULYLOCKFILE_H
#define JULYLOCKFILE_H

class QFile;
class QUdpSocket;
#include <QObject>
#include <QString>

class JulyLockFile : public QObject
{
    Q_OBJECT

public:
	bool isLocked();
	JulyLockFile(QString imageName);
	~JulyLockFile();

private:
    quint16 lockPort;
    QString lockFilePath;
	bool isLockedFile;
	QFile *lockFile;
	QUdpSocket *lockSocket;
private slots:
    void updateLockFile();
};

#endif // JULYLOCKFILE_H

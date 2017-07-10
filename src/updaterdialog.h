

#ifndef UPDATERDIALOG_H
#define UPDATERDIALOG_H

#include <QDialog>
#include "ui_updaterdialog.h"
#include "julyhttp.h"
#include <QTimer>

class UpdaterDialog : public QDialog
{
	Q_OBJECT

public:
	UpdaterDialog(bool feedbackMessage);
	~UpdaterDialog();

private:
	QByteArray getMidData(QString a, QString b,QByteArray *data);
	bool downloaded100;
	bool feedbackMessage;
	QTimer *timeOutTimer;
    void downloadError(int);
    void downloadErrorFile(int);
	QString updateVersion;
	QByteArray updateSignature;
    QByteArray versionSignature;
	QString updateChangeLog;
	QString updateLink;
	
	int stateUpdate;
    bool autoUpdate;
	JulyHttp *httpGet;
    JulyHttp *httpGetFile;
	Ui::UpdaterDialog ui;
private slots:
	void invalidData(bool);
	void dataReceived(QByteArray,int);
	void exitSlot();
	void dataProgress(int);
	void buttonUpdate();
};

#endif // UPDATERDIALOG_H

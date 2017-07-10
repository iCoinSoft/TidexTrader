

#ifndef NEWPASSWORDDIALOG_H
#define NEWPASSWORDDIALOG_H

#include <QDialog>
#include "ui_newpassworddialog.h"

class NewPasswordDialog : public QDialog
{
	Q_OBJECT

public:
	int getExchangeId();
	QString selectedProfileName();
	void updateIniFileName();
	QString getRestSign();
	QString getRestKey();
	QString getPassword();
	NewPasswordDialog(qint32);
	~NewPasswordDialog();

private:
    void setDiffBar(int);
	qint32 exchangeNum;
	QString exchangeName;
	bool clientIdEnabled;
	QString getApiUrl;
	bool isValidPassword();
	Ui::NewPasswordDialog ui;
private slots:
	void okPressed();
	void exchangeChanged(QString);
	void checkToEnableButton();
	void getApiKeySecretButton();
	int difficulty(QString, bool *, QString *);
};

#endif // NEWPASSWORDDIALOG_H

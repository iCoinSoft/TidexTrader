

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include "ui_passworddialog.h"

class PasswordDialog : public QDialog
{
	Q_OBJECT

public:
	QString lockFilePath(QString);
	bool isProfileLocked(QString);
	QString getPassword();
	bool resetData;
	bool newProfile;
	QString getIniFilePath();
	PasswordDialog(QWidget *parent = 0);
	~PasswordDialog();

private slots:
	void resetDataSlot();
	void addNewProfile();
	void checkToEnableButton(QString);
    void on_descriptionGroupBox_toggled(bool arg1);
    void showTimeMessage(QString);

private:
	void accept();
	Ui::PasswordDialog ui;
};

#endif // PASSWORDDIALOG_H

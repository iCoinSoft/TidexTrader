

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QStandardItemModel>
#include "settingsdialoglistelement.h"
#include "settingsgeneral.h"
#include "settingsnetworkproxy.h"
#include "settingsdecimals.h"
#include "ui_settingsdialog.h"

class SettingsDialogListElement;

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog();
	~SettingsDialog();
    void reject(){};

    void clickOnList(qint32);
    void disableTranslateButton();

private:
	Ui::SettingsDialog ui;
    QVBoxLayout *listLayout;
    QList<SettingsDialogListElement *> listListElement;
	SettingsGeneral *settingsGeneral;
	SettingsNetworkProxy *settingsNetworkProxy;
    SettingsDecimals *settingsDecimals;
	void resizeNameList();
	void configureNameList();
    void addDialog(QString, QString, QWidget*);
    void closeEvent(QCloseEvent *);
	bool isSettingsSaved();
	void settingsSave();
    void settingsDiscard();
};

#endif // SETTINGS_H

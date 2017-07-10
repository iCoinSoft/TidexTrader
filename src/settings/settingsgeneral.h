
#ifndef SETTINGSGENERAL_H
#define SETTINGSGENERAL_H

#include <QWidget>
#include <QSettings>
#include "ui_settingsgeneral.h"

class SettingsGeneral : public QWidget
{
	Q_OBJECT

public:
	SettingsGeneral();
	~SettingsGeneral();
	Ui::SettingsGeneral ui;

private:
    QSettings *iniSettings;
    QSettings *mainSettings;
	void loadLanguage();
    void saveLanguage();
    void loadOther();
    void saveOther();
	void loadUpdates();
    void saveUpdates();
    void loadTime();
    void saveTime();

public slots:
	void on_saveButton_clicked();
    void on_revertChangesButton_clicked();

private slots:
	void on_restoreDefaultsButton_clicked();
	void anyValueChanged();
    void on_showTranslationButton_clicked();

};

#endif // SETTINGSGENERAL_H

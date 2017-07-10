

#ifndef SETTINGSNETWORKPROXY_H
#define SETTINGSNETWORKPROXY_H

#include <QWidget>
#include <QSettings>
#include "ui_settingsnetworkproxy.h"

class SettingsNetworkProxy : public QWidget
{
	Q_OBJECT

public:
	SettingsNetworkProxy();
	~SettingsNetworkProxy();
	Ui::SettingsNetworkProxy ui;

private:
	QSettings *proxySettings;
	QSettings *networkSettings;
	void allSetStatus();
	void loadProxy();
	void saveProxy();
	void activateProxy();
	void loadNetwork();
	void saveNetwork();
	void activateNetwork();

public slots:
	void on_saveButton_clicked();
    void on_revertChangesButton_clicked();

private slots:
	void on_enabledCheckBox_stateChanged(int);
	void on_autoCheckBox_stateChanged(int);
	void on_restoreDefaultsButton_clicked();
	void anyValueChanged();

};

#endif // NETWORKPROXY_H

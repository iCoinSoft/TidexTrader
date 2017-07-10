

#ifndef SETTINGSDECIMALS_H
#define SETTINGSDECIMALS_H

#include <QWidget>
#include <QSettings>
#include "ui_settingsdecimals.h"

class SettingsDecimals : public QWidget
{
    Q_OBJECT

public:
    SettingsDecimals();
    ~SettingsDecimals();
    Ui::SettingsDecimals ui;

private:
    QSettings *decimalsSettings;
    void loadDecimals();
    void saveDecimals();
    void activateDecimals();

public slots:
    void on_saveButton_clicked();
    void on_revertChangesButton_clicked();

private slots:
    void on_restoreDefaultsButton_clicked();
    void anyValueChanged();
};

#endif // SETTINGSDECIMALS_H

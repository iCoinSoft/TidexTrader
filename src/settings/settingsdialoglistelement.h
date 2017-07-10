

#ifndef SETTINGSDIALOGLISTELEMENT_H
#define SETTINGSDIALOGLISTELEMENT_H

#include <QWidget>
#include <QMouseEvent>
#include "settingsdialog.h"
#include "ui_settingsdialoglistelement.h"

class SettingsDialog;

class SettingsDialogListElement : public QWidget
{
    Q_OBJECT

public:
    SettingsDialogListElement(SettingsDialog *, qint32, QString, QString);
    ~SettingsDialogListElement();
    quint32 width;
    void selectedItem();
    void clearSelection();

private:
    qint32 index;
    SettingsDialog *parent;
    void mouseReleaseEvent(QMouseEvent *);
    Ui::SettingsDialogListElement ui;
};

#endif // SETTINGSDIALOGLISTELEMENT_H

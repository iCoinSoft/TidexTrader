

#ifndef ADDSCRIPTWINDOW_H
#define ADDSCRIPTWINDOW_H

#include <QDialog>

namespace Ui {
class AddScriptWindow;
}

class AddScriptWindow : public QDialog
{
    Q_OBJECT

public:
    QString scriptFileName();
    QString getCopyFromFile();
    QString copyFromExistingScript;
    QString scriptName;
    explicit AddScriptWindow(QWidget *parent = 0);
    ~AddScriptWindow();

private:
    Ui::AddScriptWindow *ui;

private slots:
    void on_openFile_clicked();
    void onGroupContentChanged(bool);
    void on_scriptName_textChanged(QString);
    void on_buttonAddScript_clicked();
    void checkValidButton();
};

#endif // ADDSCRIPTWINDOW_H

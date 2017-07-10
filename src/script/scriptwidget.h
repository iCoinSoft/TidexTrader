

#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QWidget>
#include <QScriptEngine>
#include <QMenu>

class QToolButton;
class ScriptObject;
namespace Ui {
class ScriptWidget;
}

class ScriptWidget : public QWidget
{
    Q_OBJECT

public:
	void replaceScript(QString);
    void setRunning(bool on);
    bool isRunning();
    explicit ScriptWidget(QString name, QString filePathSave="", QString fileCopyFrom="");
    ~ScriptWidget();
    bool executeScript(QString script, bool testMode);
    QString getFilePath();
    int getRuleGroupId();
    bool removeGroup();
    void languageChanged();
    void currencyChanged();
    bool saveScriptToFile(QString file="");

private slots:
    void on_ruleAddButton_clicked();
    void noteChanged();
    void errorHappend(int, QString);
    void addEventsClicked();
    void addFunctionClicked();
    void on_validateButton_clicked();
    void writeConsole(QString);
    void setRunningUi(bool);
    void clearLog();
    void on_buttonStartStop_clicked();
    void on_buttonClear_clicked();
    void on_limitRowsValue_valueChanged(int);
    void on_buttonSave_clicked();
    void on_scriptSave_clicked();
    void on_consoleOutput_textChanged();

protected:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    void insertFilePath(QString description, QString mask);
    bool executeScript(bool testMode);

    QAction *NewEventsAction(QString name);
    QAction *NewFunctionsAction(QString name, QString params="");

    QList<QToolButton *> menuButtons;
    QMenu insertEventMenu;
    QMenu insertCommandMenu;
    QMenu insertFunctionMenu;

    ScriptObject *scriptObject;
    QString scriptName;
    QString fileName;
    Ui::ScriptWidget *ui;
signals:
    void setRuleTabRunning(QString,bool);
};

#endif // SCRIPTWIDGET_H

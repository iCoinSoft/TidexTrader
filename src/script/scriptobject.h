

#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H

#include <QObject>
class QScriptContext;
class QScriptEngine;
class QScriptValue;
class QDoubleSpinBox;
#include <QVariant>
#include <QString>
class QScriptEngine;
#include <QStringList>
#include <QTimer>
#include "scriptobjectthread.h"

class ScriptObject : public QObject
{
    Q_OBJECT
public:
    int testResult;
    QString scriptName;
    bool isRunning() {return isRunningFlag;}
    bool stopScript();
    bool executeScript(QString, bool);
    explicit ScriptObject(QString scriptName);
    ~ScriptObject();
    QStringList indicatorList;
    QStringList functionsList;
    QStringList argumentsList;
    QStringList commandsList;
private:
    void initValueChangedPrivate(QString &symbol, QString &scriptNameInd, double &val, bool forceEmit=false);
	void deleteEngine();
    bool scriptWantsOrderBookData;
    void timerCreate(int milliseconds, QString &command, bool once);
    QMap<QTimer *,bool> timerMap;
    double orderBookInfo(QString &symbol,double &value, bool isAsk, bool getPrice);
    bool haveTimer;
    QTimer *secondTimer;
    bool pendingStop;
    void setRunning(bool);
    bool isRunningFlag;
    bool replaceString(QString what, QString to, QString &text, bool skipFirstLeft);
    QString sourceToScript(QString);
    QScriptEngine *engine;
    QStringList functionNames;
    QList<QDoubleSpinBox*> spinBoxList;
    void addIndicator(QDoubleSpinBox *spinbox, QString value);
    void addCommand(QString, QList<QByteArray>);
    void addFunction(QString name);
    bool testMode;
    QMap<QString,double> indicatorsMap;
    ScriptObjectThread performThread;
    QHash<quint32,QString> arrayFileReadResult;
    quint32 fileOperationNumber;
    qint32 fileOpenCount;
public slots:
    void sendEvent(QString symbol, QString name, double value);
    void sendEvent(QString name, double value);
    void timer(double seconds, QString _command_);
    void delay(double seconds, QString _command_);
    void log(QVariant);
    void log(QVariant,QVariant);
    void log(QVariant,QVariant,QVariant);
    void log(QVariant,QVariant,QVariant,QVariant);
    void log(QVariant,QVariant,QVariant,QVariant,QVariant);
    void log(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant);
    void log(QVariantList);
    void logClear();
    void test(int);

    void beep();
    void playWav(QString _filePath_);

    void say(QString _text_);
    void say(int);
    void say(double);
    void say(QVariant);
    void say(QVariantList);
    void say(QVariant,QVariant);
    void say(QVariant,QVariant,QVariant);
    void say(QVariant,QVariant,QVariant,QVariant);
    void say(QVariant,QVariant,QVariant,QVariant,QVariant);
    void say(QVariant,QVariant,QVariant,QVariant,QVariant,QVariant);

    void groupDone();
    void groupStart(QString _name_);
    void groupStop(QString _name_);
    void groupStop();
    bool groupIsRunning(QString _name_);

    void startApp(QString _filePath_);
    void startApp(QString,QStringList);
    void startApp(QString,QString);
    void startApp(QString,QString,QString);
    void startApp(QString,QString,QString,QString);
    void startApp(QString,QString,QString,QString,QString);

    void sell(double amount, double price);
    void sell(QString symbol, double amount, double price);
    void buy(double amount, double price);
    void buy(QString symbol, double amount, double price);
    void cancelOrders(QString symbol);
    void cancelOrders();
    void cancelAsks(QString symbol);
    void cancelBids(QString symbol);
    void cancelAsks();
    void cancelBids();

    int getOpenAsksCount();
    int getOpenBidsCount();
    int getOpenOrdersCount();

    double getAsksVolByPrice(double price);
    double getAsksPriceByVol(double volume);
    double getAsksVolByPrice(QString symbol, double price);
    double getAsksPriceByVol(QString symbol, double volume);

    double getBidsVolByPrice(double price);
    double getBidsPriceByVol(double volume);
    double getBidsVolByPrice(QString symbol, double price);
    double getBidsPriceByVol(QString symbol, double volume);

    quint32 getTimeT();
    double get(QString indicator);
    double get(QString symbol, QString indicator);

    void fileWrite(QVariant path,QVariant data);
    void fileAppend(QVariant path,QVariant data);
    QVariant fileReadLine(QVariant path,qint64 seek=-1);
    QVariant fileReadLineSimple(QVariant path);
    QVariant fileRead(QVariant path,qint64 size);
    QVariant fileReadAll(QVariant path);
    QVariant dateTimeString();
private slots:
    void initValueChanged(QString symbol, QString name, double val);
    void timerOut();
    void secondSlot();
    void indicatorValueChanged(double);
    void fileReadResult(QByteArray,quint32);
signals:
    void eventSignal(QString symbol, QString name, double value);
    void startAppSignal(QString,QStringList);
    void setGroupEnabled(QString name, bool enabled);
    void setGroupDone(QString);
    void runningChanged(bool);
    void errorHappend(int, QString);
    void logClearSignal();
    void writeLog(QString);
    void valueChanged(QString,QString,double);

    void performFileWrite(QString,QByteArray);
    void performFileAppend(QString,QByteArray);
    void performFileReadLine(QString,qint64,quint32);
    void performFileReadLineSimple(QString,quint32);
    void performFileRead(QString,qint64,quint32);
    void performFileReadAll(QString,quint32);
    void fileReadExitLoop();
};

#endif // SCRIPTOBJECT_H

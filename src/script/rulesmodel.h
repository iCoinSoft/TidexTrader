

#ifndef RULESMODEL_H
#define RULESMODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include "ruleholder.h"
#include "scriptobject.h"

class RulesModel : public QAbstractItemModel
{
	Q_OBJECT

public:
    void currencyChanged();
    void addRule(RuleHolder &holder, bool running=false);
    void updateRule(int row, RuleHolder &holder, bool running=false);
    QList<RuleHolder> holderList;
    int getStateByRow(int row);
    bool isRowPaused(int row);
	void restoreRulesFromString(QString);
    bool haveWorkingRule();
	bool isConcurrentMode;
    RulesModel(QString groupName);
	~RulesModel();

	void disableAll();
	void enableAll();

	void moveRowUp(int row);
	void moveRowDown(int row);

	void setRuleStateByRow(int row, int state);
    bool testRuleByRow(int row, bool forceTest=false);
    void clear();
	void removeRuleByRow(int row);

	void setHorizontalHeaderLabels(QStringList list);

	QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
private slots:
	void writeLogSlot(QString);
    void runningChanged(bool);
    void setGroupDone(QString);
private:
    void swapRows(int,int);
    bool lastRuleGroupIsRunning;
    int runningCount;
    void checkRuleGroupIsRunning();
    QString groupName;
    void setStateByName(QString, int);
    quint32 lastRuleId;
    QList<ScriptObject*> scriptList;
    QList<int> stateList;
    QList<bool> pauseList;
    QList<quint32> doneList;
    int stateWidth;
    QStringList headerLabels;
    int columnsCount;
signals:
	void writeLog(QString);
    void ruleDone();
    void groupDone();
    void setRuleTabRunning(QString,bool);
	void rulesCountChanged();
};

#endif // RULESMODEL_H

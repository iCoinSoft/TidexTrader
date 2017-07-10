

#ifndef ADDRULEDIALOG_H
#define ADDRULEDIALOG_H

#include <QDialog>
#include "ruleholder.h"

namespace Ui {
class AddRuleDialog;
}
class QComboBox;
class QDoubleSpinBox;

class AddRuleDialog : public QDialog
{
    Q_OBJECT

public:
    bool saveClicked;
	QString getGroupName();
    bool isRuleEnabled();
    RuleHolder getRuleHolder();
    void fillByHolder(RuleHolder&, bool enabled);
    explicit AddRuleDialog(QString groupName, QWidget* parent = 0);
    ~AddRuleDialog();

private slots:
    void reCacheCode();
    void fixSizeWindow();
    void on_variableA_currentIndexChanged(int index);
    void on_variableB_currentIndexChanged(int index);
    void on_thanAmountPercent_toggled(bool checked);
    void on_thanType_currentIndexChanged(int index);
    void on_playButton_clicked();
    void on_thanTextBrowse_clicked();
    void on_thanPriceType_currentIndexChanged(int index);
    void on_variableBPercent_toggled(bool checked);
    void on_thanPricePercent_toggled(bool checked);
    void on_variableBPercentButton_clicked();
    void on_thanAmountPercentButton_clicked();
    void on_thanPricePercentButton_clicked();
    void on_variableBFee_currentIndexChanged(int index);
    void on_thanAmountFee_currentIndexChanged(int index);
    void on_thanPriceFee_currentIndexChanged(int index);
    void on_thanText_textChanged(const QString &arg1);
    void on_codePreview_toggled(bool checked);
    void on_buttonAddRule_clicked();
    void on_buttonSaveRule_clicked();
    void on_fillFromBuyPanel_clicked();
    void on_fillFromSellPanel_clicked();
    void on_valueBSymbol_currentIndexChanged(int index);
    void on_thanSymbol_currentIndexChanged(int index);
    void on_valueASymbol_currentIndexChanged(int index);

    void on_sayCode_currentIndexChanged(int index);

private:
    QString groupName;
	QString getFreeGroupName();
    void fixSize(bool fitToWindow = false);
    bool pendingFix;
    bool ruleIsEnabled;
    QList<QDoubleSpinBox*> usedSpinBoxes;
	RuleHolder lastHolder;
    QString comboData(QComboBox* list, int row);
    QString comboCurrentData(QComboBox*);
    void setComboIndex(QComboBox* list, int& row);
    void setComboIndex(QComboBox* list, QString& text);
    void setComboIndexByData(QComboBox* list, QString& data);
    QString currentThanType;
    Ui::AddRuleDialog* ui;
};

#endif // ADDRULEDIALOG_H

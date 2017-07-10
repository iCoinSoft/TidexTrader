

#ifndef TRANSLATIONMESSAGE_H
#define TRANSLATIONMESSAGE_H

#include <QDialog>
#include "ui_translationmessage.h"

class TranslationMessage : public QDialog
{
    Q_OBJECT

public:
    TranslationMessage();
    ~TranslationMessage();

private:
    Ui::TranslationMessage ui;

private slots:
    void on_createTranslationButton_clicked();
};

#endif // TRANSLATIONMESSAGE_H

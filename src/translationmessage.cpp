

#include "translationmessage.h"
#include "translationdialog.h"
#include "main.h"

TranslationMessage::TranslationMessage() : QDialog()
{
    ui.setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);

    julyTranslator.translateUi(this);
}

TranslationMessage::~TranslationMessage()
{
}

void TranslationMessage::on_createTranslationButton_clicked()
{
    this->close();

    TranslationDialog *translationDialog=new TranslationDialog;
    translationDialog->setWindowFlags(windowFlags());
    translationDialog->show();
}

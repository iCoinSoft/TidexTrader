

#include "addscriptwindow.h"
#include "ui_addscriptwindow.h"
#include "main.h"
#include <QFileDialog>
#include "scriptwidget.h"
#include <QMessageBox>

AddScriptWindow::AddScriptWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddScriptWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);

    Q_FOREACH(ScriptWidget* currentScript, mainWindow.ui.tabRules->findChildren<ScriptWidget*>())
        ui->existingScriptList->addItem(currentScript->windowTitle(),currentScript->property("FileName"));


    QString groupLabel=julyTr("SCRIPT_NAME","Script");
    QString newGroupName=groupLabel;

    int incr=0;
    QStringList existingGroups=mainWindow.getScriptGroupsNames();
    while(existingGroups.contains(newGroupName))
    {
        newGroupName=groupLabel;
        if(incr>0)newGroupName+=" "+QString::number(incr);
        incr++;
    }

    ui->scriptName->setText(newGroupName);


    ui->checkExistingScript->setEnabled(ui->existingScriptList->count());
    ui->existingScriptList->setEnabled(ui->existingScriptList->count());

    julyTranslator.translateUi(this);


    setWindowTitle(julyTranslator.translateButton("ADD_SCRIPT","Add script"));

    mainWindow.fixAllChildButtonsAndLabels(this);

    resize(width(),minimumSizeHint().height());
    setFixedHeight(height());
    onGroupContentChanged(true);

    bool haveTemplate=true;
    int templateId=1;
    while(haveTemplate)
    {
        QString templateIdStr=QString::number(templateId);
        if(templateIdStr.length()==1)templateIdStr.prepend("00");
        if(templateIdStr.length()==2)templateIdStr.prepend("0");
        QString currentRule=":/Resources/Templates/"+templateIdStr+".JLS";
        haveTemplate=QFile::exists(currentRule);
        if(haveTemplate)
        {
            templateId++;
            QSettings loadScript(currentRule,QSettings::IniFormat);
            QString currentName=loadScript.value("JLScript/Name").toString();
            if(currentName!="")ui->useScriptGroupTemplateList->addItem(currentName,currentRule);
        }
    }
    ui->checkUseTemplate->setEnabled(ui->useScriptGroupTemplateList->count());

    checkValidButton();
}

AddScriptWindow::~AddScriptWindow()
{
    delete ui;
}

QString AddScriptWindow::scriptFileName()
{
    if(ui->checkUseFile->isChecked())return ui->scriptFileName->text();
    return "";
}

void AddScriptWindow::on_openFile_clicked()
{
    QString lastRulesDir=mainWindow.iniSettings->value("UI/LastRulesPath",baseValues.desktopLocation).toString();
    if(!QFile::exists(lastRulesDir))lastRulesDir=baseValues.desktopLocation;

    QString fileName=QFileDialog::getOpenFileName(this, julyTr("OPEN_SCRIPT","Open Script"),lastRulesDir,"(*.JLS)");
    if(fileName.isEmpty())
    {
        ui->checkEmptyScript->setChecked(true);
        return;
    }
    QString scriptNameFile=QSettings(fileName,QSettings::IniFormat).value("JLScript/Name","").toString();
    {//Validate saved file
        if(scriptNameFile.isEmpty())
        {
        QMessageBox::warning(this,windowTitle(),julyTr("OPEN_INVALID_SCRIPT","Invalid script \"%1\"").arg(fileName));
        ui->checkEmptyScript->setChecked(true);
        return;
        }
    }

#ifdef Q_OS_WIN
    fileName.replace("/","\\");
#endif

    ui->scriptFileName->setText(fileName);
    mainWindow.iniSettings->setValue("UI/LastRulesPath",QFileInfo(fileName).dir().path());
    mainWindow.iniSettings->sync();

    ui->scriptName->setText(scriptNameFile);
}

void AddScriptWindow::onGroupContentChanged(bool on)
{
    if(!on)return;

    if(ui->checkExistingScript->isEnabled())ui->existingScriptList->setEnabled(ui->checkExistingScript->isChecked());
    if(ui->checkUseTemplate->isEnabled())ui->useScriptGroupTemplateList->setEnabled(ui->checkUseTemplate->isChecked());

    ui->scriptFileName->setEnabled(ui->checkUseFile->isChecked());
    ui->openFile->setEnabled(ui->checkUseFile->isChecked());

    if(ui->checkUseFile->isChecked())on_openFile_clicked();
    else ui->groupNameGroupbox->setEnabled(true);

    checkValidButton();
}

void AddScriptWindow::on_scriptName_textChanged(QString)
{
    checkValidButton();
}

QString AddScriptWindow::getCopyFromFile()
{
    if(ui->checkUseFile->isChecked())return ui->scriptFileName->text();
    return "";
}

void AddScriptWindow::on_buttonAddScript_clicked()
{
    scriptName=ui->scriptName->text();
    if(ui->checkExistingScript->isChecked())
        copyFromExistingScript=ui->existingScriptList->itemData(ui->existingScriptList->currentIndex()).toString();
    accept();
}

void AddScriptWindow::checkValidButton()
{
    bool isAbleToSave=ui->scriptName->text().length();
    if(ui->checkUseFile->isChecked())
        isAbleToSave=ui->scriptFileName->text().length()>3&&QFile::exists(ui->scriptFileName->text());


    ui->buttonAddScript->setEnabled(isAbleToSave);
}

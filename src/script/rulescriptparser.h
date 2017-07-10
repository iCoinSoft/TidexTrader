

#ifndef RULESCRIPTPARSER_H
#define RULESCRIPTPARSER_H

#include <QObject>
#include <QSettings>
#include "ruleholder.h"

class RuleScriptParser
{
public:
    explicit RuleScriptParser();
    static QString holderToScript(RuleHolder &holder, bool testMode=false);
    static bool writeHolderToFile(RuleHolder &holder, QString &file, QString section);
    static bool writeHolderToSettings(RuleHolder &holder, QSettings &settings, QString section);
    static RuleHolder readHolderFromFile(QString &data, QString section);
    static RuleHolder readHolderFromSettings(QSettings &settings, QString section);
signals:

public slots:

};

#endif // RULESCRIPTPARSER_H

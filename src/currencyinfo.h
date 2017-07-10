
#ifndef CURRENCYINFO_H
#define CURRENCYINFO_H

#include <QObject>

struct CurencyInfo
{
	CurencyInfo(QByteArray defS="$"){sign=defS;}
    QString name;
    QString sign;
    double valueStep;
    double valueSmall;
	bool isValid()
	{
		return !name.isEmpty()&&!sign.isEmpty();
	}
};

#endif // CURRENCYINFO_H

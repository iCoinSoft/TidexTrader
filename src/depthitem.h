
#ifndef DEPTHITEM_H
#define DEPTHITEM_H

#include <QString>

struct DepthItem 
{
	double price;
	double volume;
	QString priceStr;
	QString volumeStr;
	bool isValid();
};

#endif // DEPTHITEM_H

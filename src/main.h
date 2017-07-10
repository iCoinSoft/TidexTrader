

#ifndef MAIN_H
#define MAIN_H
#include "qtextstream.h"
#include <QFontMetrics>
#include "julytranslator.h"
#include "logthread.h"

#define textFontWidth(text) baseValues_->fontMetrics_->width(text)
#define debugLevel (baseValues_->debugLevel_)
#define appDataDir (baseValues_->appDataDir_)
#define grouped (baseValues_->groupPriceValue>0.0?2:0)
#define mainWindow (*baseValues_->mainWindow_)
#define logThread (baseValues_->logThread_)

#define julyTr baseValues_->julyTranslator_.translateString
#define julyTranslator baseValues_->julyTranslator_
#define currentExchange baseValues_->currentExchange_

#define defaultHeightForRow baseValues_->defaultHeightForRow_
#define upArrowStr baseValues_->upArrow
#define downArrowStr baseValues_->downArrow
#define upArrowNoUtfStr baseValues_->upArrowNoUtf8
#define downArrowNoUtfStr baseValues_->downArrowNoUtf8

#include "qtbitcointrader.h"

#define hmacSha512(key, baseString) QByteArray(reinterpret_cast<const char *>(HMAC(EVP_sha512(),key.constData(), key.size(), reinterpret_cast<const unsigned char *>(baseString.constData()), baseString.size(), 0, 0)),64)
#define hmacSha384(key, baseString) QByteArray(reinterpret_cast<const char *>(HMAC(EVP_sha384(),key.constData(), key.size(), reinterpret_cast<const unsigned char *>(baseString.constData()), baseString.size(), 0, 0)),48)
#define hmacSha256(key, baseString) QByteArray(reinterpret_cast<const char *>(HMAC(EVP_sha256(),key.constData(), key.size(), reinterpret_cast<const unsigned char *>(baseString.constData()), baseString.size(), 0, 0)),32)
#define hmacSha1(key, baseString) QByteArray(reinterpret_cast<const char *>(HMAC(EVP_sha1(),key.constData(), key.size(), reinterpret_cast<const unsigned char *>(baseString.constData()), baseString.size(), 0, 0)),20)

#include "currencyinfo.h"
#include "apptheme.h"

class Exchange;

#include "currencypairitem.h"

struct BaseValues
{
	void Construct();

	int scriptsThatUseOrderBookCount;

    QString osStyle;
	bool forceDotInSpinBoxes;

	int trafficSpeed;
	qint64 trafficTotal;
	int trafficTotalType;

	CurrencyPairItem currentPair;
    QMap<QString,CurrencyPairItem> currencyPairMap;

    int currentTheme;
	
	QByteArray upArrow;
	QByteArray downArrow;
	QByteArray upArrowNoUtf8;
	QByteArray downArrowNoUtf8;

	QString customUserAgent;
	QString customCookies;
	bool gzipEnabled;
    AppTheme appThemeLight;
    AppTheme appThemeDark;
    AppTheme appThemeGray;
	AppTheme appTheme;
	int debugLevel_;//0: Disabled; 1: Debug; 2: Log
	bool supportsUtfUI;
	bool highResolutionDisplay;
	int defaultHeightForRow_;
    double groupPriceValue;
	QFontMetrics *fontMetrics_;
	int apiDownCount;
	int uiUpdateInterval;
	QByteArray depthCountLimitStr;
	int depthCountLimit;
	int httpRetryCount;
	int httpRequestInterval;
	int httpRequestTimeout;
	Exchange *currentExchange_;
    QString scriptFolder;
    QString themeFolder;
    double jlScriptVersion;
	QString exchangeName;
	QString timeFormat;
	QString dateTimeFormat;
    bool use24HourTimeFormat;
	QString defaultLangFile;
	JulyTranslator julyTranslator_;
	QString appDataDir_;
	QByteArray appVerStr;
	LogThread *logThread_;
	QByteArray restKey;
	QByteArray restSign;
	QByteArray randomPassword;
	QtBitcoinTrader *mainWindow_;
	QString logFileName;
	QString iniFileName;
    QString desktopLocation;
    QString tempLocation;
    double appVerReal;
    double appVerLastReal;
	bool appVerIsBeta;
    QMap<QString,QString> currencyMapSign;
    QMap<QString,CurencyInfo> currencyMap;

    int decimalsAmountMyTransactions;
    int decimalsPriceMyTransactions;
    int decimalsTotalMyTransactions;
    int decimalsAmountOrderBook;
    int decimalsPriceOrderBook;
    int decimalsTotalOrderBook;
    int decimalsAmountLastTrades;
    int decimalsPriceLastTrades;
    int decimalsTotalLastTrades;

    bool alternativeDomainBTCe;
};

#define baseValues (*baseValues_)
extern BaseValues *baseValues_;

#endif // MAIN_H

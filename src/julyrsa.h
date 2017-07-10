

#ifndef JULYRSA_H
#define JULYRSA_H

#include <QObject>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

class JulyRSA : public QObject
{
	Q_OBJECT

public:
	static QByteArray getSignature(QByteArray sign, QByteArray key);
};

#endif // JULYRSA_H

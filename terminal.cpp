#include "terminal.h"
#include "json.h"

Terminal::Terminal(Cardreader *cardreader,
									 PinDialog *pinDialog,
									 const QString &id,
									 const QString &secret,
									 const QString &url,
									 const QString &CApath,
									 QObject *parent) :
	QObject(parent)
{
	m_cardreader = cardreader;
	m_pinDialog = pinDialog;
	
	m_id = id;
	m_secret = secret;
	
	m_request = new QNetworkRequest(QUrl(url));
	m_request->setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	m_request->setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	
	QList<QSslCertificate> CAs;
	QFile *CAfile = new QFile(CApath);
	if (CAfile->exists() && CAfile->open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QSslCertificate CA(CAfile);
		if (CA.isValid())
		{
			CAs.append(CA);
			QSslConfiguration ssl;
			ssl.setCaCertificates(CAs);
#ifdef DEBUG
			ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
#endif
			m_request->setSslConfiguration(ssl);
		}
		CAfile->close();
		delete CAfile;
	}
	
	m_postData = new PostData(id, secret);
	
	connect(pinDialog, SIGNAL(accepted()), SLOT(sessionStart()));
}

Terminal::~Terminal()
{
	delete m_request;
	delete m_postData;
}

void Terminal::request()
{
	m_error = QNetworkReply::NoError;
	QNetworkReply *reply = m_https.post(*m_request, m_postData->content());
	connect(reply, SIGNAL(finished()), SLOT(readReply()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(networkError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
}

void Terminal::sessionStart()
{
	m_postData->setClient(m_cardreader->cardnum(), m_pinDialog->pin());
	m_postData->setAction("session", "start");
	request();
}

void Terminal::readReply()
{
	QNetworkReply *reply = (QNetworkReply *)sender();
	QByteArray data = reply->readAll();
	reply->deleteLater();
	
	if (m_error != QNetworkReply::NoError)
		return;
	
	qDebug() << "JJ : " << data;
	QVariant response = JSON::decode(data, JSON::JSONObject);
	qDebug() << "DD : " << response;
}

void Terminal::networkError(QNetworkReply::NetworkError error)
{
#ifdef DEBUG
	qDebug() << "EE Network error = " << error;
#endif
	m_error = error;
}

void Terminal::sslErrors(QList<QSslError> errors)
{
#ifdef DEBUG
	QList<QSslError>::iterator i;
	for (i = errors.begin(); i != errors.end(); i++)
		qDebug() << "EE SSL error = " << *i;
#endif
}

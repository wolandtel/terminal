#include "terminal.h"

Terminal::Terminal(Cardreader *cardreader,
									 PinDialog *pinDialog,
									 const QString &id,
									 const QString &secret,
									 const QString &url,
									 const QString &CAfile,
									 QObject *parent) :
	QObject(parent)
{
	m_cardreader = cardreader;
	m_pinDialog = pinDialog;
	
	m_id = id;
	m_secret = secret;
	
	QList<QSslCertificate> CAs;
	CAs.append(QSslCertificate(new QFile(CAfile)));
	
	QSslConfiguration ssl;
	ssl.setCaCertificates(CAs);
	
	m_request = new QNetworkRequest(QUrl(url));
	m_request->setSslConfiguration(ssl);
	m_request->setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	m_request->setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	
	connect(pinDialog, SIGNAL(gotPin()), SLOT(sessionStart()));
}

Terminal::~Terminal()
{
	delete m_request;
}

void Terminal::sessionStart()
{
	//QNetworkReply *reply = m_https.post(*m_request);
}

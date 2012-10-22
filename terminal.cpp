#include "terminal.h"
#include "json.h"
#include "debug.h"

Terminal::Terminal(const JConfig &conf, QObject *parent) :
	QObject(parent)
{
	m_mainWindow = new MainWindow();
	m_mainWindow->setWindowState(Qt::WindowFullScreen);
	m_mainWindow->show();
	
	m_cardreader = new Cardreader(conf["cardreader"]["device"].toString());
	m_pinDialog	= new PinDialog(conf, m_mainWindow);
	QString currency = conf["global"]["currency"].toString();
	m_balanceDialog = new BalanceDialog(currency, m_mainWindow);
	m_paymentDialog = new PaymentDialog(currency, m_mainWindow);
	m_ejectDialog = new EjectDialog(conf, m_mainWindow);
	
	m_id = conf["terminal"]["id"].toString();
	m_secret = conf["terminal"]["secret"].toString();
	
	m_request = new QNetworkRequest(QUrl(conf["network"]["url"].toString()));
	m_request->setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	m_request->setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	
	QList<QSslCertificate> CAs;
	QFile *CAfile = new QFile(conf["network"]["CA"].toString());
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
	
	m_postData = new PostData(m_id, m_secret);
	
	connect(m_cardreader, SIGNAL(initSucceeded()), m_mainWindow, SLOT(displayReady()));
	connect(m_cardreader, SIGNAL(initFailed()), m_mainWindow, SLOT(displayError()));
	connect(m_cardreader, SIGNAL(cardEject(bool)), m_mainWindow, SLOT(ejectCard(bool)));
	connect(m_cardreader, SIGNAL(cardEjected()), m_mainWindow, SLOT(displayReady()));
	connect(m_cardreader, SIGNAL(cardInserted()), m_pinDialog, SLOT(open()));
	
	connect(m_pinDialog, SIGNAL(rejected()), m_cardreader, SLOT(ejectCard()));
	connect(m_pinDialog, SIGNAL(gotPin()), SLOT(sessionStart()));
	
	connect(this, SIGNAL(sessionStarted(double)), m_pinDialog, SLOT(accept()));
	connect(this, SIGNAL(sessionStarted(double)), m_balanceDialog, SLOT(open(double)));
	connect(this, SIGNAL(sessionStopped()), m_cardreader, SLOT(ejectCard()));
	connect(this, SIGNAL(sessionStartFailed(NetworkError)), m_pinDialog, SLOT(refresh()));
	
	connect(m_balanceDialog, SIGNAL(rejected()), SLOT(sessionStop()));
	connect(m_balanceDialog, SIGNAL(rejected()), m_mainWindow, SLOT(displayWait()));
	connect(m_balanceDialog, SIGNAL(payment()), m_paymentDialog, SLOT(open()));
	connect(m_balanceDialog, SIGNAL(eject()), m_ejectDialog, SLOT(open()));
	
	connect(m_paymentDialog, SIGNAL(credit(int)), SLOT(balance(int)));
	
#ifdef DEBUG
	connect(m_mainWindow, SIGNAL(debugDialog()), m_ejectDialog, SLOT(open()));
#endif
	
	m_cardreader->init();
}

Terminal::~Terminal()
{
	delete m_postData;
	delete m_request;
	delete m_balanceDialog;
	delete m_pinDialog;
	delete m_cardreader;
	delete m_mainWindow;
}

void Terminal::request()
{
	m_error = QNetworkReply::NoError;
	m_request->setAttribute(TERM_RA_POSTDATA, qVariantFromValue(*m_postData));
	QNetworkReply *reply = m_https.post(*m_request, m_postData->content());
	connect(reply, SIGNAL(finished()), SLOT(readReply()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(networkError(QNetworkReply::NetworkError)));
	connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
}

void Terminal::operation(int opcode, QString value, int subcode, int errcode)
{
	m_postData->setAction("operation");
	m_postData->setParam("session_id", m_session);
	m_postData->setParam("opcode_id", opcode);
	if (subcode > 0)
		m_postData->setParam("subcode", subcode);
	m_postData->setParam("errcode_id", errcode);
	m_postData->setParam("value", value);
	request();
}

void Terminal::sessionStart()
{
	m_postData->setClient(m_cardreader->cardnum(), m_pinDialog->pin());
	m_postData->setAction("session", "start");
	request();
}

void Terminal::sessionStop()
{
	m_postData->setAction("session", "stop");
	m_postData->setParam("id", m_session);
	m_postData->setParam("balance", m_balance);
	request();
}

void Terminal::readReply()
{
	QNetworkReply *reply = (QNetworkReply *)sender();
	QByteArray data = reply->readAll();
	const PostData postData = reply->request().attribute(TERM_RA_POSTDATA).value<PostData>();
#ifdef DEBUG
	qDebug() << " i RR : " << postData.dump();
#endif
	const QString &action = postData["action"]["type"].toString();
	const QString &modifier = postData["action"]["modifier"].toString();
	reply->deleteLater();
	
	if (m_error != QNetworkReply::NoError)
	{
		if (action == "session")
		{
			if (modifier == "start")
				emit sessionStartFailed(ConnectionFailed);
			else if (modifier == "stop")
				emit sessionStopFailed(ConnectionFailed);
		}
		return;
	}
	
	Json response = Json(data, Json::InputEncoded); // FIX: обработать ошибку
#ifdef DEBUG
	qDebug() << "NN << " << response.dump();
#endif
	
	int code = response["code"].toInt();
	if (code != 0)
	{
		if (action == "session")
		{
			if (modifier == "start")
				emit sessionStartFailed(OtherError);
		}
		return;
	}
	
	if (action == "session")
	{
		if (modifier == "start")
		{
			m_session = response["response"]["session"]["id"].toString();
			m_balance = response["response"]["session"]["balance"].toNumber();
			emit sessionStarted(m_balance);
		}
		else if (modifier == "stop")
			emit sessionStopped();
	}
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
#else
	(void)errors;
#endif
}

void Terminal::balance(int amount)
{
	if (amount == 0)
		return;
	
	if (m_balance + amount < 0)
	{
		operation(1, QString::number(amount), 2, 0); // FIX: set error code
		return;
	}
	
	m_balance += amount;
	m_balanceDialog->setBalance(m_balance);
	operation(1, QString::number(amount), (amount > 0 ? 1 : 2));
}

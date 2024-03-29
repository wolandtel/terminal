#include "terminal.h"
#include "json.h"
#include "debug.h"

Terminal::Terminal(const JConfig &conf, Ipc &ipc, QObject *parent) :
	QObject(parent),
	m_ipc(ipc)
{
	m_mainWindow = new MainWindow();
	m_mainWindow->setWindowState(Qt::WindowFullScreen);
	m_mainWindow->show();
	
	m_cardreader = new Cardreader(conf["cardreader"]["device"].toString());
	m_pinDialog	= new PinDialog(conf, m_mainWindow);
	m_balanceDialog = new BalanceDialog(conf["global"]["currency"].toString(DEF_CURRENCY), m_mainWindow);
	m_paymentDialog = new PaymentDialog(conf, m_mainWindow);
	m_ejectDialog = new EjectDialog(conf, m_mainWindow);
	m_printer = new Printer(conf, this);
	
	m_id = conf["terminal"]["id"].toString();
	m_secret = conf["terminal"]["secret"].toString();
	
	m_request = new QNetworkRequest(QUrl(conf["network"]["url"].toString()));
	m_request->setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	m_request->setAttribute(QNetworkRequest::CacheSaveControlAttribute, false);
	
	QList<QSslCertificate> CAs;
	QFile CAfile(conf["network"]["CA"].toString(TERM_DEF_CA));
	if (CAfile.exists() && CAfile.open(QIODevice::ReadOnly))
	{
		QSslCertificate CA(&CAfile);
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
		CAfile.close();
	}
	
	m_postData = new PostData(m_id, m_secret);
	
	connect(m_cardreader, SIGNAL(initSucceeded()), m_mainWindow, SLOT(displayReady()));
	connect(m_cardreader, SIGNAL(initFailed()), m_mainWindow, SLOT(displayError()));
	connect(m_cardreader, SIGNAL(cardEject(bool)), m_mainWindow, SLOT(ejectCard(bool)));
	connect(m_cardreader, SIGNAL(cardEjected()), m_mainWindow, SLOT(displayReady()));
	connect(m_cardreader, SIGNAL(cardInserted()), SLOT(checkClient()));
	
	connect(m_pinDialog, SIGNAL(rejected()), m_cardreader, SLOT(ejectCard()));
	connect(m_pinDialog, SIGNAL(gotPin()), SLOT(sessionStart()));
	
	connect(this, SIGNAL(clientCheckFailed(NetworkError)), m_mainWindow, SLOT(ejectCardError()));
	connect(this, SIGNAL(clientCheckFailed(NetworkError)), m_cardreader, SLOT(ejectCardError()));
	connect(this, SIGNAL(sessionStarted(double)), m_pinDialog, SLOT(accept()));
	connect(this, SIGNAL(sessionStarted(double)), m_balanceDialog, SLOT(open(double)));
	connect(this, SIGNAL(sessionStopped()), m_cardreader, SLOT(ejectCard()));
	connect(this, SIGNAL(sessionStartFailed(NetworkError)), m_pinDialog, SLOT(refresh()));
	
	connect(m_balanceDialog, SIGNAL(rejected()), SLOT(sessionStop()));
	connect(m_balanceDialog, SIGNAL(rejected()), m_mainWindow, SLOT(displayWait()));
	connect(m_balanceDialog, SIGNAL(payment()), m_paymentDialog, SLOT(open()));
	connect(m_balanceDialog, SIGNAL(eject(int)), m_ejectDialog, SLOT(open(int)));
	
	connect(m_paymentDialog, SIGNAL(credit(int)), SLOT(changeBalance(int)));
	connect(m_paymentDialog, SIGNAL(credit(int)), m_printer, SLOT(payment(int)));

	connect(m_ejectDialog, SIGNAL(eject(int)), SLOT(changeBalance(int)));
	connect(m_ejectDialog, SIGNAL(eject(int)), m_printer, SLOT(ejected(int)));
	
#ifdef DEBUG
	connect(m_mainWindow, SIGNAL(debugDialog()), m_ejectDialog, SLOT(open()));
	
	dbg << ". Let's go!";
#endif
	
	m_cardreader->init();
}

Terminal::~Terminal()
{
}

void Terminal::shutdown()
{
#ifdef DEBUG
	dbg << ". Winter is coming…";
#endif
	delete m_postData;
	delete m_request;
	delete m_printer;
	delete m_ejectDialog;
	delete m_paymentDialog;
	delete m_balanceDialog;
	delete m_pinDialog;
	delete m_cardreader;
	delete m_mainWindow;
	m_ipc.disable();
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

void Terminal::checkClient()
{
	m_postData->setClient(m_cardreader->cardnum());
	m_postData->setAction("check", "client");
	request();
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

void Terminal::actionFailed(const QString &action, const QString &modifier, NetworkError error)
{
	if (action == "session")
	{
		if (modifier == "start")
			emit sessionStartFailed(error);
		else if (modifier == "stop")
			emit sessionStopFailed(error);
	}
	else if (action == "check")
	{
		if (modifier == "client")
			emit clientCheckFailed(error);
	}
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
	dbg << " i RR : " << postData.dump();
#endif
	const QString &action = postData["action"]["type"].toString();
	const QString &modifier = postData["action"]["modifier"].toString();
	reply->deleteLater();
	
	if (m_error != QNetworkReply::NoError)
	{
		actionFailed(action, modifier, ConnectionFailed);
		return;
	}
	
	Json response = Json(data, Json::InputEncoded); // FIX: обработать ошибку
#ifdef DEBUG
	dbg << "NN << " << response.dump();
#endif
	
	int code = response["code"].toInt();
	if (code != 0)
	{
		actionFailed(action, modifier);
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
	else if (action == "check")
	{
		if (modifier == "client")
			m_pinDialog->open();
	}
}

void Terminal::networkError(QNetworkReply::NetworkError error)
{
#ifdef DEBUG
	dbg << "EE Network error = " << error;
#endif
	m_error = error;
}

void Terminal::sslErrors(QList<QSslError> errors)
{
#ifdef DEBUG
	QList<QSslError>::iterator i;
	for (i = errors.begin(); i != errors.end(); i++)
		dbg << "EE SSL error = " << *i;
#else
	(void)errors;
#endif
}

void Terminal::changeBalance(int amount)
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

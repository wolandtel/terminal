#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "mainwindow.h"
#include "cardreader.h"
#include "pindialog.h"
#include "balancedialog.h"
#include "paymentdialog.h"
#include "ejectdialog.h"
#include "postdata.h"
#include "jconfig.h"

#define TERM_DEF_CA QApplication::applicationDirPath() + "/CA.pem"

#define TERM_RA_POSTDATA QNetworkRequest::User

class Terminal : public QObject
{
		Q_OBJECT
	public:
		enum NetworkError
		{
			ConnectionFailed,
			LoginFailed,
			OtherError
		};
		
		Terminal(const JConfig &conf, QObject *parent = 0);
		~Terminal();
		
	signals:
		void sessionStartFailed(enum NetworkError error);
		void sessionStopFailed(enum NetworkError error);
		void sessionStarted(double);
		void sessionStopped();
		
	public slots:
		
	private:
		MainWindow *m_mainWindow;
		Cardreader *m_cardreader;
		PinDialog *m_pinDialog;
		BalanceDialog *m_balanceDialog;
		PaymentDialog *m_paymentDialog;
		EjectDialog *m_ejectDialog;
		QString m_id, m_secret;
		QNetworkRequest *m_request;
		QNetworkAccessManager m_https;
		PostData *m_postData;
		QNetworkReply::NetworkError m_error;
		QString m_session;
		double m_balance;
		
		void request();
		void operation(int opcode, QString value, int subcode = 0, int errcode = 0);
		
	private slots:
		void sessionStart();
		void sessionStop();
		void readReply();
		void networkError(QNetworkReply::NetworkError error);
		void sslErrors(QList<QSslError> errors);
		void balance(int amount);
};

#endif // TERMINAL_H

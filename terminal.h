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
#include "printer.h"
#include "ipc.h"

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
		
		Terminal(const JConfig &conf, Ipc &ipc, QObject *parent = 0);
		~Terminal();
		inline QString &id() { return m_id; }
		inline Cardreader *cardreader() { return m_cardreader; }
		inline double balance() { return m_balance; }
		
	signals:
		void clientCheckFailed(enum NetworkError error);
		void sessionStartFailed(enum NetworkError error);
		void sessionStopFailed(enum NetworkError error);
		void sessionStarted(double);
		void sessionStopped();
		
	public slots:
		void shutdown();
		
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
		Printer *m_printer;
		Ipc &m_ipc;
		
		void request();
		void operation(int opcode, QString value, int subcode = 0, int errcode = 0);
		void actionFailed(const QString &action, const QString &modifier, NetworkError error = OtherError);
		
	private slots:
		void checkClient();
		void sessionStart();
		void sessionStop();
		void readReply();
		void networkError(QNetworkReply::NetworkError error);
		void sslErrors(QList<QSslError> errors);
		void changeBalance(int amount);
};

#endif // TERMINAL_H

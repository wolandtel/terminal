#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "mainwindow.h"
#include "cardreader.h"
#include "pindialog.h"
#include "balancedialog.h"
#include "postdata.h"
#include "jconfig.h"

#define RA_POSTDATA QNetworkRequest::User

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
		QString m_id, m_secret;
		QNetworkRequest *m_request;
		QNetworkAccessManager m_https;
		PostData *m_postData;
		QNetworkReply::NetworkError m_error;
		QString m_session;
		double m_balance;
		
		void request();
		
	private slots:
		void sessionStart();
		void readReply();
		void networkError(QNetworkReply::NetworkError error);
		void sslErrors(QList<QSslError> errors);
};

#endif // TERMINAL_H

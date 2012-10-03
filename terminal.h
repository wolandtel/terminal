#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "cardreader.h"
#include "pindialog.h"
#include "postdata.h"

class Terminal : public QObject
{
		Q_OBJECT
	public:
		explicit Terminal(Cardreader *cardreader,
											PinDialog *pinDialog,
											const QString &id,
											const QString &secret,
											const QString &url,
											const QString &CApath,
											QObject *parent = 0);
		~Terminal();
		
	signals:
		
	public slots:
		
	private:
		Cardreader *m_cardreader;
		PinDialog *m_pinDialog;
		QString m_id, m_secret;
		QNetworkRequest *m_request;
		QNetworkAccessManager m_https;
		PostData *m_postData;
		QNetworkReply::NetworkError m_error;
		
		void request();
		
	private slots:
		void sessionStart();
		void readReply();
		void networkError(QNetworkReply::NetworkError error);
		void sslErrors(QList<QSslError> errors);
};

#endif // NETCLIENT_H

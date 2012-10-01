#ifndef TERMINAL_H
#define TERMINAL_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include "cardreader.h"
#include "pindialog.h"

class Terminal : public QObject
{
		Q_OBJECT
	public:
		explicit Terminal(Cardreader *cardreader,
											PinDialog *pinDialog,
											const QString &id,
											const QString &secret,
											const QString &url,
											const QString &CAfile,
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
		
	private slots:
		void sessionStart();
};

#endif // NETCLIENT_H

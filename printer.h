#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextCodec>
#include "jconfig.h"

#define PRN_DEF_TPL_EJECT QApplication::applicationDirPath() + "/templates/eject.tpl"
#define PRN_DEF_TPL_PAYMENT QApplication::applicationDirPath() + "/templates/payment.tpl"

class Terminal;

class Printer : public QObject
{
		Q_OBJECT
	public:
		Printer(const JConfig &conf, Terminal *terminal);
		~Printer();
	
	private:
		QFile *m_device;
		QString m_tplEject, m_tplPayment, m_formatDate, m_formatTime;
		Terminal *m_terminal;
		QTextCodec *m_codec;
		
		void print(QString receipt, int amount);
		void tplLoad(const QString &file, QString &tpl);
	
	private slots:
		void ejected(int amount);
		void payment(int amount);
};

#endif // PRINTER_H

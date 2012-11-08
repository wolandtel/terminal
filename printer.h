#ifndef PRINTER_H
#define PRINTER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextCodec>
#include "jconfig.h"

#define PRN_DEF_EJECT_TPL QApplication::applicationDirPath() + "/templates/eject.tpl"
#define PRN_DEF_PAYMENT_TPL QApplication::applicationDirPath() + "/templates/payment.tpl"

class Terminal;

class Printer : public QObject
{
		Q_OBJECT
	public:
		Printer(const JConfig &conf, Terminal *terminal);
		~Printer();
	
	private:
		QFile *m_device;
		QString m_ejectTpl, m_paymentTpl;
		Terminal *m_terminal;
		QTextCodec *m_codec;
		
		void print(QString receipt, int amount);
		void loadTpl(const QString &file, QString &tpl);
	
	private slots:
		void ejected(int amount);
		void payment(int amount);
};

#endif // PRINTER_H

#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>
#include "jconfig.h"

#define PD_HELPER_START_TIMEOUT 10000 // msec
#define PD_HELPER_STOP_TIMEOUT 10000 // msec
#define PD_ACCEPTOR_DEF_BAUDRATE 9600
#define PD_ACCEPTOR_DEF_TIMEOUT 60

namespace Ui {
	class PaymentDialog;
}

class PaymentDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit PaymentDialog(const JConfig &conf, QWidget *parent = 0);
		~PaymentDialog();
		
	signals:
		void credit(int amount);
		
	private:
		enum HelperState
		{
			HelperCreated = 0x1,
			HelperStarting = 0x2,
			HelperRunning = 0x4,
			HelperTerminating = 0x8,
			HelperFinished = 0x10
		} m_helperState;
		
		Ui::PaymentDialog *ui;
		QString m_currency;
		int m_paid;
		QString m_helperFile;
		QStringList m_helperParams;
		QProcess *m_helper;
		
		void paid(int amount);
	
	private slots:
		void open();
		void close();
		void helperRead();
		void helperStarted();
		void helperFinished();
};

#endif // PAYMENTDIALOG_H

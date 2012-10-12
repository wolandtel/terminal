#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>

namespace Ui {
	class PaymentDialog;
}

class PaymentDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit PaymentDialog(const QString &currency = QString::fromUtf8("руб."), QWidget *parent = 0);
		~PaymentDialog();
		
	signals:
		void credit(int amount);
		
	private:
		Ui::PaymentDialog *ui;
		QString m_currency;
		int m_paid;
	
	private slots:
		void open();
		void paid(int amount);
		void accept();
};

#endif // PAYMENTDIALOG_H

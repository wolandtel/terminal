#ifndef BALANCEDIALOG_H
#define BALANCEDIALOG_H

#include <QDialog>
#include "config.h"

namespace Ui {
	class BalanceDialog;
}

class BalanceDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit BalanceDialog(const QString &currency = DEF_CURRENCY, QWidget *parent = 0);
		~BalanceDialog();
		
		void setBalance(double balance = 0);
	
	signals:
		void payment();
		void eject(int balance);
		
	private slots:
		void on_ejectPb_clicked();
		
		void open(double balance = 0);
		
	private:
		Ui::BalanceDialog *ui;
		QString m_currency;
		double m_balance;
};

#endif // BALANCEDIALOG_H

#ifndef BALANCEDIALOG_H
#define BALANCEDIALOG_H

#include <QDialog>

namespace Ui {
	class BalanceDialog;
}

class BalanceDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit BalanceDialog(QWidget *parent = 0);
		~BalanceDialog();
	
	signals:
		void payment();
		void eject();
		
	private slots:
		void on_paymentPb_clicked();
		void on_ejectPb_clicked();
		
		void open(double balance = 0);
		
	private:
		Ui::BalanceDialog *ui;
		QString m_currency;
};

#endif // BALANCEDIALOG_H

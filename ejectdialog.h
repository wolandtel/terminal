#ifndef EJECTDIALOG_H
#define EJECTDIALOG_H

#include <QDialog>
#include "jconfig.h"

#define EJECT_DEF_MIN 100
#define EJECT_DEF_MAX 15000
#define EJECT_DEF_LEN 5

namespace Ui {
	class EjectDialog;
}

class EjectDialog : public QDialog
{
		Q_OBJECT
		
	public:
		explicit EjectDialog(const JConfig &conf, QWidget *parent = 0);
		~EjectDialog();
	
	signals:
		void eject(int amount);
	
	private:
		Ui::EjectDialog *ui;
		int m_min, m_max, m_len;
		QString m_amount;
		
		void amountChanged();
		
	private slots:
		void open();
		void on_kpCancelPb_clicked();
		void on_kpBsPb_clicked();
		void num();
		void refresh();
		void on_ejectPb_clicked();
};

#endif // EJECTDIALOG_H

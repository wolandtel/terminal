#ifndef EJECTDIALOG_H
#define EJECTDIALOG_H

#include <QDialog>
#include "jconfig.h"

#define EJECT_DEF_MIN 100
#define EJECT_DEF_MAX 15000
#define EJECT_DEF_LEN 5
#define EJECT_DEF_CASSETTES QApplication::applicationDirPath() + "/cassettes.conf"

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
		int m_min, m_max, m_len, m_balance;
		QString m_amount;
		QString m_helperFile;
		QStringList m_helperParams;
		JConfig m_helperCassettes;
		QProcess *m_helper;
		
		void amountChanged();
		
	private slots:
		void open(int balance);
		void on_kpCancelPb_clicked();
		void on_kpBsPb_clicked();
		void num();
		void refresh(int amount = 0);
		void on_ejectPb_clicked();
		void helperRead();
};

#endif // EJECTDIALOG_H

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QCommandLine>

#define OPTS_DEF_CONFIG QApplication::applicationDirPath() + "/terminal.conf"

class Options : public QObject
{
		Q_OBJECT
	public:
		Options(QObject *parent = 0);
		~Options();
		
		inline QString config() { return m_config; }
		inline QString command() { return m_command; }
		void showHelp(bool exit = true, int returnCode = 0);
	
	private:
		QCommandLine *m_cmdline;
		
		QString m_config;
		QString m_command;
	
	private slots:
		void switchFound(const QString &name);
		void optionFound(const QString &name, const QVariant &value);
		void paramFound(const QString &name, const QVariant &value);
		void parseError(const QString &error);
};

#endif // OPTIONS_H

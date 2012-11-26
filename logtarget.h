#ifndef LOGTARGET_H
#define LOGTARGET_H

#include "logger.h"

class LogTarget
{
	friend class Logger;
	
	public:
		enum Type
		{
			TypeNone,
			TypeErr,
			TypeFile,
			TypeSyslog,
			TypePipe
		};
		
		~LogTarget();
	
	protected:
		LogTarget(const QString &type, const QString &param = QString());
		
		void send(const QString &source, Logger::Level level, const QString &msg);
	
	private:
		enum Type m_type;
		QString m_param;
		QFile *m_file;
};

#endif // LOGTARGET_H

#include "logtarget.h"

LogTarget::LogTarget(const QString &type/* err, syslog, file, pipe */, const QString &param) :
	m_type(TypeNone)
{
	if (type == "err")
	{
		m_file = new QFile();
		if(m_file->open(2, QIODevice::Text | QIODevice::Unbuffered | QIODevice::WriteOnly))
			m_type = TypeErr;
		else
			delete m_file;
	}
	else if (type == "file")
	{
		m_file = new QFile(param);
		if(m_file->open(QIODevice::Text | QIODevice::Unbuffered | QIODevice::ReadWrite | QIODevice::Append))
			m_type = TypeFile;
		else
			delete m_file;
	}
	else if (type == "pipe")
	{
		m_file = new QFile(param);
		if(m_file->open(QIODevice::Text | QIODevice::Unbuffered | QIODevice::WriteOnly | QIODevice::Truncate))
			m_type = TypePipe;
		else
			delete m_file;
	}
}

LogTarget::~LogTarget()
{
	switch (m_type)
	{
		case TypeErr:
		case TypeFile:
		case TypePipe:
			m_file->close();
			delete m_file;
			break;
		default:;
	}
}

void LogTarget::send(const QString &source, Logger::Level level, const QString &msg)
{
	switch (m_type)
	{
		case TypeErr:
		case TypeFile:
		case TypePipe:
			m_file->write(/* TODO: source, level*/(QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz] ") + msg + "\n").toUtf8());
			break;
		default:;
	}
}

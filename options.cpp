#include <QApplication>
#include <QVariant>
#include "options.h"
#include "debug.h"

Options::Options(QObject *parent) :
	QObject(parent),
	m_config(OPTS_DEF_CONFIG)
{
	QCommandLineConfig clconf;
	
	clconf << (QCommandLineConfigEntry){ QCommandLine::Option, 'c', "config", "Path to the config file", QCommandLine::Optional }
			<< (QCommandLineConfigEntry){ QCommandLine::Param, NULL, "command", "Send command to the running process", QCommandLine::Optional };
	
	m_cmdline = new QCommandLine(*QApplication::instance(), clconf);
	m_cmdline->enableVersion(true);
	m_cmdline->enableHelp(true);
	
	connect(m_cmdline, SIGNAL(switchFound(const QString &)), SLOT(switchFound(const QString &)));
	connect(m_cmdline, SIGNAL(optionFound(const QString &, const QVariant &)), SLOT(optionFound(const QString &, const QVariant &)));
	connect(m_cmdline, SIGNAL(paramFound(const QString &, const QVariant &)), SLOT(paramFound(const QString &, const QVariant &)));
	connect(m_cmdline, SIGNAL(parseError(const QString &)), SLOT(parseError(const QString &)));
	
	m_cmdline->parse();
	
}

Options::~Options()
{
	delete m_cmdline;
}

void Options::showHelp(bool exit, int returnCode)
{
	m_cmdline->showHelp(exit, returnCode);
}

void Options::switchFound(const QString &name)
{
	dbg << name;
}

void Options::optionFound(const QString &name, const QVariant &value)
{
	if (name == "config")
		m_config = value.toString();
}

void Options::paramFound(const QString &name, const QVariant &value)
{
	if (name == "command")
		m_command = value.toString();
}

void Options::parseError(const QString &error)
{
	dbg << error;
	m_cmdline->showHelp(true, -1);
	QApplication::quit();
}

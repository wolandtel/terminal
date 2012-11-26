#include "logger.h"
#include "logtarget.cpp"

Logger *Logger::m_instance = NULL;

Logger *Logger::instance(const JConfig &conf)
{
	if (conf.isNull())
	{
		if (m_instance)
			return m_instance;
		return m_instance = new Logger();
	}
	
	if (m_instance)
		delete m_instance;
	
	return m_instance = new Logger(conf);
}

void Logger::clear()
{
	if (!m_instance)
		return;
	
	delete m_instance;
	m_instance = NULL;
}

Logger::Logger()
{
	m_targets.insert("stderr", new LogTarget("err"));
	m_routes.insert("default", LogRoutes());
	m_routes["default"].insert(LevelDefault, m_targets["stderr"]);
}

Logger::Logger(const JConfig &conf)
{
	const JsonObject &ctgs = conf["logger"]["targets"].toObject();
	JsonObject::const_iterator ti;
	for (ti = ctgs.constBegin(); ti != ctgs.constEnd(); ti++)
	{
		const JConfig &t = ti.value();
		
		QString type = t["type"].toString(), param;
		if ((type == "file") || (type == "pipe"))
			param = t["path"].toPath();
		else if (type == "syslog")
			param = t["facility"].toString();
		
		m_targets.insert(ti.key(), new LogTarget(type, param));
	}
	
	const JsonArray &crts = conf["logger"]["routes"].toArray();
	JsonArray::const_iterator ri;
	for (ri = crts.constBegin(); ri != crts.constEnd(); ri++)
	{
		const Json &r = *ri;
		addRoute(r["source"], r["level"], r["target"]);
	}
}

Logger::~Logger()
{
	LogTargetMap::iterator i;
	for (i = m_targets.begin(); i != m_targets.end(); i++)
		delete i.value();
}

void Logger::addRoute(const Json &source, const Json &level, const Json &target)
{
	if (source.isArray())
	{
		const JsonArray &s = source.toArray();
		JsonArray::const_iterator i;
		for (i = s.constBegin(); i != s.constEnd(); i++)
			addRoute(i->toString(), level, target);
	}
	else
		addRoute(source.toString(), level, target);
}

void Logger::addRoute(const QString &source, const Json &level, const Json &target)
{
	if (level.isArray())
	{
		const JsonArray &l = level.toArray();
		JsonArray::const_iterator i;
		for (i = l.constBegin(); i != l.constEnd(); i++)
			addRoute(source, stringToLevel(i->toString()), target);
	}
	else
		addRoute(source, stringToLevel(level.toString()), target);
}

void Logger::addRoute(const QString &source, enum Level level, const Json &target)
{
	if (level == LevelNone)
		return;
	
	if (target.isArray())
	{
		const JsonArray &t = target.toArray();
		JsonArray::const_iterator i;
		for (i = t.constBegin(); i != t.constEnd(); i++)
			addRoute(source, level, i->toString());
	}
	else
		addRoute(source, level, target.toString());
}

void Logger::addRoute(const QString &source, enum Level level, const QString &target)
{
	if (!m_targets.contains(target))
		return;
	
	if (!m_routes.contains(source))
		m_routes.insert(source, LogRoutes());
	
	m_routes[source].insertMulti(level, m_targets[target]);
}

void Logger::debug(const QString &source, const QString &msg)
{
	return log(source, LevelDebug, msg);
}

void Logger::info(const QString &source, const QString &msg)
{
	return log(source, LevelInfo, msg);
}

void Logger::warning(const QString &source, const QString &msg)
{
	return log(source, LevelWarn, msg);
}

void Logger::error(const QString &source, const QString &msg)
{
	return log(source, LevelErr, msg);
}

void Logger::critical(const QString &source, const QString &msg)
{
	return log(source, LevelCrit, msg);
}

void Logger::fatal(const QString &source, const QString &msg)
{
	return log(source, LevelFatal, msg);
}

void Logger::log(const QString &source, enum Level level, const QString &msg)
{
	QString src = source;
	
	switch (level)
	{
		case LevelNone:
		case LevelAll:
		case LevelDefault:
			return;
		default:;
	}
	
	if (!m_routes.contains(src))
	{
		if ((src == "*") || (src == "default"))
			return;
		
		if (m_routes.contains("*"))
			src = "*";
		else if (m_routes.contains("default"))
			src = "default";
		else
			return;
	}
	
	send(m_routes[src], source, level, msg);
	
	if ((src != "*") && (src != "default") && m_routes.contains("*"))
		send(m_routes["*"], source, level, msg);
}

Logger::Level Logger::stringToLevel(const QString &level)
{
	if (level == "debug")
		return LevelDebug;
	if (level == "info")
		return LevelInfo;
	if (level == "warn")
		return LevelWarn;
	if (level == "error")
		return LevelErr;
	if (level == "crit")
		return LevelCrit;
	if (level == "fatal")
		return LevelFatal;
	if (level == "*")
		return LevelAll;
	if (level == "default")
		return LevelDefault;
	
	return LevelNone;
}

void Logger::send(LogRoutes &routes, const QString &source, enum Level level, const QString &msg)
{
	bool sent = false;
	LogRoutes::iterator i;
	for (i = routes.find(level); (i != routes.end()) && ((i.key() == level) || (i.key() == LevelAll)); i++)
	{
		i.value()->send(source, level, msg);
		sent = true;
	}
	
	if (sent)
		return;
	
	for (i = routes.find(LevelDefault); (i != routes.end()) && (i.key() == LevelDefault); i++)
		i.value()->send(source, level, msg);
}

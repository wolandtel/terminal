#ifndef LOGGER_H
#define LOGGER_H

#include "jconfig.h"

#define LOG_DEF_FILENAME QApplication::applicationDirPath() + "/terminal.log"
#define Log Logger::instance()

/*
	"targets":
	{
		"stderr":
		{
			"target" : "err"
		},
		"syslog":
		{
			"target" : "syslog",
			"facility" : "daemon"
		},
		"write to file":
		{
			"target" : "file",
			"path" : "terminal.log"
		},
		"console":
		{
			"target" : "pipe",
			"path" : "/dev/xconsole"
		}
	},
	"routes":
	[
		{
			"source" : "ipc",
			"level" : "warn",
			"target" : "write to file"
		},
		{
			"source" : "*" ,
			"level" : ["debug", "notice"],
			"target" : null
		},
		{
			"source" : "default",
			"level" : "*",
			"target" : ["stderr", "syslog"]
		}
	]
	
	===
	
	source	level	target
					target
					target
					…
			level	target
					…
			…
	
	source	…
	
	…
*/

class LogTarget;

class Logger
{
	public:
		enum Level
		{
			LevelNone,
			LevelDebug,
			LevelInfo,
			LevelWarn,
			LevelErr,
			LevelCrit,
			LevelFatal,
			LevelAll,
			LevelDefault
		};
		
		struct LogMessage
		{
			QString source;
			QString msg;
		};
		
		~Logger();
		
		void addRoute(const Json &source, const Json &level, const Json &target);
		void addRoute(const QString &source, const Json &level, const Json &target);
		void addRoute(const QString &source, enum Level level, const Json &target);
		void addRoute(const QString &source, enum Level level, const QString &target);
		
		void debug(const QString &source, const QString &msg);
		void info(const QString &source, const QString &msg);
		void warning(const QString &source, const QString &msg);
		void error(const QString &source, const QString &msg);
		void critical(const QString &source, const QString &msg);
		void fatal(const QString &source, const QString &msg);
		
		void log(const QString &source, enum Level level, const QString &msg);
		
		static Logger *instance(const JConfig &conf = JConfig());
		static void clear();
		static enum Level stringToLevel(const QString &level);
	
	protected:
		Logger();
		Logger(const JConfig &conf);
	
	private:
		typedef QMap<Level,LogTarget *> LogRoutes;
		typedef QMap<QString,LogRoutes> LogRouteMap;
		typedef QMap<QString,LogTarget *> LogTargetMap;
		
		LogRouteMap m_routes;
		LogTargetMap m_targets;
		static Logger *m_instance;
		
		void send(LogRoutes &routes, const QString &source, enum Level level, const QString &msg);
};

#endif // LOGGER_H

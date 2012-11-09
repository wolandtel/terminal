#ifndef JCONFIG_H
#define JCONFIG_H

#include <QApplication>
#include "json.h"

#define JCONF_BASEDIR QApplication::applicationDirPath()

class JConfig : public Json
{
	public:
		enum SourceType
		{
			SourceString,
			SourceFile
		};
		
		JConfig();
		JConfig(const QString &source, enum SourceType type = SourceFile);
		JConfig(QIODevice *device);
		
		bool load(const QString &filename);
		bool load(QIODevice *device);
		
		void save(const QString &filename) const;
		void save(QIODevice *device) const;
		
		QString toPath(const Json &def = Json()) const;
		
		const JConfig &operator[](const JsonIndex &idx) const;
		JConfig &operator[](const JsonIndex &idx);
};

#endif // JCONFIG_H

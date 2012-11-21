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
		JConfig(const Json &json);
		
		bool load(const QString &filename = QString());
		bool load(QIODevice *device);
		
		bool save(const QString &filename = QString());
		bool save(QIODevice *device) const;
		
		QString toPath(const Json &def = Json()) const;
		
		const JConfig &operator[](const JsonIndex &idx) const;
		JConfig &operator[](const JsonIndex &idx);
	
	private:
		QString m_filename;
};

#endif // JCONFIG_H

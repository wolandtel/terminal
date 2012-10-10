#ifndef JCONFIG_H
#define JCONFIG_H

#include "json.h"

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
		
		void save(const QString &filename);
		void save(QIODevice *device);
};

#endif // JCONFIG_H

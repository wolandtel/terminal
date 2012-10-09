#ifndef JCONFIG_H
#define JCONFIG_H

#include "json.h"

class JConfig : public Json
{
	public:
		JConfig();
		JConfig(const QString &string, const bool content = false);
		JConfig(QIODevice *device);
		
		bool load(const QString &filename);
		bool load(QIODevice *device);
		
		void save(const QString &filename);
		void save(QIODevice *device);
};

#endif // JCONFIG_H

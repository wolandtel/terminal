#include "jconfig.h"
#include "debug.h"

JConfig::JConfig() {}

JConfig::JConfig(const QString &source, enum SourceType type)
{
	switch (type)
	{
		case SourceString:
			parse(source);
			break;
		case SourceFile:
			load(source);
			break;
	}
}

JConfig::JConfig(QIODevice *device)
{
	load(device);
}

bool JConfig::load(const QString &filename)
{
	bool result = false;
	QFile file(filename);
	
	if (file.exists() && file.open(QIODevice::ReadOnly))
	{
		result = load(&file);
		file.close();
	}
	
	return result;
}

bool JConfig::load(QIODevice *device)
{
	parse(device->readAll());
	
	return (error() == ErrorNone) && !isNull();
}

void JConfig::save(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		save(&file);
	file.close();
}

void JConfig::save(QIODevice *device)
{
	device->write(dump().toUtf8());
}

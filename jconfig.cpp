#include "jconfig.h"

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
	QFile file(filename);
	
	if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text))
		return load(&file);
	
	return false;
}

bool JConfig::load(QIODevice *device)
{
	parse(device->readAll());
	
	return !isNull();
}

void JConfig::save(const QString &filename)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
		save(&file);
	file.close();
}

void JConfig::save(QIODevice *device)
{
	device->write(dump().toUtf8());
}

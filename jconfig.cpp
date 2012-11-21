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

JConfig::JConfig(const Json &json)
{
	m_type = Null;
	setValue(json);
}

bool JConfig::load(const QString &filename)
{
	if (!filename.isNull())
		m_filename = filename;
	
	if (m_filename.isNull())
		return false;
	
	bool result = false;
	QFile file(m_filename);
	
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

bool JConfig::save(const QString &filename)
{
	if (!filename.isNull())
		m_filename = filename;
	
	if (m_filename.isNull())
		return false;
	
	bool result = false;
	QFile file(m_filename);
	
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		result = save(&file);
		file.close();
	}
	
	return result;
}

bool JConfig::save(QIODevice *device) const
{
	const QByteArray &data = dump().toUtf8();
	return device->write(data) == data.size();
}

QString JConfig::toPath(const Json &def) const
{
	QString path = toString(def);
	
	if (path.isNull() || path[0] == '/')
		return path;
	
	return JCONF_BASEDIR + "/" + path;
}

const JConfig &JConfig::operator[](const JsonIndex &idx) const
{
	return ((const JConfig &)Json::operator[](idx));
}

JConfig &JConfig::operator[](const JsonIndex &idx)
{
	return ((JConfig &)Json::operator[](idx));
}


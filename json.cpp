#include <QtScript/QtScript>
#include "json.h"
#include "bytearray.h"

QVariant JSON::decode(const QString &json, enum JSONContainer container)
{
  QScriptEngine engine;
	QVariant result = engine.evaluate("(" + json + ")").toVariant();
	if (!(((container & JSONObject) && (result.type() == QVariant::Map))
				|| ((container & JSONArray) && (result.type() == QVariant::List))))
		result = QVariant();
	return result;
}

QString JSON::encode(const QVariantMap &object)
{
	return encodeObject(object);
}

QString JSON::encode(const QVariantList &array)
{
	return encodeArray(array);
}

QString JSON::encodeObject(const QVariantMap &object)
{
	QString json;
	QVariantMap::const_iterator i;
	
	for (i = object.constBegin(); i != object.constEnd(); i++)
	{
		if (json.isNull())
			json = "{";
		else
			json += ",";
		
		json += i.key() + ":";
		json += encodeValue(i.value());
	}
	
	if (json.isNull())
		json = "{";
	json += "}";
	
	return json;
}

QString JSON::encodeArray(const QVariantList &array)
{
	QString json;
	QVariantList::const_iterator i;
	
	for (i = array.constBegin(); i != array.constEnd(); i++)
	{
		if (json.isNull())
			json = "[";
		else
			json += ",";
		
		json += encodeValue(*i);
	}
	
	if (json.isNull())
		json = "[";
	json += "]";
	
	return json;
}

QString JSON::encodeValue(const QVariant &value)
{
	if (value.isNull())
		return "null";
	
	switch (value.type())
	{
		case QVariant::Map:
			return encodeObject(value.toMap());
		case QVariant::List:
		case QVariant::StringList:
			return encodeArray(value.toList());
		case QVariant::Bool:
			if (value.toBool())
				return "true";
			else
				return "false";
		case QVariant::String:
			return QString("\"%1\"").arg(escape(value.toString()));
		case QVariant::Int:
		case QVariant::Double:
		case QVariant::LongLong:
		case QVariant::UInt:
		case QVariant::ULongLong:
		case QVariant::Size:
			return QString("%1").arg(value.toString());
		default:
			return "null";
	}
	return "";
}

QString JSON::escape(const QChar &chr)
{
	uint code = chr.unicode();
	
	if (chr.isPrint() && (code < 255))
		return QString(chr);
	
	return "\\u" + (ByteArray(((char *)&code)[1]) + ByteArray(((char *)&code)[0])).toHex();
}

QString JSON::escape(const QString &str)
{
	QString s(str);
	s.replace(QRegExp("([\\\\\\\"/])"), "\\\\1")
								.replace(QRegExp("\n"), "\\n")
								.replace(QRegExp("\r"), "\\r")
								.replace(QRegExp("\t"), "\\t")
								.replace(QRegExp("\b"), "\\b")
								.replace(QRegExp("\f"), "\\f");
	
	int i = 0;
	while (i < s.length())
	{
		QString after = escape(s[i]);
		int l = after.length();
		if (l > 1)
			s.replace(i, 1, after);
		i += l;
	}
	
	return s;
}

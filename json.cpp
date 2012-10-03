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
	unescape(result);
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

void JSON::unescape(QVariant &var)
{
	switch (var.type())
	{
		case QVariant::String:
			var = unescape(var.toString());
			break;
		case QVariant::Map:
		{
			QVariantMap m = var.toMap();
			QVariantMap::iterator i;
			for (i = m.begin(); i != m.end(); i++)
				unescape(i.value());
			break;
		}
		case QVariant::List:
		{
			QVariantList l = var.toList();
			QVariantList::iterator i;
			for (i = l.begin(); i != l.end(); i++)
				unescape(*i);
			break;
		}
		case QVariant::StringList:
		{
			QStringList s = var.toStringList();
			QStringList::iterator i;
			for (i = s.begin(); i != s.end(); i++)
				unescape(*i);
			break;
		}
		default:;
	}
}

QString JSON::escape(const QString &str)
{
	QString s(str);
	s.replace(QRegExp("([\\\\\"/])"), "\\\\1")
		.replace("\n", "\\n")
		.replace("\r", "\\r")
		.replace("\t", "\\t")
		.replace("\b", "\\b")
		.replace("\f", "\\f");
	
	int i = 0;
	while (i < s.length())
	{
		uint code = s[i].unicode();
		if (s[i].isPrint() && (code < 255))
		{
			++i;
			continue;
		}
		
		s.replace(i, 1, "\\u" + (ByteArray(((char *)&code)[1]) + ByteArray(((char *)&code)[0])).toHex());
		i += 6;
	}
	
	return s;
}

QString JSON::unescape(const QString &str)
{
	QString s(str);
	QRegExp uc("(^|[^\\\\])\\\\u[0-9a-fA-F]{4}");
	int pos = s.indexOf(uc);
	while (pos > -1)
	{
		if (s[pos] != '\\')
			++pos;
		
		s.replace(pos, 6, QChar(s.mid(pos + 2, 4).toUShort(0, 16)));
		pos = s.indexOf(uc, pos);
	}
	
	s.replace(QRegExp("([^\\\\])\\\\u"), "\\1")
		.replace(QRegExp("\\\\([^nrtbf])"), "\\1")
		.replace("\\n", "\n")
		.replace("\\r", "\r")
		.replace("\\t", "\t")
		.replace("\\b", "\b")
		.replace("\\f", "\f");
	
	return s;
}

#include "json.h"
#include "bytearray.h"

Json::Json(const QString &json, enum JsonContainer container)
{
	QScriptEngine engine;
	QScriptValue val = engine.evaluate("(" + json + ")");
	
	if (((container & JsonObject) && (val.isObject()))
		|| ((container & JsonArray) && (val.isArray())))
		m_container = new JsonElement(decode(val));
	else
		m_container = new JsonElement();
}

Json::Json(const QVariantMap &object)
{
	m_container = new JsonElement(object);
}

Json::Json(const QVariantList &array)
{
	m_container = new JsonElement(array);
}

Json::~Json()
{
	delete m_container;
}

QString Json::toString() const
{
	if (m_encoded.isNull())
		m_encoded = encodeValue(*m_container);
	
	return m_encoded;
}

const QVariant &Json::toVariant() const
{
	return *m_container;
}

bool Json::hasElement(const QVariant &idx) const
{
	return m_container->hasElement(idx);
}

const JsonElement Json::operator[](const QVariant &idx) const
{
	return (*m_container)[idx];
}

QString Json::escape(const QString &str)
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

QString Json::unescape(const QString &str)
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

QVariant Json::decode(const QScriptValue &val) const
{
	QVariant result;
	
	if (val.isArray())
	{
		QVariantList list;
		QScriptValueIterator i(val);
		while (i.hasNext())
		{
			i.next();
			QScriptValue v = i.value();
			if (v.isObject() || v.isArray())
				list << decode(v);
			else
				list << v.toVariant();
		}
		result = list;
	}
	else if (val.isObject())
	{
		QVariantMap map;
		QScriptValueIterator i(val);
		while (i.hasNext())
		{
			i.next();
			QString n = i.name();
			QScriptValue v = i.value();
			if (v.isObject() || v.isArray())
				map[n] = decode(v);
			else
				map[n] = v.toVariant();
		}
		result = map;
	}
	
	return result;
}

QString Json::encodeObject(const QVariantMap &object) const
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

QString Json::encodeArray(const QVariantList &array) const
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

QString Json::encodeValue(const QVariant &value) const
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

/* Вроде, и без этого всё работает
void Json::unescape(QVariant &var)
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
*/

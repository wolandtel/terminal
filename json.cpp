#include "json.h"
#include "bytearray.h"
#include "math.h"

Json::Json()
{
	m_type = Null;
}

Json::Json(const JsonObject &object)
{
	m_type = Null;
	setObject(object);
}

Json::Json(const JsonArray &array)
{
	m_type = Null;
	setArray(array);
}

Json::Json(const QString &string, bool parse)
{
	m_type = Null;
	if (parse)
		this->parse(string);
	else
		setString(string);
}

Json::Json(const int val)
{
	m_type = Null;
	setNumber(val);
}

Json::Json(const double val)
{
	m_type = Null;
	setNumber(val);
}

Json::Json(const bool val)
{
	m_type = Null;
	setBool(val);
}

Json::Json(const Json &json)
{
	m_type = Null;
	setJson(json);
}

Json::Json(const QVariant &elem)
{
	m_type = Null;
	setVariant(elem);
}

Json::Json(const QVariantMap &object)
{
	m_type = Null;
	setObject(object);
}

Json::Json(const QVariantList &array)
{
	m_type = Null;
	setArray(array);
}

Json::~Json()
{
	setNull();
}

void Json::parse(const QString &json)
{
	setNull();
	setJson(parse(QScriptEngine().evaluate("(" + json + ")")));
}

QString Json::toString(bool escape) const
{
	switch (m_type)
	{
		case Object:
			return objectToString(escape);
		case Array:
			return arrayToString(escape);
		case String:
		{
			QString string = *((QString *)m_data);
			if (escape)
				string = "\"" + this->escape(string) + "\"";
			return string;
		}
		case Number:
			return QString::number(*((double *)m_data));
		case Bool:
			if (*((bool *)m_data))
				return "true";
			else
				return "false";
		default:
			return "null";
	}
}

QString Json::dump() const
{
	return toString(false);
}

const JsonObject Json::toObject() const
{
	if (isObject())
		return *((JsonObject *)m_data);
	return JsonObject();
}

const JsonArray Json::toArray() const
{
	if (isArray())
		return *((JsonArray *)m_data);
	return JsonArray() << *this;
}

double Json::toNumber() const
{
	switch (m_type)
	{
		case Object:
			return ((JsonObject *)m_data)->size();
		case Array:
			return ((JsonArray *)m_data)->size();
		case String:
			return ((QString *)m_data)->toDouble();
		case Number:
			return *((double *)m_data);
		case Bool:
			if (*((bool *)m_data))
				return 1;
			else
				return 0;
		default:
			return 0;
	}
}

bool Json::toBool() const
{
	switch (m_type)
	{
		case Object:
			return ((JsonObject *)m_data)->size() > 0;
		case Array:
			return ((JsonArray *)m_data)->size() > 0;
		case String:
		{
			QString &data = *((QString *)m_data);
			return (data.size() > 0) && (data.toLower() != "false") ;
		}
		case Number:
			return *((double *)m_data) != 0;
		case Bool:
			return *((bool *)m_data);
		default:
			return false;
	}
}

int Json::toInt() const
{
	return (int)round(toNumber());
}

bool Json::contains(const JsonIndex &idx) const
{
	if (idx.isInt())
	{
		if (isArray())
		{
			int i = idx.toInt();
			return (0 <= i) && (i >= toArray().size());
		}
		
	}
	else if (isObject())
		return toObject().contains(idx.toString());
	
	return false;
}

const Json Json::operator[](const JsonIndex &idx) const
{
	if (idx.isInt())
	{
		if (isArray())
			return toArray()[idx.toInt()];
	}
	else if (isObject())
		return toObject()[idx.toString()];
	
	return *this;
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

void Json::setNull()
{
	if (isNull())
		return;
	
	switch (m_type)
	{
		case Object:
			delete (JsonObject *)m_data;
			break;
		case Array:
			delete (JsonArray *)m_data;
			break;
		case String:
			delete (QString *)m_data;
			break;
		case Number:
			delete (double *)m_data;
			break;
		case Bool:
			delete (bool *)m_data;
			break;
		default:;
	}
	
	m_type = Null;
}

void Json::setObject(const JsonObject &object)
{
	setNull();
	m_type = Object;
	m_data = new JsonObject(object);
}

void Json::setArray(const JsonArray &array)
{
	setNull();
	m_type = Array;
	m_data = new JsonArray(array);
}

void Json::setString(const QString &string)
{
	setNull();
	m_type = String;
	m_data = new QString(string);
}

void Json::setNumber(const int val)
{
	setNull();
	m_type = Number;
	m_data = new double;
	*((int *)m_data) = val;
}

void Json::setNumber(const double val)
{
	setNull();
	m_type = Number;
	m_data = new double;
	*((double *)m_data) = val;
}

void Json::setBool(const bool val)
{
	setNull();
	m_type = Bool;
	m_data = new bool;
	*((bool *)m_data) = val;
}

void Json::setJson(const Json &json)
{
	setNull();
	switch (json.type())
	{
		case Null:
			setNull();
			break;
		case Object:
			setObject(json.toObject());
			break;
		case Array:
			setArray(json.toArray());
			break;
		case String:
			setString(json.toString(false));
			break;
		case Number:
			setNumber(json.toNumber());
			break;
		case Bool:
			setBool(json.toBool());
			break;
	}
}

void Json::setVariant(const QVariant &elem)
{
	setNull();
	if (elem.isNull())
		return;
	
	switch (elem.type())
	{
		case QVariant::Map:
			setObject(elem.toMap());
			break;
		case QVariant::List:
			setArray(elem.toList());
			break;
		case QVariant::StringList:
			setArray(elem.toStringList());
			break;
		case QVariant::Bool:
			setBool(elem.toBool());
			break;
		case QVariant::String:
			setString(elem.toString());
			break;
		case QVariant::Int:
		case QVariant::Double:
		case QVariant::LongLong:
		case QVariant::UInt:
		case QVariant::ULongLong:
		case QVariant::Size:
			setNumber(elem.toDouble());
			break;
		default:;
	}
}

void Json::setObject(const QVariantMap &object)
{
	JsonObject jo;
	QVariantMap::const_iterator i;
	for (i = object.constBegin(); i != object.constEnd(); i++)
		jo[i.key()] = Json((QVariant)i.value());
	setObject(jo);
}

void Json::setArray(const QVariantList &array)
{
	JsonArray ja;
	QVariantList::const_iterator i;
	for (i = array.constBegin(); i != array.constEnd(); i++)
		ja << Json((QVariant)*i);
	setArray(ja);
}

void Json::setArray(const QStringList &array)
{
	JsonArray ja;
	QStringList::const_iterator i;
	for (i = array.constBegin(); i != array.constEnd(); i++)
		ja << Json((QString)*i);
	setArray(ja);
}

QString Json::objectToString(bool escape) const
{
	QString json;
	JsonObject &object = *((JsonObject *)m_data);
	JsonObject::const_iterator i;
	
	for (i = object.constBegin(); i != object.constEnd(); i++)
	{
		if (json.isNull())
			json = "{";
		else
			json += ",";
		
		json += i.key() + ":";
		json += i.value().toString(escape);
	}
	
	if (json.isNull())
		json = "{";
	json += "}";
	
	return json;
}

QString Json::arrayToString(bool escape) const
{
	QString json;
	JsonArray &array = *((JsonArray *)m_data);
	JsonArray::const_iterator i;
	
	for (i = array.constBegin(); i != array.constEnd(); i++)
	{
		if (json.isNull())
			json = "[";
		else
			json += ",";
		
		json += ((Json)*i).toString(escape);
	}
	
	if (json.isNull())
		json = "[";
	json += "]";
	
	return json;
}

Json Json::parse(const QScriptValue &sv)
{
	if (sv.isNull())
		return Json();
	
	if (sv.isArray()) // !Важно: массив также определяется как объект, поэтому проверку на массив надо делать раньше
	{
		JsonArray array;
		QScriptValueIterator i(sv);
		while (i.hasNext())
		{
			i.next();
			array << parse(i.value());
		}
		return Json(array);
	}
	
	if (sv.isObject())
	{
		JsonObject object;
		QScriptValueIterator i(sv);
		while (i.hasNext())
		{
			i.next();
			object[i.name()] = parse(i.value());
		}
		return Json(object);
	}
	
	if (sv.isString())
		setString(sv.toString());
	
	if (sv.isNumber())
		return Json(sv.toNumber());
	
	if (sv.isBool())
		return Json(sv.toBool());
	
	return Json();
}



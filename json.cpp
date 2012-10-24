#include "json.h"
#include "bytearray.h"
#include "math.h"
#include "debug.h"

Json::Json()
{
	m_type = Null;
}

Json::Json(const JsonObject &object)
{
	m_type = Null;
	setValue(object);
}

Json::Json(const JsonArray &array)
{
	m_type = Null;
	setValue(array);
}

Json::Json(const QString &string, enum InputFormat format)
{
	m_type = Null;
	switch (format)
	{
		case InputPlain:
			setValue(string);
			break;
		case InputEncoded:
			parse(string);
			break;
	}
}

Json::Json(const char *string, enum InputFormat format)
{
	m_type = Null;
	switch (format)
	{
		case InputPlain:
			setValue(string);
			break;
		case InputEncoded:
			parse(string);
			break;
	}
}

Json::Json(const QByteArray &data, enum InputFormat format)
{
	m_type = Null;
	switch (format)
	{
		case InputPlain:
			setValue(data);
			break;
		case InputEncoded:
			parse(data);
			break;
	}
}

Json::Json(const int val)
{
	m_type = Null;
	setValue(val);
}

Json::Json(const double val)
{
	m_type = Null;
	setValue(val);
}

Json::Json(const bool val)
{
	m_type = Null;
	setValue(val);
}

Json::Json(const Json &json)
{
	m_type = Null;
	setValue(json);
}

Json::Json(const QVariant &elem)
{
	m_type = Null;
	setValue(elem);
}

Json::Json(const QVariantMap &object)
{
	m_type = Null;
	setValue(object);
}

Json::Json(const QVariantList &array)
{
	m_type = Null;
	setValue(array);
}

Json::~Json()
{
	setNull();
}

void Json::parse(const QString &jString)
{
	QScriptEngine se;
	const QScriptValue &sv = se.evaluate("(" + jString + ")");
	
	if (se.hasUncaughtException())
		m_error = ErrorParsing;
	else
		setValue(parse(sv)); // error is set by setNull()
}

void Json::parse(const QByteArray &jData)
{
	parse(QString::fromUtf8(jData.constData()));
}

void Json::parse(const char *jString)
{
	parse(QString::fromUtf8(jString));
}

QString Json::encode(enum EncodeMode mode) const
{
	m_error = ErrorNone;
	switch (m_type)
	{
		case Object:
			return encodeObject(mode);
		case Array:
			return encodeArray(mode);
		case String:
			return "\"" + escape(*((QString *)m_data), mode) + "\"";
		default:
			return toString();
	}
}

const JsonObject &Json::toObject(const Json &def) const
{
	static JsonObject err;
	
	if (isObject())
	{
		m_error = ErrorNone;
		return *((JsonObject *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isObject())
		return def.toObject();
	
	return err = JsonObject();
}

JsonObject &Json::toObject(const Json &def)
{
	static JsonObject err;
	
	if (isObject())
	{
		m_error = ErrorNone;
		return *((JsonObject *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isObject())
		return err = def.toObject();
	
	return err = JsonObject();
}

const JsonArray &Json::toArray(const Json &def) const
{
	static JsonArray err;
	
	if (isArray())
	{
		m_error = ErrorNone;
		return *((JsonArray *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isArray())
		return def.toArray();
	
	err = JsonArray();
	err << *this;
	
	return err;
}

JsonArray &Json::toArray(const Json &def)
{
	static JsonArray err;
	
	if (isArray())
	{
		m_error = ErrorNone;
		return *((JsonArray *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isArray())
		return err = def.toArray();
	
	return err = JsonArray();
}

QString Json::toString(const Json &def) const
{
	if (isString())
	{
		m_error = ErrorNone;
		return *((QString *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isString())
		return def.toString();
	
	switch (m_type)
	{
		case Object:
			return encodeObject(EncodeDump);
		case Array:
			return encodeArray(EncodeDump);
		case String:
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

double Json::toNumber(const Json &def) const
{
	if (isNumber())
	{
		m_error = ErrorNone;
		return *((double *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isNumber())
		return def.toNumber();
	
	switch (m_type)
	{
		case Object:
			return ((JsonObject *)m_data)->size();
		case Array:
			return ((JsonArray *)m_data)->size();
		case String:
			return ((QString *)m_data)->toDouble();
		case Bool:
			if (*((bool *)m_data))
				return 1;
			else
				return 0;
		default:
			return 0;
	}
}

bool Json::toBool(const Json &def) const
{
	if (isBool())
	{
		m_error = ErrorNone;
		return *((bool *)m_data);
	}
	
	m_error = ErrorTypeMismatch;
	if (def.isBool())
		return def.toBool();
	
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
		default:
			return false;
	}
}

int Json::toInt(const Json &def) const
{
	return (int)round(toNumber(def));
}

bool Json::contains(const JsonIndex &idx) const
{
	m_error = ErrorNone;
	if (idx.isInt())
	{
		if (isArray())
		{
			int i = idx.toInt();
			return (0 <= i) && (i <= toArray().size());
		}
	}
	else if (isObject())
		return toObject().contains(idx.toString());
	
	return false;
}

const Json &Json::operator[](const JsonIndex &idx) const
{
	static Json json;
	
	m_error = ErrorNone;
	if (idx.isInt())
	{
		if (isArray())
		{
			int i = idx.toInt();
			const JsonArray &array = toArray();
			
			if ((0 <= i) && (i <= array.size()))
				return json = array[i];
		}
	}
	else if (isObject())
	{
		const JsonObject &object = toObject();
		const QString &i = idx.toString();
		
		if (object.contains(i))
			return json = object[i];
	}
	
	return json = Json();
}

Json &Json::operator[](const JsonIndex &idx)
{
	static Json err;
	
	m_error = ErrorNone;
	if (idx.isInt())
	{
		if (isArray())
		{
			int i = idx.toInt();
			JsonArray &array = toArray();
			
			if ((0 <= i) && (i <= array.size()))
				return array[i];
			else
			{
				array << Json();
				return array[array.size()];
			}
		}
	}
	else if (isObject())
	{
		JsonObject &object = toObject();
		const QString &i = idx.toString();
		
		if (!object.contains(i))
			object[i] = Json();
		
		return object[i];
	}
	
	return err = Json();
}

Json &Json::operator=(const Json &val)
{
	setValue(val);
	return *this;
}

QString Json::escape(const QString &str, enum EncodeMode mode)
{
	QString s(str);
	
	s.replace(QRegExp(QString("([\\\\\"") + (mode == EncodeDump ? "" : "/") + "])"), "\\\\1")
		.replace("\n", "\\n")
		.replace("\r", "\\r")
		.replace("\t", "\\t")
		.replace("\b", "\\b")
		.replace("\f", "\\f");
	
	if (mode == EncodeDump)
		return s;
	
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
	m_error = ErrorNone;
	if (isNull())
		return;
	
	switch (m_type)
	{
		case Object:
			delete ((JsonObject *)m_data);
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

void Json::setValue(const JsonObject &val)
{
	setNull(); // error is set here
	m_type = Object;
	m_data = new JsonObject(val);
}

void Json::setValue(const JsonArray &val)
{
	setNull(); // error is set here
	m_type = Array;
	m_data = new JsonArray(val);
}

void Json::setValue(const QString &val)
{
	setNull(); // error is set here
	m_type = String;
	m_data = new QString(val);
}

void Json::setValue(const QByteArray &val)
{
	setValue(QString::fromUtf8(val.constData()));
}

void Json::setValue(const char *val)
{
	setValue(QString::fromUtf8(val));
}

void Json::setValue(const int val)
{
	setValue((const double)val);
}

void Json::setValue(const double val)
{
	setNull(); // error is set here
	m_type = Number;
	m_data = new double;
	*((double *)m_data) = val;
}

void Json::setValue(const bool val)
{
	setNull(); // error is set here
	m_type = Bool;
	m_data = new bool;
	*((bool *)m_data) = val;
}

void Json::setValue(const Json &val)
{
	switch (val.type())
	{
		case Null:
			setNull();
			break;
		case Object:
			setValue(val.toObject());
			break;
		case Array:
			setValue(val.toArray());
			break;
		case String:
			setValue(val.toString());
			break;
		case Number:
			setValue(val.toNumber());
			break;
		case Bool:
			setValue(val.toBool());
			break;
	}
	m_error = val.error();
}

void Json::setValue(const QVariant &val)
{
	setNull(); // error is set here
	switch (val.type())
	{
		case Null:
			return;
		case QVariant::Map:
			setValue(val.toMap());
			break;
		case QVariant::List:
			setValue(val.toList());
			break;
		case QVariant::StringList:
			setValue(val.toStringList());
			break;
		case QVariant::Bool:
			setValue(val.toBool());
			break;
		case QVariant::String:
			setValue(val.toString());
			break;
		case QVariant::Int:
		case QVariant::Double:
		case QVariant::LongLong:
		case QVariant::UInt:
		case QVariant::ULongLong:
		case QVariant::Size:
			setValue(val.toDouble());
			break;
		default:;
	}
}

void Json::setValue(const QVariantMap &val)
{
	JsonObject jo;
	QVariantMap::const_iterator i;
	for (i = val.constBegin(); i != val.constEnd(); i++)
		jo[i.key()] = Json((QVariant)i.value());
	setValue(jo); // error is set here
}

void Json::setValue(const QVariantList &val)
{
	JsonArray ja;
	QVariantList::const_iterator i;
	for (i = val.constBegin(); i != val.constEnd(); i++)
		ja << Json((QVariant)*i);
	setValue(ja); // error is set here
}

void Json::setValue(const QStringList &val)
{
	JsonArray ja;
	QStringList::const_iterator i;
	for (i = val.constBegin(); i != val.constEnd(); i++)
		ja << Json((QString)*i);
	setValue(ja); // error is set here
}

QString Json::encodeObject(enum EncodeMode mode) const
{
	QString json;
	JsonObject &object = *((JsonObject *)m_data);
	JsonObject::const_iterator i;
	
	m_error = ErrorNone;
	for (i = object.constBegin(); i != object.constEnd(); i++)
	{
		if (json.isNull())
			json = "{";
		else
			json += ",";
		
		QString key = escape(i.key(), mode);
		switch (mode)
		{
			case EncodeStandard:
				json += "\"" + key + "\":";
				break;
			case EncodeDump:
				json += " " + key + " : ";
				break;
			default:
				json += key;
		}
		json += i.value().encode(mode);
	}
	
	if (json.isNull())
		json = "{";
	else if (mode == EncodeDump)
		json += " ";
	
	json += "}";
	
	return json;
}

QString Json::encodeArray(enum EncodeMode mode) const
{
	QString json;
	JsonArray &array = *((JsonArray *)m_data);
	JsonArray::const_iterator i;
	
	m_error = ErrorNone;
	for (i = array.constBegin(); i != array.constEnd(); i++)
	{
		if (json.isNull())
			json = "[";
		else
			json += ",";
		
		json += (mode == EncodeDump ? " " : "") + ((Json)*i).encode(mode);
	}
	
	if (json.isNull())
		json = "[";
	else if (mode == EncodeDump)
		json += " ";
	
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
			if (i.name() != "length")
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
		return Json(sv.toString());
	
	if (sv.isNumber())
		return Json(sv.toNumber());
	
	if (sv.isBool())
		return Json(sv.toBool());
	
	return Json();
}

#ifndef JSON_H
#define JSON_H

#include <QtScript/QtScript>
#include <QVariant>
#include <QMap>
#include <QList>
#include "jsonindex.h"

class Json;

typedef QMap<QString,Json> JsonObject;
typedef QList<Json> JsonArray;

class Json
{
	public:
		enum Type
		{
			Null,
			Object,
			Array,
			String,
			Number,
			Bool
		};
		
		enum InputFormat
		{
			InputPlain,
			InputEncoded
		};
		
		enum EncodeMode
		{
			EncodeStandard,
			EncodeDump
		};
		
		explicit Json();
		Json(const JsonObject &object);
		Json(const JsonArray &array);
		Json(const QString &string, enum InputFormat format = InputPlain);
		Json(const QByteArray &data, enum InputFormat format = InputPlain);
		Json(const char *string, enum InputFormat format = InputPlain);
		Json(const int val);
		Json(const double val);
		Json(const bool val);
		Json(const Json &json);
		Json(const QVariant &elem);
		Json(const QVariantMap &object);
		Json(const QVariantList &array);
		~Json();
		
		inline void setValue() { setNull(); }
		void parse(const QString &json);
		void parse(const QByteArray &json);
		void parse(const char *json);
		QString encode(enum EncodeMode mode = EncodeStandard) const;
		inline QString dump() const { return encode(EncodeDump); }
		
		inline enum Type type() const { return m_type; }
		
		inline bool isNull() const { return m_type == Null; }
		inline bool isObject() const { return m_type == Object; }
		inline bool isArray() const { return m_type == Array; }
		inline bool isString() const { return m_type == String; }
		inline bool isNumber() const { return m_type == Number; }
		inline bool isBool() const { return m_type == Bool; }
		
		const JsonObject &toObject() const;
		const JsonArray &toArray() const;
		QString toString() const;
		double toNumber() const;
		bool toBool() const;
		int toInt() const;
		
		JsonObject &toObject();
		JsonArray &toArray();
		
		bool contains(const JsonIndex &idx) const;
		const Json &operator[](const JsonIndex &idx) const;
		Json &operator[](const JsonIndex &idx);
		Json &operator=(const Json &val);
		
		static QString escape(const QString &str, enum EncodeMode mode = EncodeDump);
		static QString unescape(const QString &str);
	
	protected:
		void setValue(const JsonObject &val);
		void setValue(const JsonArray &val);
		void setValue(const QString &val);
		void setValue(const QByteArray &val);
		void setValue(const char *val);
		void setValue(const int val);
		void setValue(const double val);
		void setValue(const bool val);
		void setValue(const Json &val);
		void setValue(const QVariant &val);
		void setValue(const QVariantMap &val);
		void setValue(const QVariantList &val);
		void setValue(const QStringList &val);
		
		QString encodeObject(enum EncodeMode mode) const;
		QString encodeArray(enum EncodeMode mode) const;
	
	private:
		enum Type m_type;
		void *m_data;
		
		void setNull();
		
		Json parse(const QScriptValue &sv);
};

Q_DECLARE_METATYPE(Json)

#endif // JSON_H

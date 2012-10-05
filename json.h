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
		enum JsonType
		{
			Null,
			Object,
			Array,
			String,
			Number,
			Bool
		};
		
		explicit Json();
		Json(const JsonObject &object);
		Json(const JsonArray &array);
		Json(const QString &string, bool parse = false);
		Json(const int val);
		Json(const double val);
		Json(const bool val);
		Json(const Json &elem);
		Json(const QVariant &elem);
		Json(const QVariantMap &object);
		Json(const QVariantList &array);
		~Json();
		
		void parse(const QString &json);
		QString toString(bool escape = true) const;
		QString dump() const;
		
		inline enum JsonType type() const { return m_type; }
		
		inline bool isNull() const { return m_type == Null; }
		inline bool isObject() const { return m_type == Object; }
		inline bool isArray() const { return m_type == Array; }
		inline bool isString() const { return m_type == String; }
		inline bool isNumber() const { return m_type == Number; }
		inline bool isBool() const { return m_type == Bool; }
		
		const JsonObject toObject() const;
		const JsonArray toArray() const;
		double toNumber() const;
		bool toBool() const;
		int toInt() const;
		
		bool contains(const JsonIndex &idx) const;
		const Json operator[](const JsonIndex &idx) const;
		
		static QString escape(const QString &str);
		static QString unescape(const QString &str);
	
	private:
		enum JsonType m_type;
		void *m_data;
		bool m_constructed;
		
		void setNull();
		void setObject(const JsonObject &object);
		void setArray(const JsonArray &array);
		void setString(const QString &string);
		void setNumber(const int val);
		void setNumber(const double val);
		void setBool(const bool val);
		void setJson(const Json &json);
		void setVariant(const QVariant &elem);
		void setObject(const QVariantMap &object);
		void setArray(const QVariantList &array);
		void setArray(const QStringList &array);
		QString objectToString(bool escape = true) const;
		QString arrayToString(bool escape = true) const;
		Json parse(const QScriptValue &sv);
};

#endif // JSON_H

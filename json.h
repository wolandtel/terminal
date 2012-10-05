#ifndef JSON_H
#define JSON_H

#include <QtScript/QtScript>
#include "jsonelement.h"

class Json
{
	public:
		enum JsonContainer
		{
			JsonObject = 1,
			JsonArray = 2,
			JsonAny = 0xFF
		};
		
		Json(const QString &json, enum JsonContainer container = JsonAny);
		Json(const QVariantMap &object);
		Json(const QVariantList &array);
		~Json();
		
		QString toString() const;
		const QVariant &toVariant() const;
		
		bool hasElement(const QVariant &idx) const;
		const JsonElement operator[](const QVariant &idx) const;
		
		static QString escape(const QString &str);
		static QString unescape(const QString &str);
	
	protected:
		JsonElement *m_container;
	
	private:
		mutable QString m_encoded;
		
		QVariant decode(const QScriptValue &val) const;
		QString encodeObject(const QVariantMap &object) const;
		QString encodeArray(const QVariantList &array) const;
		QString encodeValue(const QVariant &value) const;
		// static void unescape(QVariant &var);
};

#endif // JSON_H

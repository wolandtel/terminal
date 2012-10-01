#ifndef JSON_H
#define JSON_H

class JSON
{
	public:
		enum JSONContainer
		{
			JSONObject = 1,
			JSONArray = 2,
			JSONAny = 0xFF
		};
		
		static QVariant decode(const QString &json, enum JSONContainer container = JSONAny);
		static QString encode(const QVariantMap &object);
		static QString encode(const QVariantList &array);
		static QString escape(const QString &str);
		
	private:
		static QString encodeObject(const QVariantMap &object);
		static QString encodeArray(const QVariantList &array);
		static QString encodeValue(const QVariant &value);
		static QString escape(const QChar &chr);
};

#endif // JSON_H

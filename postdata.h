#ifndef POSTDATA_H
#define POSTDATA_H

#include "json.h"

class PostData : public Json
{
	public:
		explicit PostData();
		PostData(const QString &id, const QString &secret);
		PostData(const JsonObject &object);
		void setClient(const QString &card, const QString &pin);
		void setAction(const QString &type, const QString &modifier);
		void setParam(const QString &type, const Json &value);
		QByteArray content();
	
	private:
		void clearAction();
		void clear();
};

Q_DECLARE_METATYPE(PostData)

#endif // POSTDATA_H

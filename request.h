#ifndef REQUEST_H
#define REQUEST_H

#include <QVariantMap>

class Request : public QVariantMap
{
	public:
		explicit Request(unsigned long long id, const QString &secret);
		void setClient(const QString &card, const QString &pin);
		void setAction(const QString &type, const QString &modifier);
		void setParam(const QString &type, const QVariant &value);
		QString prepare();
	
	private:
		void clearAction();
		void clear();
};

#endif // REQUEST_H

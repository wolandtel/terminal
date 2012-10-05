#ifndef POSTDATA_H
#define POSTDATA_H

#include <QVariantMap>

class PostData : public QVariantMap
{
	public:
		explicit PostData(const QString &id, const QString &secret);
		PostData(const QVariantMap &map);
		void setClient(const QString &card, const QString &pin);
		void setAction(const QString &type, const QString &modifier);
		void setParam(const QString &type, const QVariant &value);
		QByteArray content();
	
	private:
		void clearAction();
		void clear();
};

#endif // POSTDATA_H

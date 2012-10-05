#ifndef JSONELEMENT_H
#define JSONELEMENT_H

#include <QVariant>

class JsonElement : public QVariant
{
	public:
		explicit JsonElement();
		JsonElement(const QVariant &elem);
		
		bool isObject() const;
		bool isArray() const;
		bool hasElement(const QVariant &idx) const;
		const JsonElement operator[](const QVariant &idx) const;
};

#endif // JSONELEMENT_H

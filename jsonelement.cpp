#include "jsonelement.h"

JsonElement::JsonElement() : QVariant() {}

JsonElement::JsonElement(const QVariant &elem) : QVariant(elem) {}

bool JsonElement::isObject() const
{
	return type() == QVariant::Map;
}

bool JsonElement::isArray() const
{
	return type() == QVariant::List;
}

bool JsonElement::hasElement(const QVariant &idx) const
{
	if (idx.type() == QVariant::String)
	{
		if (isObject())
			return toMap().contains(idx.toString());
	}
	else if ((idx.type() == QVariant::Int) && isArray())
	{
		int i = idx.toInt();
		return (0 <= i) && (i >= toList().size());
	}
	
	return false;
}
const JsonElement JsonElement::operator[](const QVariant &idx) const
{
	if (idx.type() == QVariant::String)
	{
		if (isObject())
			return toMap()[idx.toString()];
	}
	else if ((idx.type() == QVariant::Int) && isArray())
		return toList()[idx.toInt()];
	
	return *this;
}


#include "request.h"
#include "json.h"

Request::Request(unsigned long long id, const QString &secret)
{
	Request &self = *this;
	
	self["device"] = QVariantMap();
	((QVariantMap &)self["device"])["id"] = id;
	((QVariantMap &)self["device"])["secret"] = secret;
	
	self["client"] = QVariantMap();
	((QVariantMap &)self["client"])["card"];
	((QVariantMap &)self["client"])["pin"];
	
	self["action"] = QVariantMap();
	((QVariantMap &)self["action"])["type"];
	((QVariantMap &)self["action"])["modifier"];
	
	self["param"] = QVariantMap();
}

void Request::setClient(const QString &card, const QString &pin)
{
	Request &self = *this;
	
	clear();
	((QVariantMap &)self["client"])["card"] = card;
	((QVariantMap &)self["client"])["pin"] = pin;
}

void Request::setAction(const QString &type, const QString &modifier)
{
	Request &self = *this;
	
	clearAction();
	((QVariantMap &)self["action"])["type"] = type;
	((QVariantMap &)self["action"])["modifier"] = modifier;
}

void Request::setParam(const QString &name, const QVariant &value)
{
	Request &self = *this;
	
	((QVariantMap &)self["param"])[name] = value;
}

void Request::clearAction()
{
	Request &self = *this;
	
	((QVariant &)((QVariantMap &)self["action"])["type"]).clear();
	((QVariant &)((QVariantMap &)self["action"])["modifier"]).clear();
	((QVariantMap &)self["param"]).clear();
}

void Request::clear()
{
	Request &self = *this;
	
	clearAction();
	((QVariant &)((QVariantMap &)self["client"])["card"]).clear();
	((QVariant &)((QVariantMap &)self["client"])["pin"]).clear();
}

QString Request::prepare()
{
	return JSON::encode(*this);
}

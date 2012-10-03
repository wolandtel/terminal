#include <QUrl>
#include "postdata.h"
#include "json.h"

PostData::PostData(const QString &id, const QString &secret)
{
	PostData &self = *this;
	
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

void PostData::setClient(const QString &card, const QString &pin)
{
	PostData &self = *this;
	
	clear();
	((QVariantMap &)self["client"])["card"] = card;
	((QVariantMap &)self["client"])["pin"] = pin;
}

void PostData::setAction(const QString &type, const QString &modifier)
{
	PostData &self = *this;
	
	clearAction();
	((QVariantMap &)self["action"])["type"] = type;
	((QVariantMap &)self["action"])["modifier"] = modifier;
}

void PostData::setParam(const QString &name, const QVariant &value)
{
	PostData &self = *this;
	
	((QVariantMap &)self["param"])[name] = value;
}

void PostData::clearAction()
{
	PostData &self = *this;
	
	((QVariant &)((QVariantMap &)self["action"])["type"]).clear();
	((QVariant &)((QVariantMap &)self["action"])["modifier"]).clear();
	((QVariantMap &)self["param"]).clear();
}

void PostData::clear()
{
	PostData &self = *this;
	
	clearAction();
	((QVariant &)((QVariantMap &)self["client"])["card"]).clear();
	((QVariant &)((QVariantMap &)self["client"])["pin"]).clear();
}

QByteArray PostData::content()
{
	return "request=" + QUrl::toPercentEncoding(JSON::encode(*this));
}


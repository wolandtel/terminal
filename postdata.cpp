#include <QUrl>
#include "postdata.h"

PostData::PostData() {}

PostData::PostData(const QString &id, const QString &secret)
{
	JsonObject o;
	
	o["device"] = JsonObject();
	o["device"]["id"] = id;
	o["device"]["secret"] = secret;
	
	o["client"] = JsonObject();
	o["client"]["card"];
	o["client"]["pin"];
	
	o["action"] = JsonObject();
	o["action"]["type"];
	o["action"]["modifier"];
	
	o["param"] = JsonObject();
	
	setValue(o);
}

PostData::PostData(const JsonObject &object) : Json(object) {}

void PostData::setClient(const QString &card, const Json &pin)
{
	PostData &self = *this;
	
	clear();
	self["client"]["card"] = card;
	self["client"]["pin"] = pin;
}

void PostData::setAction(const QString &type, const Json &modifier)
{
	PostData &self = *this;
	
	clearAction();
	self["action"]["type"] = type;
	self["action"]["modifier"] = modifier;
}

void PostData::setParam(const QString &name, const Json &value)
{
	PostData &self = *this;
	
	self["param"][name] = value;
}

void PostData::clearAction()
{
	PostData &self = *this;
	
	self["action"]["type"].setValue();
	self["action"]["modifier"].setValue();
	self["param"] = JsonObject();
}

void PostData::clear()
{
	PostData &self = *this;
	
	clearAction();
	self["client"]["card"].setValue();
	self["client"]["pin"].setValue();
}

QByteArray PostData::content()
{
	return "request=" + QUrl::toPercentEncoding(encode());
}


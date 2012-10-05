#include "jsonindex.h"

JsonIndex::JsonIndex(const int idx) : m_int(true), m_idx(QString::number(idx)) {}

JsonIndex::JsonIndex(const QString &idx) : m_int(false), m_idx(idx) {}

JsonIndex::JsonIndex(const char *idx) : m_int(false), m_idx(idx) {}

int JsonIndex::toInt() const
{
	if (m_int)
		return m_idx.toInt();
	else
		return -1;
}

QString JsonIndex::toString() const
{
	return m_idx;
}

#ifndef JSONINDEX_H
#define JSONINDEX_H

#include <QString>

class JsonIndex
{
	public:
		explicit JsonIndex(const int idx);
		JsonIndex(const QString &idx);
		JsonIndex(const char *idx);
		
		inline bool isInt() const { return m_int; };
		int toInt() const;
		QString toString() const;
		
	private:
		const bool m_int;
		const QString m_idx;
};

#endif // JSONINDEX_H

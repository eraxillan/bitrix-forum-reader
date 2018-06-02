#ifndef __BFR_LOGGER_H__
#define __BFR_LOGGER_H__

#include <QByteArray>
#include <QString>
#include <QDateTime>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h> // must be included

#define ConsoleLogger spdlog::get("console")

// FIXME: don't work
//FMT_DISABLE_CONVERSION_TO_INT(QByteArray);
/*inline std::ostream& operator << (std::ostream &os, const QByteArray &c)
{
    return os << c.toStdString();
}
*/

inline std::ostream& operator << (std::ostream &os, const QString &c)
{
    return os << c.toStdString();
}

inline std::ostream& operator << (std::ostream &os, const QDateTime &c)
{
    return os << c.toString().toStdString();
}

#endif // __BFR_LOGGER_H__

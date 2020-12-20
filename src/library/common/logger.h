/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifndef __BFR_LOGGER_H__
#define __BFR_LOGGER_H__

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDateTime>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h> // must be included
#ifdef Q_OS_ANDROID
#include <spdlog/sinks/android_sink.h>
#endif
#ifdef Q_OS_WIN
#include <thirdparty/spdlog/sinks/windebug_sink.h>
#endif

#define SystemLogger spdlog::get("console")

// FIXME: don't work
//FMT_DISABLE_CONVERSION_TO_INT(QByteArray);
/*inline std::ostream& operator << (std::ostream &os, const QByteArray &c)
{
    return os << c.toStdString();
}
*/

inline std::ostream &operator<<(std::ostream &os, const QString &c) { return os << c.toStdString(); }

inline std::ostream &operator<<(std::ostream &os, const QDateTime &c) { return os << c.toString().toStdString(); }

#endif // __BFR_LOGGER_H__

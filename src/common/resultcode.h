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
#ifndef __BFR_RESULTCODE_H__
#define __BFR_RESULTCODE_H__

namespace result_code {

enum class Type {
	Invalid = -1,
	// General
	Ok = 0,
	OkFalse = 1,
	Fail,
	InProgress,
	// CURL
	CurlError,
	// System
	NetworkError,
	InputOutputError,
	// Forum parser
	ForumPageParserError,
	ForumParserError,
// Serialization
#ifdef BFR_SERIALIZATION_ENABLED
	InvalidFileFormat,
	InvalidFileVersion,
#endif // #ifdef BFR_SERIALIZATION_ENABLED
	// TODO: others
	Count
};

inline bool succeeded(Type rc) { return ((rc == Type::Ok) || (rc == Type::OkFalse)); }

inline bool failed(Type rc) { return !succeeded(rc); }

}  // namespace result_code


#define BFR_DECLARE_DEFAULT_RETURN_TYPE_N_VALUE(Type, value) static Type __invalid_return_value__ = value;
#define BFR_DECLARE_DEFAULT_RETURN_TYPE(Type) static Type __invalid_return_value__;
#define BFR_RETURN_VOID_IF(cond, msg) { if (cond) { Q_ASSERT_X(0, Q_FUNC_INFO, msg); ConsoleLogger->error("{}: assert failure with message '{}'", Q_FUNC_INFO, msg); return; } }
#define BFR_RETURN_DEFAULT_IF(cond, msg) { if (cond) { Q_ASSERT_X(0, Q_FUNC_INFO, msg); ConsoleLogger->error("{}: assert failure with message '{}'", Q_FUNC_INFO, msg); return __invalid_return_value__; } }
#define BFR_RETURN_VALUE_IF(cond, returnValue, msg) { if (cond) { Q_ASSERT_X(0, Q_FUNC_INFO, msg); ConsoleLogger->error("{}: assert failure with message '{}'", Q_FUNC_INFO, msg); return returnValue; } }

#endif // __BFR_RESULTCODE_H__

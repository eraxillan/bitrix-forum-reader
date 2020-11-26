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

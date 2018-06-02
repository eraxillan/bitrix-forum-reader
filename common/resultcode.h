#ifndef __BFR_RESULTCODE_H__
#define __BFR_RESULTCODE_H__

namespace result_code {

enum class Type
{
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
    // Forum parser
    ForumPageParserError,
    ForumParserError,
    // TODO: others
    Count
};

inline bool succeeded(Type rc)
{
    return (rc == Type::Ok || rc == Type::OkFalse);
}

inline bool failed(Type rc)
{
    return !succeeded(rc);
}

}  // namespace result_code

#endif // __BFR_RESULTCODE_H__

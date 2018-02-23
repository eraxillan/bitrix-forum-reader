#ifndef BRR_RESULTCODE_H
#define BRR_RESULTCODE_H

namespace result_code {

enum class Type
{
    // General
    Ok = 0,
    OkFalse = 1,
    Fail,
    // CURL
    CurlError,
    // System
    NetworkError,
    // Forum parser
    ForumPageParserError,
    ForumParserError
    // TODO: others
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

#endif // BRR_RESULTCODE_H

#ifndef BRR_RESULTCODE_H
#define BRR_RESULTCODE_H

enum class ResultCode
{
    // General
    Ok = 0,
    Fail = 1,
    // CURL
    CurlError,
    // System
    NetworkError,
    // Forum parser
    ForumPageParserError,
    ForumParserError
    // TODO: others
};

#endif // BRR_RESULTCODE_H

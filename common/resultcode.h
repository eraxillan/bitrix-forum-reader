#ifndef RESULTCODE_H
#define RESULTCODE_H

enum class ResultCode
{
    // General
    S_OK = 0,
    E_FAIL,
    // CURL
    E_CURL,
    // System
    E_NETWORK,
    // Forum parser
    E_FORUM_PAGE_PARSER,
    E_FORUM_PARSER
    // TODO: others
};

#endif // RESULTCODE_H

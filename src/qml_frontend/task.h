#ifndef TASK_H
#define TASK_H

#include <common/forumthreadurl.h>

class BfrTask
{
public:
    enum class Action {
        Invalid = -1,
        ParseForumThreadPageCount,  // Input: URL           | Output: URL, int
        ParseForumThreadPagePosts,  // Input: URL, pageNo   | Output: URL, PostList
        ExtractForumThreadUsers,    // Input: URL           | Output: URL, UserList
//        AnalyzeForumThreadUsers,    // Input: URL, UserList | Output: URL, UserList
        Count
    };

private:
    static const int INVALID_PAGENO = -1;

    // Input data
    Action m_action;
    ForumThreadUrlData m_url;
    int m_pageNo;

    // Output data
//    int m_pageCount; // Action: ParseForumThreadPageCount
//    bfr::PostList m_posts; // Action: ParseForumThreadPagePosts
//    bfr::UserList m_users; // Action: ExtractForumThreadUsers, AnalyzeForumThreadUsers

public:
    BfrTask();

    BfrTask(Action action, ForumThreadUrlData url);

    BfrTask(Action action, ForumThreadUrlData url, int pageNo);

    bool isValid() const;

    Action action() const { return m_action; }
    ForumThreadUrlData url() const { return m_url; }
    int pageNo() const { return m_pageNo; }
};

#endif // TASK_H

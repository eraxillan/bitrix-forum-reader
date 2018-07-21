#ifndef FORUMTHREADPOOL_H
#define FORUMTHREADPOOL_H

#include <common/resultcode.h>
#include <common/logger.h>
#include <common/filedownloader.h>
#include <common/forumthreadurl.h>
#include <website_backend/websiteinterface_fwd.h>

#include <functional>

class ForumThreadPool : public QObject
{
    Q_OBJECT

protected:
    using PagePostMap = QMap<int /*pageNo*/, bfr::PostList /*pagePosts*/>;
    using ThreadPagePostMap = QMap<ForumThreadUrlData /*forumThreadUrl*/, PagePostMap /*forumThreadPagesPosts*/>;
    using ThreadPageCountMap = QMap<ForumThreadUrlData /*forumThreadUrl*/, int /*pageCount*/>;

    ThreadPageCountMap m_threadPageCountCollection;
    ThreadPagePostMap m_threadPagePostCollection;

    explicit ForumThreadPool(QObject *parent = nullptr) : QObject(parent) {}
    //ForumThreadPool() {}
    ~ForumThreadPool() {}

    size_t pageCountCacheSize() { return m_threadPageCountCollection.size() * (sizeof(ForumThreadUrlData) + sizeof(int)); }
    size_t pagePostsCacheSize() { /*FIXME: implement correct size calc using loop on inner map*/ return m_threadPagePostCollection.size() * (sizeof(ForumThreadUrlData) + sizeof(PagePostMap)); }

    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

public:
    static ForumThreadPool &globalInstance();

    // Delete copy and move constructors and assign operators
    ForumThreadPool(ForumThreadPool const&) = delete;             // Copy construct
    ForumThreadPool(ForumThreadPool&&) = delete;                  // Move construct
    ForumThreadPool& operator=(ForumThreadPool const&) = delete;  // Copy assign
    ForumThreadPool& operator=(ForumThreadPool &&) = delete;      // Move assign

public:
    // FIXME: implement
    //enum class Policy { Invalid = -1, CachedOnly, PreferCached, PreferNew, NewOnly, Count };
    //void setPolicy(Policy policy);
    // FIXME: implement
    //void setMaximumMemoryUsage(quint64 maxCacheMem);

    /*SYNC*/ result_code::Type getForumThreadPageCount(ForumThreadUrlData urlData, int &pageCount);

    /*SYNC*/ result_code::Type getForumPagePosts(ForumThreadUrlData urlData, int pageNo, bfr::PostList &posts);

    /*SYNC*/ result_code::Type getForumThreadPosts(ForumThreadUrlData urlData, bfr::PostList &posts);

signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
//    void downloadFinished();
//    void downloadFailed(result_code::Type code);

    void threadParseProgress(int pageNo, int pageCount);
};

#endif // FORUMTHREADPOOL_H

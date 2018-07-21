#ifndef __BFR_FORUMREADER_H__
#define __BFR_FORUMREADER_H__

#include "common/resultcode.h"
#include "common/filedownloader.h"
#include "common/forumthreadurl.h"
#include "website_backend/websiteinterface.h"

class ForumReader : public QObject
{
    Q_OBJECT

    using PostList = bfr::PostList;
    using ResultCodeFutureWatcher = QFutureWatcher<result_code::Type>;

    ResultCodeFutureWatcher m_forumPageCountWatcher;
    ResultCodeFutureWatcher m_forumPageParserWatcher;

    PostList m_pagePosts;
    int      m_pageCount;
    int      m_pageNo;

    result_code::Type m_lastError;

public:
    ForumReader();
    ~ForumReader();

    // Helper functions
    Q_INVOKABLE QString   applicationDirPath() const;
    Q_INVOKABLE QUrl      convertToUrl(QString urlStr) const;

    // Forum HTML page parser async API (use Qt signal-slots system)
    Q_INVOKABLE void      startPageCountAsync(ForumThreadUrl *url);
    Q_INVOKABLE void      startPageParseAsync(ForumThreadUrl *url, int pageNo);

    // The number of pages and posts
    Q_INVOKABLE int       pageCount() const;
    Q_INVOKABLE int       pagePostCount() const;

    // Page posts properties getters
    Q_INVOKABLE QString   postAuthorQml(int index) const;
    Q_INVOKABLE QString   postAuthorSignature(int index) const;

    // FIXME: currently unused! use it or remove
    Q_INVOKABLE int       postAvatarMaxWidth() const;

    Q_INVOKABLE QDateTime postDateTime(int index) const;
    Q_INVOKABLE QString   postText(int index) const;
    Q_INVOKABLE QString   postLastEdit(int index) const;
    Q_INVOKABLE int       postLikeCount(int index) const;

signals:
    // Forum parser signals
    void pageCountParsed(int pageCount);
    void pageContentParsed(int pageNo);

    void pageContentParseProgressRange(int minimum, int maximum);
    void pageContentParseProgress(int value);

private slots:
    // Forum page count parser slots
    void onForumPageCountParsed();
    void onForumPageCountParsingCancelled();

    // Forum page downloader slots
    void onForumPageDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    // Forum page user posts parser slots
    void onForumPageParsed();
    void onForumPageParsingCancelled();
};

#endif // __BFR_FORUMREADER_H__

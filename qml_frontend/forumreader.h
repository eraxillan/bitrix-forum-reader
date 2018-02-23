#ifndef FORUMREADER_H
#define FORUMREADER_H

#include "common/resultcode.h"
#include "common/filedownloader.h"
#include "website_backend/websiteinterface.h"

class ForumReader : public QObject
{
    Q_OBJECT

    using UserPosts = BankiRuForum::UserPosts;
    using IntFutureWatcher = QFutureWatcher<int>;
    using ParserFutureWatcher = QFutureWatcher<BankiRuForum::UserPosts>;

    FileDownloader m_downloader;

    IntFutureWatcher    m_forumPageCountWatcher;
    ParserFutureWatcher m_forumPageParserWatcher;

    QByteArray m_pageData;
    UserPosts  m_pagePosts;
    int        m_pageCount;
    int        m_pageNo;

    ResultCode m_lastError;

public:
    ForumReader();
    ~ForumReader();

    // Helper functions
    Q_INVOKABLE QString   applicationDirPath() const;
    Q_INVOKABLE QUrl      convertToUrl(QString urlStr) const;

    // Forum HTML page parser sync API (e.g. for testing purposes)
#ifdef BITRIX_FORUM_READER_SYNC_API
    Q_INVOKABLE int       parsePageCount(QString urlStr);
    Q_INVOKABLE bool      parseForumPage(QString urlStr, int pageNo);
#endif

    // Forum HTML page parser async API (use Qt signal-slots system)
    Q_INVOKABLE void      startPageCountAsync(QString urlStr);
    Q_INVOKABLE void      startPageParseAsync(QString urlStr, int pageNo);

    // The number of pages and posts
    Q_INVOKABLE int       pageCount() const;
    Q_INVOKABLE int       postCount() const;

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
    void onForumPageCountParsingCanceled();

    // Forum page downloader slots
    void onForumPageDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onForumPageDownloaded();
    void onForumPageDownloadFailed(ResultCode code);

    // Forum page user posts parser slots
    void onForumPageParsed();
    void onForumPageParsingCanceled();
};

#endif // FORUMREADER_H

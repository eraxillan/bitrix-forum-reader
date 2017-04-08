#ifndef FORUMREADER_H
#define FORUMREADER_H

#include "common/resultcode.h"
#include "website_backend/websiteinterface.h"

class ForumReader : public QObject
{
    Q_OBJECT

    typedef BankiRuForum::UserPosts                 UserPosts;
    typedef QFutureWatcher<int>                     IntFutureWatcher;
    typedef QFutureWatcher<BankiRuForum::UserPosts> ParserFutureWatcher;

    IntFutureWatcher    m_forumPageCountWatcher;
    ParserFutureWatcher m_forumPageParserWatcher;

    UserPosts m_pagePosts;
    int       m_pageCount;
    int       m_pageNo;

    ResultCode m_lastError;

public:
    ForumReader();
    ~ForumReader();

    // Helper functions
    Q_INVOKABLE QString   applicationDirPath() const;
    Q_INVOKABLE QUrl      convertToUrl(QString urlStr) const;

    // Forum HTML page parser sync API (e.g. for testing purposes)
    Q_INVOKABLE int       parsePageCount(QString urlStr);
    Q_INVOKABLE bool      parseForumPage(QString urlStr, int pageNo);

    // Forum HTML page parser async API (use Qt signal-slots system)
    Q_INVOKABLE void      startPageCountAsync(QString urlStr);
    Q_INVOKABLE void      startPageParseAsync(QString urlStr, int pageNo);

    // The number of pages and posts
    Q_INVOKABLE int       pageCount() const;
    Q_INVOKABLE int       postCount() const;

    // Page posts properties getters
    Q_INVOKABLE QString   postAuthor(int index) const;
    Q_INVOKABLE int       postAuthorPostCount(int index) const;
    Q_INVOKABLE QDate     postAuthorRegistrationDate(int index) const;
    Q_INVOKABLE int       postAuthorReputation(int index) const;
    Q_INVOKABLE QString   postAuthorCity(int index) const;
    Q_INVOKABLE QString   postAuthorSignature(int index) const;

    Q_INVOKABLE QString   postAvatarUrl(int index) const;
    Q_INVOKABLE int       postAvatarWidth(int index) const;
    Q_INVOKABLE int       postAvatarHeight(int index) const;
    Q_INVOKABLE int       postAvatarMaxWidth() const;

    Q_INVOKABLE QDateTime postDateTime(int index) const;
    Q_INVOKABLE QString   postText(int index) const;
    Q_INVOKABLE QString   postLastEdit(int index) const;
    Q_INVOKABLE int       postLikeCount(int index) const;

    Q_INVOKABLE QString   postFooterQml() const;

signals:
    // Forum parser signals
    void pageCountParsed(int pageCount);
    void pageContentParsed(int pageNo);

private slots:
    // Forum page count parser slots
    void onForumPageCountParsed();
    void onForumPageCountParsingCanceled();

    // Forum page user posts parser slots
    void onForumPageParsed();
    void onForumPageParsingCanceled();
};

#endif // FORUMREADER_H

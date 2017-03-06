#ifndef FORUMREADER_H
#define FORUMREADER_H

#include <QObject>

#include "website_backend/websiteinterface.h"

class ForumReader : public QObject
{
    Q_OBJECT

    BankiRuForum::UserPosts m_userPosts;
    int m_pageCount = 0;
    int m_pageNo = 0;

public:
    ForumReader();
    ~ForumReader();

    // Helper functions
    Q_INVOKABLE QString applicationDirPath() const;
    Q_INVOKABLE QUrl convertToUrl(QString urlStr) const;

    // Download and parse
    Q_INVOKABLE int parsePageCount(QString urlStr);
    Q_INVOKABLE bool parseForumPage(QString urlStr, int pageNo);

    // Topic functions
    Q_INVOKABLE int pageCount() const;

    // Page functions

    // Total post count on the page
    Q_INVOKABLE int postCount() const;

    Q_INVOKABLE QString postAuthor(int index) const;

    Q_INVOKABLE QString postAvatarUrl(int index) const;
    Q_INVOKABLE int postAvatarWidth(int index) const;
    Q_INVOKABLE int postAvatarHeight(int index) const;
    Q_INVOKABLE int postAvatarMaxWidth() const;

    Q_INVOKABLE QDateTime postDateTime(int index) const;
    Q_INVOKABLE QString postText(int index) const;
    Q_INVOKABLE QString postLastEdit(int index) const;
    Q_INVOKABLE int postLikeCount(int index) const;

    Q_INVOKABLE int postAuthorPostCount(int index) const;
    Q_INVOKABLE QDate postAuthorRegistrationDate(int index) const;
    Q_INVOKABLE int postAuthorReputation(int index) const;
    Q_INVOKABLE QString postAuthorCity(int index) const;

    Q_INVOKABLE QString postAuthorSignature(int index) const;

    Q_INVOKABLE QString postFooterQml() const;
};

#endif // FORUMREADER_H

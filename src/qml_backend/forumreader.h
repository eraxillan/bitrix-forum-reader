#ifndef FORUMREADER_H
#define FORUMREADER_H

#include <QObject>

#include "src/website_backend/websiteinterface.h"

class ForumReader : public QObject
{
    Q_OBJECT

    BankiRuForum::UserPosts m_userPosts;

public:
    ForumReader();
    ~ForumReader();

    explicit ForumReader( BankiRuForum::UserPosts userPosts, QObject *parent = 0 );

    // Helper functions
    Q_INVOKABLE QUrl convertToUrl(QString urlStr) const;
    Q_INVOKABLE bool parseForumPage(QString forumPageRawHtml);

    // Total post count on the page
    Q_INVOKABLE int postCount() const;

    Q_INVOKABLE QString postAuthor(int index) const;

    Q_INVOKABLE QString postAvatarUrl(int index) const;
    Q_INVOKABLE int postAvatarWidth(int index) const;
    Q_INVOKABLE int postAvatarHeight(int index) const;
    Q_INVOKABLE int postAvatarMaxWidth() const;

    Q_INVOKABLE QDateTime postDateTime(int index) const;
    Q_INVOKABLE QString postText(int index) const;
    Q_INVOKABLE int postLikeCount(int index) const;

    Q_INVOKABLE int postAuthorPostCount(int index) const;
    Q_INVOKABLE QDate postAuthorRegistrationDate(int index) const;
    Q_INVOKABLE int postAuthorReputation(int index) const;
    Q_INVOKABLE QString postAuthorCity(int index) const;

    Q_INVOKABLE QString postAuthorSignature(int index) const;
};

#endif // FORUMREADER_H

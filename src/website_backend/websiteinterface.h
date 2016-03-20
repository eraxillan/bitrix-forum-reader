#ifndef WEBSITEINTERFACE_H
#define WEBSITEINTERFACE_H

#include <QtCore/QtCore>

namespace BankiRuForum
{
    //--------------------------------------------------------------------------------------------
    // Data structures
    struct Image
    {
        QUrl m_url;
        int m_width = -1;
        int m_height = -1;

        bool isValid() const { return !m_url.isEmpty() && (m_width > 0 && m_height > 0); }
    };

    struct User
    {
        // Base info
        int m_userId = -1;
        QString m_userName;
        QUrl m_userProfileUrl;

        // Avatar info
        Image m_userAvatar;

        // Additional info
        QUrl m_allPostsUrl;
        int m_postCount = -1;
        QDate m_registrationDate;
        int m_reputation = -1;
        QString m_city;
    };

    struct Post
    {
        int m_id;
        //int m_postNumber;
        int m_likeCounter;

        QString m_text;
//		QString m_style;
        QString m_userSignature;
        QDateTime m_date;
//		QUrl m_permalink;
    };

    typedef QVector<QPair<User, Post>> UserPosts;

    //---------------------------------------------------------------------------------------------
    // Interfaces

    class IForumPageReader
    {
    public:
        virtual ~IForumPageReader() {}

        virtual int getPagePosts(QUrl webPageUrl, UserPosts& userPosts) = 0;
    };

}

#endif // WEBSITEINTERFACE_H

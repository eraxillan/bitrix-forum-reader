#ifndef WEBSITEINTERFACE_H
#define WEBSITEINTERFACE_H

#include <QtCore/QtCore>
#include <QtWidgets/QApplication>

//#define RBR_DRAW_FRAME_ON_COMPONENT_FOR_DEBUG
//#define RBR_PRINT_DEBUG_OUTPUT
//#define RBR_DUMP_GENERATED_QML_IN_FILES
//#define RBR_QML_OUTPUT_DIR QString("__temp_qml")

namespace BankiRuForum
{
    const QString QUOTE_WRITE_VERB = "пишет";

    struct IPostObject
    {
        virtual ~IPostObject();

        virtual bool    isValid() const = 0;
        virtual uint    getHash(uint seed) const = 0;
        virtual QString getQmlString(int randomSeed) const = 0;
    };
    typedef QList< QSharedPointer<IPostObject> > IPostObjectList;

    // NOTE: spoiler text is HTML
    struct PostSpoiler : IPostObject
    {
        QString m_title;
        IPostObjectList m_data;

        PostSpoiler();

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    // NOTE: quote text is HTML
    struct PostQuote : IPostObject
    {
        QString m_title;
        QString m_userName;
        QUrl m_url;
        IPostObjectList m_data;

        PostQuote();

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct PostImage : IPostObject
    {
        QString m_url;
        int m_width = -1;
        int m_height = -1;
        int m_border = -1;
        QString m_altName;
        QString m_id;
        QString m_className;

        PostImage();
        PostImage(QString url, int width, int height, int border = 0, QString altName = QString(), QString id = QString(), QString className = QString());

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct PostLineBreak : IPostObject
    {
        PostLineBreak();

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct PostPlainText : IPostObject
    {
        QString m_text;

        PostPlainText();
        PostPlainText(QString text);

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct PostRichText : IPostObject
    {
        QString m_text;
        QString m_color = "black";
        bool m_isBold = false;
        bool m_isItalic = false;
        bool m_isUnderlined = false;
        bool m_isStrikedOut = false;

        PostRichText();
        PostRichText(QString text, QString color, bool isBold, bool isItalic, bool isUnderlined, bool isStrikedOut);

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct PostVideo : IPostObject
    {
        // URL
        QString m_urlStr;
        QUrl m_url;

        PostVideo();
        PostVideo(QString urlStr);

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    // FIXME: implement hyperlink using Qt/QML power only, without Qt primitive HTML-mode of Text item
    struct PostHyperlink : IPostObject
    {
        // URL
        QString m_urlStr;
        QUrl m_url;

        // Display name
        QString m_title;

        // Balloon tip
        QString m_tip;

        QString m_rel;

        PostHyperlink();
        PostHyperlink(QString urlStr, QString title, QString tip = QString(), QString rel = QString());

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    // ----------------------------------------------------------------------------------------------------------------

    struct Post
    {
        int m_id = -1;
//      int m_postNumber;
        int m_likeCounter = -1;

        //QString m_text;
        IPostObjectList m_data;

//		QString m_style;
        QString m_lastEdit;
        QString m_userSignature;
        QDateTime m_date;
//		QUrl m_permalink;

        bool isValid() const
        {
            return (m_id > 0) && (m_likeCounter >= 0) && !m_data.isEmpty() && m_date.isValid();
        }

        uint getHash(uint seed) const
        {
            uint dataHash = 0;
            for (auto obj : m_data) dataHash ^= obj->getHash(seed);

            return qHash(m_id, seed) ^ qHash(m_likeCounter, seed) ^ dataHash
                    ^ qHash(m_lastEdit, seed) ^ qHash(m_userSignature, seed) ^ qHash(m_date, seed);
        }
    };

    struct User
    {
        // Base info
        int m_userId = -1;
        QString m_userName;
        QUrl m_userProfileUrl;

        // Avatar info
        QSharedPointer<PostImage> m_userAvatar;

        // Additional info
        QUrl m_allPostsUrl;
        int m_postCount = -1;
        QDate m_registrationDate;
        int m_reputation = -1;
        QString m_city;

        bool isValid() const
        {
            return (m_userId > 0) && !m_userName.isEmpty() && m_userProfileUrl.isValid()
                    && m_allPostsUrl.isValid() && (m_postCount > 0) && m_registrationDate.isValid() && (m_reputation >= 0);
        }

        uint getHash(uint seed) const
        {
            return qHash(m_userId, seed) ^ qHash(m_userName, seed) ^ qHash(m_userProfileUrl, seed)
                    ^ (m_userAvatar ? m_userAvatar->getHash(seed) : 0) ^ qHash(m_allPostsUrl, seed) ^ qHash(m_postCount, seed)
                    ^ qHash(m_registrationDate, seed) ^ qHash(m_reputation, seed) ^ qHash(m_city, seed);
        }
    };

    typedef QPair<User, Post>  UserPost;
    typedef QVector<UserPost>  UserPosts;
    typedef QVector<UserPosts> PageUserPosts;

    //---------------------------------------------------------------------------------------------
    // Interfaces

    class IForumPageReader
    {
    public:
        virtual ~IForumPageReader();

        virtual int getPageCount(QByteArray rawData, int& pageCount) = 0;
        virtual int getPagePosts(QByteArray rawData, UserPosts& userPosts) = 0;
    };

}   // namespace BankiRuForum

//---------------------------------------------------------------------------------------------

inline uint qHash(const BankiRuForum::IPostObjectList& key, uint seed)
{
    uint dataHash = 0;
    for (auto obj : key)
        dataHash ^= obj->getHash(seed);
    return dataHash;
}

inline uint qHash(const BankiRuForum::Post& key, uint seed)
{
    return key.getHash(seed);
}

inline uint qHash(const BankiRuForum::User& key, uint seed)
{
    return key.getHash(seed);
}

#endif // WEBSITEINTERFACE_H

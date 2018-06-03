#ifndef __BFR_WEBSITEINTERFACE_H__
#define __BFR_WEBSITEINTERFACE_H__

#include <QtCore/QtCore>

#include <common/resultcode.h>

#include <website_backend/websiteinterface_fwd.h>

//#define BFR_QML_OUTPUT_DIR QString("__temp_qml")

#define BFR_SHOW_SPOILER
#define BFR_SHOW_QUOTE
#define BFR_SHOW_IMAGE
#define BFR_SHOW_LINEBREAK
#define BFR_SHOW_PLAINTEXT
#define BFR_SHOW_RICHTEXT
#define BFR_SHOW_VIDEO
#define BFR_SHOW_HYPERLINK

namespace bfr
{
    const QString QUOTE_WRITE_VERB = "пишет";

    struct IPostObject
    {
        virtual ~IPostObject();

        virtual bool    isValid() const = 0;
        virtual uint    getHash(uint seed) const = 0;
        virtual QString getQmlString(int randomSeed) const = 0;
    };

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

    struct Post : IPostObject
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

        UserPtr m_author;

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct User : IPostObject
    {
        // Base info
        int m_userId = -1;
        QString m_userName;
        QUrl m_userProfileUrl;

        // Avatar info
        PostImagePtr m_userAvatar;

        // Additional info
        QUrl m_allPostsUrl;
        int m_postCount = -1;
        QDate m_registrationDate;
        int m_reputation = -1;
        QString m_city;

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    //---------------------------------------------------------------------------------------------
    // Interfaces

    class IForumPageReader
    {
    public:
        virtual ~IForumPageReader();

        virtual result_code::Type getPageCount(QByteArray rawData, int &pageCount) = 0;
        virtual result_code::Type getPagePosts(QByteArray rawData, PostList &userPosts) = 0;
    };

}   // namespace bfr

//---------------------------------------------------------------------------------------------

inline uint qHash(const bfr::IPostObjectList& key, uint seed)
{
    uint dataHash = 0;
    for (auto obj : key)
        dataHash ^= obj->getHash(seed);
    return dataHash;
}

inline uint qHash(const bfr::Post& key, uint seed)
{
    return key.getHash(seed);
}

inline uint qHash(const bfr::User& key, uint seed)
{
    return key.getHash(seed);
}

#endif // __BFR_WEBSITEINTERFACE_H__

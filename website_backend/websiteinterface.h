#ifndef WEBSITEINTERFACE_H
#define WEBSITEINTERFACE_H

#include <QtCore/QtCore>
#include <QtWidgets/QApplication>

#ifdef HAVE_QX_ORM
#include <QxOrm.h>
#include <common/export.h>
#endif

#include <common/resultcode.h>

//#define RBR_QML_OUTPUT_DIR QString("__temp_qml")

#define RBR_SHOW_SPOILER
#define RBR_SHOW_QUOTE
#define RBR_SHOW_IMAGE
#define RBR_SHOW_LINEBREAK
#define RBR_SHOW_PLAINTEXT
#define RBR_SHOW_RICHTEXT
#define RBR_SHOW_VIDEO
#define RBR_SHOW_HYPERLINK

//--------------------------------------------------------------------------------------------------------------------------
class BasePostData;
class UserData;
class PostData;

using PostObjectPtr = std::shared_ptr<BasePostData>;
using PostObjectCollection = std::vector<PostObjectPtr>;

using UserDataPtr = std::shared_ptr<UserData>;
using UserDataCollection = std::vector<UserDataPtr>;
using PostDataPtr = std::shared_ptr<PostData>;
using PostDataCollection = std::vector<PostDataPtr>;

struct BFR_DLL_EXPORT BasePostData
{
#ifdef HAVE_QX_ORM
    int m_databaseId;
#endif

    virtual ~BasePostData() {}
};

struct BFR_DLL_EXPORT PostSpoilerData : BasePostData
{
    QString m_title;
    PostObjectCollection m_data;
};

struct BFR_DLL_EXPORT PostQuoteData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostQuote)
    int m_databaseId;
#endif*/

    QString m_title;
    QString m_userName;
    QUrl m_url;
    PostObjectCollection m_data;

//    PostQuoteData();
};

struct BFR_DLL_EXPORT PostImageData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostImage)
    int m_databaseId;
#endif*/

    QString m_url;
    int m_width = -1;
    int m_height = -1;
    int m_border = -1;
    QString m_altName;
    QString m_id;
    QString m_className;

//    PostImageData();
//    PostImageData(QString url, int width, int height, int border = 0, QString altName = QString(), QString id = QString(), QString className = QString());
};

struct BFR_DLL_EXPORT PostLineBreakData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostLineBreak)
    int m_databaseId;
#endif*/

//    PostLineBreakData();
};

struct BFR_DLL_EXPORT PostPlainTextData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostPlainText)
    int m_databaseId;
#endif*/

    QString m_text;

//    PostPlainTextData();
//    PostPlainTextData(QString text);
};

struct BFR_DLL_EXPORT PostRichTextData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostRichText)
    int m_databaseId;
#endif*/

    QString m_text;
    QString m_color = "black";
    bool m_isBold = false;
    bool m_isItalic = false;
    bool m_isUnderlined = false;
    bool m_isStrikedOut = false;

//    PostRichTextData();
//    PostRichTextData(QString text, QString color, bool isBold, bool isItalic, bool isUnderlined, bool isStrikedOut);
};

struct BFR_DLL_EXPORT PostVideoData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostVideo)
    int m_databaseId;
#endif*/

    // URL
    QString m_urlStr;
    QUrl m_url;

//    PostVideoData();
//    PostVideoData(QString urlStr);
};

struct BFR_DLL_EXPORT PostHyperlinkData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostHyperlink)
    int m_databaseId;
#endif*/

    // URL
    QString m_urlStr;
    QUrl m_url;

    // Display name
    QString m_title;

    // Balloon tip
    QString m_tip;

    QString m_rel;

//    PostHyperlinkData();
//    PostHyperlinkData(QString urlStr, QString title, QString tip = QString(), QString rel = QString());
};

struct BFR_DLL_EXPORT PostData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::Post)
    int m_databaseId;
#endif*/

    int m_id = -1;
//      int m_postNumber;
    int m_likeCounter = -1;

    //QString m_text;
    PostObjectCollection m_data;

//		QString m_style;
    QString m_lastEdit;
    QString m_userSignature;
    QDateTime m_date;
//		QUrl m_permalink;

    UserDataPtr m_author;
};

struct BFR_DLL_EXPORT UserData : BasePostData
{
/*#ifdef HAVE_QX_ORM
    QX_REGISTER_FRIEND_CLASS(BankiRuForum::User)
    int m_databaseId;
#endif*/

    // Base info
    int m_userId = -1;
    QString m_userName;
    QUrl m_userProfileUrl;

    // Avatar info
    std::shared_ptr<PostImageData> m_userAvatar;

    // Additional info
    QUrl m_allPostsUrl;
    int m_postCount = -1;
    QDate m_registrationDate;
    int m_reputation = -1;
    QString m_city;

    // All user posts in this forum topic
    PostDataCollection m_posts;
};

#ifdef HAVE_QX_ORM

QX_REGISTER_PRIMARY_KEY(BasePostData, int)
QX_REGISTER_HPP_BFR(BasePostData, qx::trait::no_base_class_defined, 0)

QX_REGISTER_PRIMARY_KEY(PostSpoilerData, int)
QX_REGISTER_HPP_BFR(PostSpoilerData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostQuoteData, int)
QX_REGISTER_HPP_BFR(PostQuoteData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostImageData, int)
QX_REGISTER_HPP_BFR(PostImageData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostLineBreakData, int)
QX_REGISTER_HPP_BFR(PostLineBreakData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostPlainTextData, int)
QX_REGISTER_HPP_BFR(PostPlainTextData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostRichTextData, int)
QX_REGISTER_HPP_BFR(PostRichTextData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostVideoData, int)
QX_REGISTER_HPP_BFR(PostVideoData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostHyperlinkData, int)
QX_REGISTER_HPP_BFR(PostHyperlinkData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(PostData, int)
QX_REGISTER_HPP_BFR(PostData, BasePostData, 0)

QX_REGISTER_PRIMARY_KEY(UserData, int)
QX_REGISTER_HPP_BFR(UserData, BasePostData, 0)

#endif

//--------------------------------------------------------------------------------------------------------------------------

namespace BankiRuForum
{
    class User;
    class Post;
    using UserPtr = std::shared_ptr<User>;
    using UserCollection = std::vector<UserPtr>;
    using PostPtr = std::shared_ptr<Post>;
    using PostCollection = std::vector<PostPtr>;
    // FIXME: add others

    const QString QUOTE_WRITE_VERB = "пишет";

    struct BFR_DLL_EXPORT IPostObject
    {
#ifdef HAVE_QX_ORM
//        QX_REGISTER_FRIEND_CLASS(BankiRuForum::IPostObject)
        int m_databaseId;
#endif

        virtual ~IPostObject();

        virtual bool    isValid() const = 0;
        virtual uint    getHash(uint seed) const = 0;
        virtual QString getQmlString(int randomSeed) const = 0;
    };
    //using IPostObjectList = QList< std::shared_ptr<IPostObject>>;
    using IPostObjectList = std::vector<std::shared_ptr<IPostObject>>;

    // NOTE: spoiler text is HTML
    struct BFR_DLL_EXPORT PostSpoiler : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostSpoiler)
        int m_databaseId;
#endif*/

        QString m_title;
        IPostObjectList m_data;

        PostSpoiler();

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    // NOTE: quote text is HTML
    struct BFR_DLL_EXPORT PostQuote : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostQuote)
        int m_databaseId;
#endif*/

        QString m_title;
        QString m_userName;
        QUrl m_url;
        IPostObjectList m_data;

        PostQuote();

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct BFR_DLL_EXPORT PostImage : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostImage)
        int m_databaseId;
#endif*/

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

    struct BFR_DLL_EXPORT PostLineBreak : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostLineBreak)
        int m_databaseId;
#endif*/

        PostLineBreak();

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct BFR_DLL_EXPORT PostPlainText : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostPlainText)
        int m_databaseId;
#endif*/

        QString m_text;

        PostPlainText();
        PostPlainText(QString text);

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct BFR_DLL_EXPORT PostRichText : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostRichText)
        int m_databaseId;
#endif*/

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

    struct BFR_DLL_EXPORT PostVideo : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostVideo)
        int m_databaseId;
#endif*/

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
    struct BFR_DLL_EXPORT PostHyperlink : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::PostHyperlink)
        int m_databaseId;
#endif*/

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

    struct BFR_DLL_EXPORT Post : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::Post)
        int m_databaseId;
#endif*/

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

        //void addObject( std::shared_ptr<IPostObject> obj);
        //void removeObject( std::shared_ptr<IPostObject> obj);

        bool isValid() const Q_DECL_OVERRIDE;
        uint getHash(uint seed) const Q_DECL_OVERRIDE;
        QString getQmlString(int randomSeed) const Q_DECL_OVERRIDE;
    };

    struct BFR_DLL_EXPORT User : IPostObject
    {
/*#ifdef HAVE_QX_ORM
        QX_REGISTER_FRIEND_CLASS(BankiRuForum::User)
        int m_databaseId;
#endif*/

        // Base info
        int m_userId = -1;
        QString m_userName;
        QUrl m_userProfileUrl;

        // Avatar info
         std::shared_ptr<PostImage> m_userAvatar;

        // Additional info
        QUrl m_allPostsUrl;
        int m_postCount = -1;
        QDate m_registrationDate;
        int m_reputation = -1;
        QString m_city;

        // All user posts in this forum topic
        PostCollection m_posts;

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
        virtual result_code::Type getPagePosts(QByteArray rawData, PostCollection &userPosts) = 0;
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

//---------------------------------------------------------------------------------------------

#ifdef HAVE_QX_ORM

QX_REGISTER_PRIMARY_KEY(BankiRuForum::IPostObject, int)
QX_REGISTER_ABSTRACT_CLASS(BankiRuForum::IPostObject)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::IPostObject, qx::trait::no_base_class_defined, 0, qx_test_IPostObject)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostSpoiler, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostSpoiler, BankiRuForum::IPostObject, 0, qx_test_PostSpoiler)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostQuote, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostQuote, BankiRuForum::IPostObject, 0, qx_test_PostQuote)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostImage, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostImage, BankiRuForum::IPostObject, 0, qx_test_PostImage)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostLineBreak, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostLineBreak, BankiRuForum::IPostObject, 0, qx_test_PostLineBreak)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostPlainText, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostPlainText, BankiRuForum::IPostObject, 0, qx_test_PostPlainText)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostRichText, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostRichText, BankiRuForum::IPostObject, 0, qx_test_PostRichText)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostVideo, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostVideo, BankiRuForum::IPostObject, 0, qx_test_PostVideo)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::PostHyperlink, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::PostHyperlink, BankiRuForum::IPostObject, 0, qx_test_PostHyperlink)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::Post, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::Post, BankiRuForum::IPostObject, 0, qx_test_Post)

QX_REGISTER_PRIMARY_KEY(BankiRuForum::User, int)
QX_REGISTER_COMPLEX_CLASS_NAME_HPP_BFR(BankiRuForum::User, BankiRuForum::IPostObject, 0, qx_test_User)

#endif


#endif // WEBSITEINTERFACE_H

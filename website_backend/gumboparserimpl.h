#ifndef GUMBOPARSERIMPL_H
#define GUMBOPARSERIMPL_H

#include "websiteinterface.h"
#include "qtgumbodocument.h"

namespace BankiRuForum {

class ForumPageParser : public IForumPageReader
{
    struct UserBaseInfo
    {
        int m_id = -1;
        QString m_name;
        QUrl m_profileUrl;
    };

    struct UserAdditionalInfo
    {
        QUrl m_allPostsUrl;
        int m_postCount = -1;
        QDate m_registrationDate;
        int m_reputation = -1;
        QString m_city;
    };

private:
    mutable bool m_textQuoteFlag = false;

private:
    void printTagsRecursively(QtGumboNode node, int &level);
    void findMsdivNodesRecursively(QtGumboNode node, QVector<QtGumboNode> &msdivNodes);
    void findPageCount(QString rawData, int &pageCount);
    UserBaseInfo getUserBaseInfo(QtGumboNode userInfoNode);
    UserAdditionalInfo getUserAdditionalInfo(QtGumboNode userInfoNode);
    QSharedPointer<PostImage> getUserAvatar(QtGumboNode userInfoNode);
    User getPostUser(QtGumboNode trNode1);
    Post getPostValue(QtGumboNode trNode1);
    QString getPostLastEdit(QtGumboNode postEntryNode);
    QString getPostUserSignature(QtGumboNode postEntryNode);
    IPostObjectList getPostAttachments(QtGumboNode postEntryNode);
    int getLikeCounterValue(QtGumboNode trNode2);
    int getPostId(QtGumboNode msdivNode);
    void fillPostList(QtGumboNode node, UserPosts &posts);

    QSharedPointer<PostHyperlink> parseHyperlink(QtGumboNode aNode) const;
    QSharedPointer<PostImage> parseImage(QtGumboNode imgNode) const;
    QSharedPointer<PostQuote> parseQuote(QtGumboNode tableNode) const;
    void parseMessage(QtGumboNodes nodes, IPostObjectList& postObjects) const;

public:
    // IForumPageReader implementation
    virtual int getPagePosts(QString rawData, UserPosts& userPosts, int &pageCount) Q_DECL_OVERRIDE;
};

}

#endif // GUMBOPARSERIMPL_H

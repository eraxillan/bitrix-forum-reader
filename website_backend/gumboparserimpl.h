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

    QtGumboDocumentPtr m_htmlDocument;

    mutable bool m_textQuoteFlag = false;

private:

    void printTagsRecursively(QtGumboNodePtr node, int &level);
    void findMsdivNodesRecursively(QtGumboNodePtr node, QVector<QtGumboNodePtr> &msdivNodes);
    void findPageCount(QString rawData, int &pageCount);
    UserBaseInfo getUserBaseInfo(QtGumboNodePtr userInfoNode);
    UserAdditionalInfo getUserAdditionalInfo(QtGumboNodePtr userInfoNode);
     std::shared_ptr<PostImage> getUserAvatar(QtGumboNodePtr userInfoNode);
    UserPtr getPostUser(QtGumboNodePtr trNode1);
    PostPtr getPostValue(QtGumboNodePtr trNode1);
    QString getPostLastEdit(QtGumboNodePtr postEntryNode);
    QString getPostUserSignature(QtGumboNodePtr postEntryNode);
    IPostObjectList getPostAttachments(QtGumboNodePtr postEntryNode);
    int getLikeCounterValue(QtGumboNodePtr trNode2);
    int getPostId(QtGumboNodePtr msdivNode);
    void fillPostList(QtGumboNodePtr node, PostCollection &posts);

     std::shared_ptr<PostHyperlink> parseHyperlink(QtGumboNodePtr aNode) const;
     std::shared_ptr<PostImage> parseImage(QtGumboNodePtr imgNode) const;
     std::shared_ptr<PostQuote> parseQuote(QtGumboNodePtr tableNode) const;
     std::shared_ptr<PostSpoiler> parseSpoiler(QtGumboNodePtr tableNode) const;

    void parseMessage(QtGumboNodes nodes, IPostObjectList& postObjects) const;

public:
    // IForumPageReader implementation
    result_code::Type getPageCount(QByteArray rawData, int& pageCount) override;
    result_code::Type getPagePosts(QByteArray rawData, PostCollection& userPosts) override;
};

}

#endif // GUMBOPARSERIMPL_H

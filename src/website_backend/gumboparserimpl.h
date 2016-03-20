#ifndef GUMBOPARSERIMPL_H
#define GUMBOPARSERIMPL_H

#include <QtCore/QtCore>
#include "gumbo.h"
#include "src/website_backend/websiteinterface.h"

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
    void printTagsRecursively(GumboNode* node, int& level);
    void findMsdivNodesRecursively(GumboNode* node, QVector<GumboNode*>& msdivNodes);
    UserBaseInfo getUserBaseInfo(GumboNode* userInfoNode);
    UserAdditionalInfo getUserAdditionalInfo(GumboNode* userInfoNode);
    Image getUserAvatar(GumboNode* userInfoNode);
    User getPostUser(GumboNode* trNode1);
    Post getPostValue(GumboNode* trNode1);
    int getLikeCounterValue(GumboNode* trNode2);
    int getPostId(GumboNode* msdivNode);
    void fillPostList(GumboNode* node, UserPosts& posts);

public:
    // IForumPageReader implementation
    virtual int getPagePosts(QUrl webPageUrl, UserPosts& userPosts) override;
};

}

#endif // GUMBOPARSERIMPL_H

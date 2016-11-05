#include "forumreader.h"

#include "src/website_backend/gumboparserimpl.h"

ForumReader::ForumReader()
{
    // NOTE: for testing purposes
    //BankiRuForum::ForumPageParser fpp;
    //fpp.getPagePosts( QUrl::fromLocalFile("C:/Projects/__DATA/test_page_30.html"), m_userPosts );
}

ForumReader::~ForumReader()
{
}

ForumReader::ForumReader(BankiRuForum::UserPosts userPosts, QObject *parent) : QObject(parent), m_userPosts(userPosts), m_pageCount(0)
{
}

QUrl ForumReader::convertToUrl(QString urlStr) const
{
    return QUrl(urlStr);
}

bool ForumReader::parseForumPage(QString forumPageRawHtml)
{
    m_userPosts.clear();
    m_pageCount = 0;

    BankiRuForum::ForumPageParser fpp;
    int result = fpp.getPagePosts(forumPageRawHtml, m_userPosts, m_pageCount);
    return (result == 0);
}

int ForumReader::pageCount() const
{
    return m_pageCount;
}

int ForumReader::postCount() const
{
    return m_userPosts.size();
}

QString ForumReader::postAuthor(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_userName;
}

QString ForumReader::postAvatarUrl(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_userAvatar.m_url.toString();
}

int ForumReader::postAvatarWidth(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_userAvatar.m_width;
}

int ForumReader::postAvatarHeight(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_userAvatar.m_height;
}

int ForumReader::postAvatarMaxWidth() const
{
    int maxWidth = 100;
    for(int i = 0; i < m_userPosts.size(); ++i)
    {
        int width = m_userPosts[i].first.m_userAvatar.m_width;
        if(width > maxWidth) maxWidth = width;
    }
    return maxWidth;
}

QDateTime ForumReader::postDateTime(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_date;
}

QString ForumReader::postText(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_text;
}

QString ForumReader::postLastEdit(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_lastEdit;
}

int ForumReader::postLikeCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_likeCounter;
}

int ForumReader::postAuthorPostCount(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_postCount;
}

QDate ForumReader::postAuthorRegistrationDate(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_registrationDate;
}

int ForumReader::postAuthorReputation(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_reputation;
}

QString ForumReader::postAuthorCity(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_city;
}

QString ForumReader::postAuthorSignature(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_userSignature;
}

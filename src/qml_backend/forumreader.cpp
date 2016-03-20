#include "forumreader.h"

#include "src/website_backend/gumboparserimpl.h"

QHash<int, QByteArray> ForumModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    return roles;
}

ForumModel::ForumModel()
{
    //
}

ForumModel::~ForumModel()
{
    //
}

QVariant ForumModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
    {
        return QVariant();
    }
    if(role == NameRole)
    {
        //return QVariant(backing[index.row()]);
        return "Foking fok!";
    }
    return QVariant();
}

void ForumModel::activate(const int i)
{
    if(i < 0 || i >= backing.size())
    {
        return;
    }
    QString value = backing[i];

    // Remove the value from the old location.
    beginRemoveRows(QModelIndex(), i, i);
    backing.erase(backing.begin() + i);
    endRemoveRows();

    // Add it to the top.
    beginInsertRows(QModelIndex(), 0, 0);
    backing.insert(0, value);
    endInsertRows();
}

//-------------------------------------------------------------------------------------------------

ForumReader::ForumReader()
{
    BankiRuForum::ForumPageParser fpp;
    BankiRuForum::UserPosts userPosts;
    fpp.getPagePosts( QUrl::fromLocalFile("C:/Projects/__DATA/test_page_30.html"), m_userPosts );
}

ForumReader::~ForumReader()
{
}

ForumReader::ForumReader(BankiRuForum::UserPosts userPosts, QObject *parent) : QObject(parent), m_userPosts(userPosts)
{
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

QUrl ForumReader::postAvatarUrl(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].first.m_userAvatar.m_url;
}

QString ForumReader::postText(int index) const
{
    Q_ASSERT(index >= 0 && index < m_userPosts.size());

    return m_userPosts[index].second.m_text;
}

QUrl ForumReader::avatarUrlForUser(QString userName) const
{
    QUrl url;
    for(int i = 0; i < m_userPosts.size(); ++i)
    {
        BankiRuForum::User currentUser = m_userPosts[i].first;
        if( QString::compare( currentUser.m_userName, userName, Qt::CaseInsensitive ) == 0 )
        {
            url = currentUser.m_userAvatar.m_url;
            break;
        }
    }

//    qDebug() << "USER URL:" << url.toString();
    return url;
}

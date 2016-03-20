#ifndef FORUMREADER_H
#define FORUMREADER_H

#include <QObject>

#include "src/website_backend/websiteinterface.h"

class ForumModel : public QAbstractListModel
{
    Q_OBJECT
    QVector<QString> backing;

public:
    enum DemoRoles
    {
        NameRole = Qt::UserRole + 1,
    };

    ForumModel();
    //explicit ForumModel(QObject* parent = 0);
    ~ForumModel();

    virtual int rowCount(const QModelIndex&) const { return backing.size(); }
    virtual QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void activate(const int i);
};

//-------------------------------------------------------------------------------------------------

class ForumReader : public QObject
{
    Q_OBJECT

    BankiRuForum::UserPosts m_userPosts;

public:
    ForumReader();
    ~ForumReader();

    explicit ForumReader( BankiRuForum::UserPosts userPosts, QObject *parent = 0 );

    Q_INVOKABLE int postCount() const;
    Q_INVOKABLE QString postAuthor(int index) const;
    Q_INVOKABLE QUrl postAvatarUrl(int index) const;
    Q_INVOKABLE QString postText(int index) const;

    Q_INVOKABLE QUrl avatarUrlForUser(QString userName) const;
//    Q_INVOKABLE QString forumPostTextF
signals:

public slots:
};

#endif // FORUMREADER_H

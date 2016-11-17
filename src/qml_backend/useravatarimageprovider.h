#ifndef USERAVATARIMAGEPROVIDER_H
#define USERAVATARIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>

#include "src/network/filedownloader.h"
#include "src/website_backend/websiteinterface.h"

class UserAvatarImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT

    BankiRuForum::UserPosts m_userPosts;
    int m_idxCurrentUser = 0;
    FileDownloader* m_pImgCtrl = nullptr;
    QPixmap m_pxmUserAvatar;

public:
    UserAvatarImageProvider(BankiRuForum::UserPosts userPosts);

    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);
};

class UserAvatarImageResponse : public QQuickImageResponse, public QRunnable
{
    // FIXME: test whether this statement is required
    Q_OBJECT

    QString m_id;
    QSize m_requestedSize;
    QSharedPointer<BankiRuForum::PostImage> m_avatarInfo;
    QQuickTextureFactory* m_texture = nullptr;
    bool m_cancelled = false;
    FileDownloader* m_pImgCtrl = nullptr;

private slots:
    void loadImage();

public:
    UserAvatarImageResponse(const QString& id, const QSize& requestedSize, QSharedPointer<BankiRuForum::PostImage> avatarInfo);

    QQuickTextureFactory* textureFactory() const override;
    void run() override;
    void cancel() override;

    // FIXME: implement
    //virtual QString errorString() const;
};

class UserAvatarAsyncImageProvider: public QQuickAsyncImageProvider
{
//    Q_OBJECT

    BankiRuForum::UserPosts m_userPosts;

public:
    UserAvatarAsyncImageProvider(BankiRuForum::UserPosts userPosts);
    QQuickImageResponse* requestImageResponse(const QString& id, const QSize& requestedSize) override;
};

#endif // USERAVATARIMAGEPROVIDER_H

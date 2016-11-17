
#include "useravatarimageprovider.h"

static QImage fallbackImage()
{
    QImage fallback;
    fallback.load("pics/qtlogo.png");
//    *realSize = fallback.size();
    return fallback;
}

void UserAvatarImageResponse::loadImage()
{
    QImage image;
    bool loadResult = image.loadFromData(m_pImgCtrl->downloadedData());
    if (!loadResult || image.isNull()) image = fallbackImage();

    m_texture = QQuickTextureFactory::textureFactoryForImage(image);

    emit finished();
}


UserAvatarImageResponse::UserAvatarImageResponse(const QString& id, const QSize& requestedSize, QSharedPointer<BankiRuForum::PostImage> avatarInfo):
    m_id(id), m_requestedSize(requestedSize), m_avatarInfo(avatarInfo), m_texture(nullptr), m_cancelled(false)
{
    setAutoDelete(false);
}

QQuickTextureFactory* UserAvatarImageResponse::textureFactory() const
{
    return m_texture;
}

void UserAvatarImageResponse::run()
{
    if (m_cancelled)
    {
        qDebug() << "Running cancelled user avatar:" << m_id;
        emit finished();
        return;
    }

    qDebug() << "Running user avatar" << m_id;

    m_pImgCtrl = new FileDownloader( QUrl( "https://upload.wikimedia.org/wikipedia/en/5/5a/Qt_Creator_logo.png" ) /*m_avatarInfo.m_url*/, NULL /*this*/);
    //connect(m_pImgCtrl, SIGNAL(downloaded()), this, SLOT(loadImage()));
}

void UserAvatarImageResponse::cancel()
{
    m_cancelled = true;
    qDebug() << "CANCEL ALBUM ART" << m_id;
}

// FIXME: implement
//virtual QString UserAvatarImageResponse::errorString() const;

//-------------------------------------------------------------------------------------------------

UserAvatarAsyncImageProvider::UserAvatarAsyncImageProvider(BankiRuForum::UserPosts userPosts) : m_userPosts( userPosts )
{
}

QQuickImageResponse* UserAvatarAsyncImageProvider::requestImageResponse(const QString& id, const QSize& requestedSize)
{
    qDebug() << "Loading user avatar image" << id;

    // Search for user with specified name
    QSharedPointer<BankiRuForum::PostImage> avatarInfo;
    for( int i = 0; i < m_userPosts.size(); ++i )
    {
        auto currentUser = m_userPosts[i].first;
        if( QString::compare( currentUser.m_userName, id, Qt::CaseInsensitive ) == 0 )
        {
            qDebug() << "Avatar info for user" << id << "was found successfully";
            avatarInfo = currentUser.m_userAvatar;
            break;
        }
    }

    // FIXME: should it be started MANUALLY?
    UserAvatarImageResponse* response = new UserAvatarImageResponse(id, requestedSize, avatarInfo);
    QThreadPool::globalInstance()->start( response );
    return response;
}

//------------------------------------------------------------------------------------------------

UserAvatarImageProvider::UserAvatarImageProvider(BankiRuForum::UserPosts userPosts) : QQuickImageProvider(QQuickImageProvider::Pixmap), m_userPosts(userPosts)
{
}

QPixmap UserAvatarImageProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    Q_UNUSED(id);

    QSharedPointer<BankiRuForum::PostImage> imgAvatar = m_userPosts[m_idxCurrentUser].first.m_userAvatar;
    if (size)
    {
        *size = QSize(imgAvatar->m_width, imgAvatar->m_height);
    }

    // FIXME: use real avatar id
    QUrl imageUrl("https://upload.wikimedia.org/wikipedia/en/5/5a/Qt_Creator_logo.png");
    m_pImgCtrl = new FileDownloader(imageUrl, this);

//    connect(m_pImgCtrl, SIGNAL(downloaded()), this, SLOT(loadImage()));

    if (requestedSize.width() > 0 && requestedSize.height())
        m_pxmUserAvatar = m_pxmUserAvatar.scaled(requestedSize);
    return m_pxmUserAvatar;
}

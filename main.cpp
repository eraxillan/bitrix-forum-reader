
#include <QtCore>
#include <QtQml>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickImageProvider>
/*
#include "C:/Projects/gumbo-parser/src/gumbo.h"
#ifdef _WIN64
#pragma comment( lib, "C:/Projects/gumbo-parser/visualc/x64/Debug/gumbo.lib" )
#elif _WIN32
#pragma comment( lib, "C:/Projects/gumbo-parser/visualc/Debug/gumbo.lib" )
#endif
*/

#include "src/website_backend/gumboparserimpl.h"
#include "src/qml_backend/useravatarimageprovider.h"
#include "src/qml_backend/forumreader.h"

// FIXME: save full post history to the LOCAL database
// FIXME: add sorting by user/post reputation option

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<ForumReader>("ru.banki.reader", 1, 0, "ForumReader");
    qmlRegisterType<ForumModel>( "ru.banki.reader", 1, 0, "ForumModel");

	//
    /*for (auto up : userPosts)
	{
		if (!up.first.m_userAvatar.isValid()) continue;

		qDebug() << "Avatar URL:" << up.first.m_userAvatar.m_url;
		qDebug() << "Avatar size:" << up.first.m_userAvatar.m_width << "x" << up.first.m_userAvatar.m_height;
    }*/
	//

    QQmlApplicationEngine engine;
//    engine.addImageProvider(QLatin1String("avatars"), new UserAvatarImageProvider(userPosts));
//    engine.addImageProvider( QLatin1String("avatars"), new UserAvatarAsyncImageProvider(userPosts) );
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
